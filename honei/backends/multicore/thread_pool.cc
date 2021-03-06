/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2008 - 2012 Sven Mallach <mallach@honei.org>
 *
 * This file is part of the HONEI C++ library. HONEI is free software;
 * you can redistribute it and/or modify it under the terms of the GNU General
 * Public License version 2, as published by the Free Software Foundation.
 *
 * HONEI is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <honei/backends/multicore/thread_pool.hh>
#include <honei/backends/multicore/thread_function.hh>
#include <honei/backends/multicore/topology.hh>
#include <honei/util/configuration.hh>
#include <honei/util/exception.hh>
#include <honei/util/instantiation_policy-impl.hh>
#include <honei/util/private_implementation_pattern-impl.hh>
#include <honei/util/lock.hh>
#include <honei/util/log.hh>
#include <honei/util/stringify.hh>
#include <honei/util/thread.hh>

#include <errno.h>
#include <sched.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <unistd.h>

namespace honei
{
    template <> struct Implementation<mc::ThreadPool>
    {
        /// Information about processor topology (such as number of processing units)
        mc::Topology * const _topology;

        /// Number of demanded (via honeirc) and pooled threads
        const unsigned _num_threads;

        /// Whether to use thread affinity
        const bool _affinity;

        /// Whether to use linear or alternating thread assignment (only with affinity)
        /// 0 = socket-linear assignment, 1 = socket-alternating assignment
        const int _assign_policy;

        /// List of user POSIX threads
        std::list<Thread *> _threads;

        /// List of thread specific data
        std::list<mc::ThreadData *> _thread_data;

        /// Exchange data between the pool and its threads
        mc::PoolSyncData * const _pool_sync;

        /// Function pointer to any of the default dispatch strategies
        mc::DispatchPolicy (* policy) ();

        Implementation() :
            _topology(mc::Topology::instance()),
            _num_threads(Configuration::instance()->get_value("mc::num_threads", _topology->num_lpus())),
            _affinity(Configuration::instance()->get_value("mc::affinity", true)),
            _assign_policy(Configuration::instance()->get_value("mc::thread_assignment", 0)),
            _pool_sync(new mc::PoolSyncData)
        {
            CONTEXT("When initializing the basic implementation of thread pool:");

#ifdef DEBUG
            std::string msg = "Affinity is ";
            msg += (_affinity ? "enabled.\n" : "disabled.\n");
            msg += "The default DispatchPolicy is configured to be: \n";
#endif
            std::string dis = Configuration::instance()->get_value("mc::dispatch", "anycore");

            if (! _affinity || dis == "anycore")
            {
                policy = &mc::DispatchPolicy::any_core;
#ifdef DEBUG
                msg += "arbitrary - the next available thread will execute a task\n";
#endif
            }
            else if (dis == "alternating")
            {
                policy = &mc::DispatchPolicy::alternating_socket;
#ifdef DEBUG
                msg += "alternating - tasks shall be assigned alternatingly on available (NUMA) sockets\n";
#endif
            }
            else if (dis == "linear")
            {
                policy = &mc::DispatchPolicy::linear_socket;
#ifdef DEBUG
                msg += "linear - tasks shall be assigned to fill up available (NUMA) sockets one-by-one\n";
#endif
            }

#ifdef DEBUG
            msg += "Will create " + stringify(_num_threads) + " POSIX worker threads\n";
            LOGMESSAGE(lc_backend, msg);
#endif
        }

        virtual ~Implementation()
        {
            std::list<mc::ThreadData *>::iterator j(_thread_data.begin());
            for(std::list<Thread *>::iterator i(_threads.begin()), i_end(_threads.end()) ; i != i_end ; ++i, ++j)
            {
                {
                    Lock l(*_pool_sync->mutex);
                    (*j)->terminate = true;
                    _pool_sync->barrier->broadcast();
                }

                delete (*i);
                delete (*j);
            }

            delete _pool_sync;
        }

        /// Used only from Implementations with affinity...
        virtual void set_lpu_successors()
        {
            CONTEXT("When setting the LPU enqueue successors:");

#ifdef DEBUG
            std::string msg;
#endif
            for (unsigned s(0) ; s < _topology->num_cpus() ; ++s)
            {
                LPU * first_on_socket(NULL);

                unsigned i(0);
                for ( ; i < _topology->num_lpus() ; ++i)
                {
                    if (_topology->lpu(i)->socket_id == (int) s && _topology->lpu(i)->has_thread)
                    {
                        first_on_socket = _topology->lpu(i);
                        ++i;
                        break;
                    }
                }

                if (first_on_socket != NULL)
                {
                    LPU * last_with_thread = first_on_socket;
                    LPU * succ(last_with_thread->linear_succ);

                    while (succ != _topology->lpu(0))
                    {
                        if (succ->has_thread && succ->socket_id == (int) s)
                        {
#ifdef DEBUG
                            msg += "Setting linear enqueue successor of LPU " + stringify(last_with_thread->sched_id) + " to " + stringify(succ->sched_id) + " \n";
#endif

                            last_with_thread->linear_enqueue_succ = succ;
                            last_with_thread = succ;
                        }

                        succ = succ->linear_succ;
                    }

                    last_with_thread->linear_enqueue_succ = first_on_socket;
#ifdef DEBUG
                    msg += "Setting linear enqueue successor of LPU " + stringify(last_with_thread->sched_id) + " to " + stringify(first_on_socket->sched_id) + " \n";
#endif
                }

            }


            LPU * last_with_thread = _topology->lpu(0);
            LPU * succ = last_with_thread->alternating_succ;

            while (succ != _topology->lpu(0))
            {
                if (succ->has_thread)
                {
#ifdef DEBUG
                    msg += "Setting alternating enqueue successor of LPU " + stringify(last_with_thread->sched_id) + " to " + stringify(succ->sched_id) + " \n";
#endif
                    last_with_thread->alternating_enqueue_succ = succ;
                    last_with_thread = succ;
                }

                succ = succ->alternating_succ;
            }

            last_with_thread->alternating_enqueue_succ = _topology->lpu(0);
#ifdef DEBUG
            msg += "Setting alternating enqueue successor of LPU " + stringify(last_with_thread->sched_id) + " to " + stringify(_topology->lpu(0)->sched_id) + " \n";

            LOGMESSAGE(lc_backend, msg);
#endif

        }

        virtual Ticket<tags::CPU::MultiCore> enqueue(const function<void ()> & task, mc::DispatchPolicy p) = 0;
        virtual Ticket<tags::CPU::MultiCore> enqueue(const function<void ()> & task) = 0;
    };

    /* StandardImplementation assumes affinity to be disabled. */

    template <typename ListType> struct StandardImplementation;

    template <> struct StandardImplementation<mc::CASDeque<mc::ThreadTask *> > :
        public Implementation<mc::ThreadPool>
    {
        /// Waiting list of worker tasks to be executed (otherwise)
        mc::CASDeque<mc::ThreadTask *> _tasks;

        StandardImplementation() :
            Implementation<mc::ThreadPool>()
        {
            CONTEXT("When initializing the standard implementation of thread pool:");

            for (unsigned i(0) ; i < _num_threads ; ++i)
            {
                mc::ThreadData * td = new mc::ThreadData;
                mc::SimpleThreadFunction<mc::CASDeque<mc::ThreadTask *> > tobj(_pool_sync, td, &_tasks, i);

                Thread * t;
                {
                    Lock l(*_pool_sync->mutex);
                    t = new Thread(tobj); // tobj will be copied here!
                    _pool_sync->barrier->wait(*_pool_sync->mutex); // Wait until the thread is really setup / got cpu time for the first time
                }

                _threads.push_back(t);
                _thread_data.push_back(td);
            }
        }

        ~StandardImplementation()
        {
        }

        virtual Ticket<tags::CPU::MultiCore> enqueue(const function<void ()> & task, mc::DispatchPolicy)
        {
            return enqueue(task);
        }

        virtual Ticket<tags::CPU::MultiCore> enqueue(const function<void ()> & task)
        {
            CONTEXT("When creating a ThreadTask:");

            Ticket<tags::CPU::MultiCore> ticket(policy().apply());
            mc::ThreadTask * t_task(new mc::ThreadTask(task, ticket));

            _tasks.push_back(t_task);

            {
                Lock l(*_pool_sync->mutex);
                _pool_sync->barrier->broadcast();
            }

            return ticket;
        }
    };

    template <> struct StandardImplementation<mc::ConcurrentDeque<mc::ThreadTask *> > :
        public Implementation<mc::ThreadPool>
    {
        /// Waiting list of worker tasks to be executed (otherwise)
        mc::ConcurrentDeque<mc::ThreadTask *> _tasks;

        StandardImplementation() :
            Implementation<mc::ThreadPool>()
        {
            CONTEXT("When initializing the standard implementation of thread pool:");

            for (unsigned i(0) ; i < _num_threads ; ++i)
            {
                mc::ThreadData * td = new mc::ThreadData;
                mc::SimpleThreadFunction<mc::ConcurrentDeque<mc::ThreadTask *> > tobj(_pool_sync, td, &_tasks, i);

                Thread * t;
                {
                    Lock l(*_pool_sync->mutex);
                    t = new Thread(tobj); // tobj will be copied here!
                    _pool_sync->barrier->wait(*_pool_sync->mutex); // Wait until the thread is really setup / got cpu time for the first time
                }

                _threads.push_back(t);
                _thread_data.push_back(td);
            }
        }

        ~StandardImplementation()
        {
        }

        virtual Ticket<tags::CPU::MultiCore> enqueue(const function<void ()> & task, mc::DispatchPolicy)
        {
            return enqueue(task);
        }

        virtual Ticket<tags::CPU::MultiCore> enqueue(const function<void ()> & task)
        {
            CONTEXT("When creating a ThreadTask:");

            Ticket<tags::CPU::MultiCore> ticket(policy().apply());
            mc::ThreadTask * t_task(new mc::ThreadTask(task, ticket));

            _tasks.push_back(t_task);

            {
                Lock l(*_pool_sync->mutex);
                _pool_sync->barrier->broadcast();
            }

            return ticket;
        }
    };

    template <> struct StandardImplementation<std::deque<mc::ThreadTask *> > :
        public Implementation<mc::ThreadPool>
    {
        /// Waiting list of worker tasks to be executed (otherwise)
        std::deque<mc::ThreadTask *> _tasks;

        StandardImplementation() :
            Implementation<mc::ThreadPool>()
        {
            CONTEXT("When initializing the standard implementation of thread pool:");

            for (unsigned i(0) ; i < _num_threads ; ++i)
            {
                mc::ThreadData * td = new mc::ThreadData;
                mc::SimpleThreadFunction<std::deque<mc::ThreadTask *> > tobj(_pool_sync, td, &_tasks, i);
                Thread * t;
                {
                    Lock l(*_pool_sync->mutex);
                    t = new Thread(tobj); // tobj will be copied here!
                    _pool_sync->barrier->wait(*_pool_sync->mutex); // Wait until the thread is really setup / got cpu time for the first time
                }

                _threads.push_back(t);
                _thread_data.push_back(td);
            }
        }

        ~StandardImplementation()
        {
        }

        virtual Ticket<tags::CPU::MultiCore> enqueue(const function<void ()> & task, mc::DispatchPolicy)
        {
            return enqueue(task);
        }

        virtual Ticket<tags::CPU::MultiCore> enqueue(const function<void ()> & task)
        {
            CONTEXT("When creating a ThreadTask:");

            Ticket<tags::CPU::MultiCore> ticket(policy().apply());
            mc::ThreadTask * t_task(new mc::ThreadTask(task, ticket));

            {
                Lock l(*_pool_sync->mutex);
                _tasks.push_back(t_task);
                _pool_sync->barrier->broadcast();
            }

            return ticket;
        }
    };

    struct AffinityImplementation :
        public Implementation<mc::ThreadPool>
    {
        /// Waiting list of worker tasks to be executed (if affinity is enabled)
        std::deque<mc::ThreadTask *> _tasks;

        /// Array of affinity masks for main process and all controlled threads
        cpu_set_t * _affinity_mask;

        AffinityImplementation() :
            Implementation<mc::ThreadPool>(),
            _affinity_mask(new cpu_set_t[_num_threads + 1])
        {
            CONTEXT("When initializing the affinity implementation of thread pool:");

            LPU * lpu = _topology->lpu(0);

            // set main threads' affinity first
            CPU_ZERO(&_affinity_mask[_num_threads]);
            CPU_SET(lpu->sched_id, &_affinity_mask[_num_threads]);
            if(sched_setaffinity(syscall(__NR_gettid), sizeof(cpu_set_t), &_affinity_mask[_num_threads]) != 0)
                throw ExternalError("Unix: sched_setaffinity()", "could not set affinity! errno: " + stringify(errno));

#ifdef DEBUG
            std::string msg = "THREAD \t\t POOL_ID \t LPU \t NODE \n";
            msg += "MAIN \t\t - \t\t" + stringify(lpu->sched_id) + "\t\t" + stringify(lpu->socket_id) + " \n";
#endif

            for (unsigned i(0) ; i < _num_threads ; ++i)
            {
                mc::ThreadData * td = new mc::ThreadData;

                mc::AffinityThreadFunction tobj(_pool_sync, td, &_tasks, i, lpu);

                Thread * t;
                {
                    Lock l(*_pool_sync->mutex);
                    t = new Thread(tobj); // tobj will be copied here!
                    _pool_sync->barrier->wait(*_pool_sync->mutex); // Wait until the thread is really setup / got cpu time for the first time
                }

                _threads.push_back(t);
                _thread_data.push_back(td);

                CPU_ZERO(&_affinity_mask[i]);
                CPU_SET(lpu->sched_id, &_affinity_mask[i]);
                if(sched_setaffinity(tobj.tid(), sizeof(cpu_set_t), &_affinity_mask[i]) != 0)
                    throw ExternalError("Unix: sched_setaffinity()", "could not set affinity! errno: " + stringify(errno));
#ifdef DEBUG
                msg += stringify(tobj.tid()) + "\t\t" + stringify(i) + "\t\t" + stringify(lpu->sched_id) + "\t\t" + stringify(lpu->socket_id) + " \n";
#endif

                if (! lpu->has_thread) // Could otherwise cause problems
                //- assigning the same LPU more than once to the array threaded_lpus due to multiple threads on it can possibly exceed the arrays size
                {
                    lpu->has_thread = true;
                    Socket * sock = _topology->sockets()[lpu->socket_id];
                    sock->_threaded_lpus[sock->_num_threads] = lpu;
                    ++sock->_num_threads;
                }

                switch (_assign_policy)
                {
                    case 0:
                        lpu = lpu->linear_succ;
                        break;

                    case 1:
                        lpu = lpu->alternating_succ;
                }
            }

            set_lpu_successors();

#ifdef DEBUG
            LOGMESSAGE(lc_backend, msg);
#endif
        }

        ~AffinityImplementation()
        {
            delete[] _affinity_mask;
        }


        virtual Ticket<tags::CPU::MultiCore> enqueue(const function<void ()> & task, mc::DispatchPolicy p)
        {
            CONTEXT("When creating a ThreadTask:");

            Ticket<tags::CPU::MultiCore> ticket(p.apply());
            mc::ThreadTask * t_task(new mc::ThreadTask(task, ticket));

            {
                Lock l(*_pool_sync->mutex);
                _tasks.push_back(t_task);
                _pool_sync->barrier->broadcast();
            }

            return ticket;
        }

        virtual Ticket<tags::CPU::MultiCore> enqueue(const function<void ()> & task)
        {
            CONTEXT("When creating a ThreadTask:");

            Ticket<tags::CPU::MultiCore> ticket(policy().apply());
            mc::ThreadTask * t_task(new mc::ThreadTask(task, ticket));

            {
                Lock l(*_pool_sync->mutex);
                _tasks.push_back(t_task);
                _pool_sync->barrier->broadcast();
            }

            return ticket;
        }
    };

    template <typename ListType> struct WorkStealingImplementation;

    template <> struct WorkStealingImplementation<std::deque<mc::ThreadTask *> > :
        public Implementation<mc::ThreadPool>
    {
        /// List of user POSIX threads
        std::vector<mc::WorkStealingThreadFunction<std::deque<mc::ThreadTask *> > *> _thread_fn;

        /// Array of affinity masks for main process and all controlled threads
        cpu_set_t * _affinity_mask;

        volatile bool global_terminate;

        WorkStealingImplementation() :
            Implementation<mc::ThreadPool>(),
            _affinity_mask(NULL),
            global_terminate(false)
        {
            CONTEXT("When initializing the work stealing implementation thread pool:");

#ifdef DEBUG
            std::string msg;
#endif

            LPU * lpu = _topology->lpu(0);

            if (_affinity)
            {
                _affinity_mask = new cpu_set_t[_num_threads + 1];

                // set main threads' affinity first
                CPU_ZERO(&_affinity_mask[_num_threads]);
                CPU_SET(lpu->sched_id, &_affinity_mask[_num_threads]);
                if(sched_setaffinity(syscall(__NR_gettid), sizeof(cpu_set_t), &_affinity_mask[_num_threads]) != 0)
                    throw ExternalError("Unix: sched_setaffinity()", "could not set affinity! errno: " + stringify(errno));

#ifdef DEBUG
               std::string msg = "THREAD \t\t POOL_ID \t LPU \t NODE \n";
               msg += "MAIN \t\t - \t\t" + stringify(lpu->sched_id) + "\t\t" + stringify(lpu->socket_id) + " \n";
#endif
            }

            Lock l(*_pool_sync->steal_mutex); // Prevent threads from stealing before all threads are alive

            for (unsigned i(0) ; i < _num_threads ; ++i)
            {
                unsigned sched_id(_affinity ? lpu->sched_id : 0xFFFF );

                mc::ThreadData * td = new mc::ThreadData;

                mc::WorkStealingThreadFunction<std::deque<mc::ThreadTask *> > * tobj =
                    new mc::WorkStealingThreadFunction<std::deque<mc::ThreadTask *> >(_pool_sync, td, i, sched_id,
                            _thread_fn, _num_threads, global_terminate);

                Thread * t;
                {
                    Lock l(*_pool_sync->mutex);
                    t = new Thread(*tobj); // tobj will be copied here!
                    _pool_sync->barrier->wait(*_pool_sync->mutex); // Wait until the thread is really setup / got cpu time for the first time
                }

                _threads.push_back(t);
                _thread_data.push_back(td);
                _thread_fn.push_back(tobj);

                if (_affinity)
                {
                    CPU_ZERO(&_affinity_mask[i]);
                    CPU_SET(sched_id, &_affinity_mask[i]);
                    if(sched_setaffinity(tobj->tid(), sizeof(cpu_set_t), &_affinity_mask[i]) != 0)
                        throw ExternalError("Unix: sched_setaffinity()", "could not set affinity! errno: " + stringify(errno));
#ifdef DEBUG
                    msg += stringify(tobj->tid()) + "\t\t" + stringify(i) + "\t\t" + stringify(sched_id) + "\t\t" + stringify(lpu->socket_id) + " \n";
#endif

                    if (! lpu->has_thread) // Could otherwise cause problems
                    //- assigning the same LPU more than once to the array threaded_lpus due to multiple threads on it can possibly exceed the arrays size
                    {
                        lpu->has_thread = true;
                        Socket * sock = _topology->sockets()[lpu->socket_id];
                        sock->_threaded_lpus[sock->_num_threads] = lpu;
                        ++sock->_num_threads;
                    }

                    switch (_assign_policy)
                    {
                        case 0:
                            lpu = lpu->linear_succ;
                            break;

                        case 1:
                            lpu = lpu->alternating_succ;
                    }
                }
            }

            if (_affinity)
                set_lpu_successors();

#ifdef DEBUG
            LOGMESSAGE(lc_backend, msg);
#endif
        }

        ~WorkStealingImplementation()
        {
            {
                Lock l(*_pool_sync->steal_mutex);
                global_terminate = true;
            }

            // This will delete our copies - Thread has an own one!
            for (std::vector<mc::WorkStealingThreadFunction<std::deque<mc::ThreadTask *> > *>::iterator i(_thread_fn.begin()),
                    i_end(_thread_fn.end()) ; i != i_end ; ++i)
            {
                delete (*i);
            }

            delete[] _affinity_mask;
        }

        virtual Ticket<tags::CPU::MultiCore> enqueue(const function<void ()> & task, mc::DispatchPolicy p)
        {
            CONTEXT("When creating a ThreadTask:");

            Ticket<tags::CPU::MultiCore> ticket(p.apply());
            mc::ThreadTask * t_task(new mc::ThreadTask(task, ticket));

            int idx(-1);
            int req_sched(ticket.req_sched());

            if (! _affinity)
            {
                idx = rand() % _num_threads;
            }
            else
            {
                if (req_sched != 0xFFFF)
                {
                    idx = req_sched;
                }
                else // req_sched == 0xFFFF
                {
                    int req_socket(ticket.req_socket());

                    if (req_socket == 0xFFFF)
                    {
                        idx = rand() % _num_threads;
                    }
                    else
                    {
                        if (_topology->sockets()[req_socket]->_num_threads > 0)
                            idx = _topology->sockets()[req_socket]->_threaded_lpus[rand() % _topology->sockets()[req_socket]->_num_threads]->sched_id;
                        else
                            idx = rand() % _num_threads;
                    }
                }
            }

            mc::WorkStealingThreadFunction<std::deque<mc::ThreadTask *> > * wfunc(_thread_fn[idx]);
            wfunc->enqueue(t_task);

            {
                Lock l(*_pool_sync->mutex);
                _pool_sync->barrier->broadcast();
            }

            return ticket;
        }

        virtual Ticket<tags::CPU::MultiCore> enqueue(const function<void ()> & task)
        {
            CONTEXT("When creating a ThreadTask:");

            Ticket<tags::CPU::MultiCore> ticket(policy().apply());
            mc::ThreadTask * t_task(new mc::ThreadTask(task, ticket));

            int idx(-1);
            int req_sched(ticket.req_sched());

            if (! _affinity)
            {
                idx = rand() % _num_threads;
            }
            else
            {
                if (req_sched != 0xFFFF)
                {
                    idx = req_sched;
                }
                else // req_sched == 0xFFFF
                {
                    int req_socket(ticket.req_socket());

                    if (req_socket == 0xFFFF)
                    {
                        idx = rand() % _num_threads;
                    }
                    else
                    {
                        if (_topology->sockets()[req_socket]->_num_threads > 0)
                            idx = _topology->sockets()[req_socket]->_threaded_lpus[rand() % _topology->sockets()[req_socket]->_num_threads]->sched_id;
                        else
                            idx = rand() % _num_threads;
                    }
                }
            }

            mc::WorkStealingThreadFunction<std::deque<mc::ThreadTask *> > * wfunc(_thread_fn[idx]);
            wfunc->enqueue(t_task);

            {
                Lock l(*_pool_sync->mutex);
                _pool_sync->barrier->broadcast();
            }

            return ticket;
        }
    };

    template <> struct WorkStealingImplementation<mc::ConcurrentDeque<mc::ThreadTask *> > :
        public Implementation<mc::ThreadPool>
    {
        /// List of user POSIX threads
        std::vector<mc::WorkStealingThreadFunction<mc::ConcurrentDeque<mc::ThreadTask *> > *> _thread_fn;

        /// Array of affinity masks for main process and all controlled threads
        cpu_set_t * _affinity_mask;

        volatile bool global_terminate;

        WorkStealingImplementation() :
            Implementation<mc::ThreadPool>(),
            _affinity_mask(NULL),
            global_terminate(false)
        {
            CONTEXT("When initializing the work stealing implementation thread pool:");

#ifdef DEBUG
            std::string msg;
#endif

            LPU * lpu = _topology->lpu(0);

            if (_affinity)
            {
                _affinity_mask = new cpu_set_t[_num_threads + 1];

                // set main threads' affinity first
                CPU_ZERO(&_affinity_mask[_num_threads]);
                CPU_SET(lpu->sched_id, &_affinity_mask[_num_threads]);
                if(sched_setaffinity(syscall(__NR_gettid), sizeof(cpu_set_t), &_affinity_mask[_num_threads]) != 0)
                    throw ExternalError("Unix: sched_setaffinity()", "could not set affinity! errno: " + stringify(errno));

#ifdef DEBUG
               std::string msg = "THREAD \t\t POOL_ID \t LPU \t NODE \n";
               msg += "MAIN \t\t - \t\t" + stringify(lpu->sched_id) + "\t\t" + stringify(lpu->socket_id) + " \n";
#endif
            }

            Lock l(*_pool_sync->steal_mutex); // Prevent threads from stealing before all threads are alive

            for (unsigned i(0) ; i < _num_threads ; ++i)
            {
                unsigned sched_id(_affinity ? lpu->sched_id : 0xFFFF);

                mc::ThreadData * td = new mc::ThreadData;

                mc::WorkStealingThreadFunction<mc::ConcurrentDeque<mc::ThreadTask *> > * tobj =
                    new mc::WorkStealingThreadFunction<mc::ConcurrentDeque<mc::ThreadTask *> >(_pool_sync, td, i, sched_id,
                            _thread_fn, _num_threads, global_terminate);

                Thread * t;
                {
                    Lock l(*_pool_sync->mutex);
                    t = new Thread(*tobj); // tobj will be copied here!
                    _pool_sync->barrier->wait(*_pool_sync->mutex); // Wait until the thread is really setup / got cpu time for the first time
                }

                _threads.push_back(t);
                _thread_data.push_back(td);
                _thread_fn.push_back(tobj);

                if (_affinity)
                {
                    CPU_ZERO(&_affinity_mask[i]);
                    CPU_SET(sched_id, &_affinity_mask[i]);
                    if(sched_setaffinity(tobj->tid(), sizeof(cpu_set_t), &_affinity_mask[i]) != 0)
                        throw ExternalError("Unix: sched_setaffinity()", "could not set affinity! errno: " + stringify(errno));
#ifdef DEBUG
                    msg += stringify(tobj->tid()) + "\t\t" + stringify(i) + "\t\t" + stringify(sched_id) + "\t\t" + stringify(lpu->socket_id) + " \n";
#endif

                    if (! lpu->has_thread) // Could otherwise cause problems
                    //- assigning the same LPU more than once to the array threaded_lpus due to multiple threads on it can possibly exceed the arrays size
                    {
                        lpu->has_thread = true;
                        Socket * sock = _topology->sockets()[lpu->socket_id];
                        sock->_threaded_lpus[sock->_num_threads] = lpu;
                        ++sock->_num_threads;
                    }

                    switch (_assign_policy)
                    {
                        case 0:
                            lpu = lpu->linear_succ;
                            break;

                        case 1:
                            lpu = lpu->alternating_succ;
                    }
                }
            }

            if (_affinity)
                set_lpu_successors();

#ifdef DEBUG
            LOGMESSAGE(lc_backend, msg);
#endif
        }

        ~WorkStealingImplementation()
        {
            {
                Lock l(*_pool_sync->steal_mutex);
                global_terminate = true;
            }

            // This will delete our copies - Thread has an own one!
            for (std::vector<mc::WorkStealingThreadFunction<mc::ConcurrentDeque<mc::ThreadTask *> > *>::iterator i(_thread_fn.begin()),
                    i_end(_thread_fn.end()) ; i != i_end ; ++i)
            {
                delete (*i);
            }

            delete[] _affinity_mask;
        }

        virtual Ticket<tags::CPU::MultiCore> enqueue(const function<void ()> & task, mc::DispatchPolicy p)
        {
            CONTEXT("When creating a ThreadTask:");

            Ticket<tags::CPU::MultiCore> ticket(p.apply());
            mc::ThreadTask * t_task(new mc::ThreadTask(task, ticket));

            int idx(-1);
            int req_sched(ticket.req_sched());

            if (! _affinity)
            {
                idx = rand() % _num_threads;
            }
            else
            {
                if (req_sched != 0xFFFF)
                {
                    idx = req_sched;
                }
                else // req_sched == 0xFFFF
                {
                    int req_socket(ticket.req_socket());

                    if (req_socket == 0xFFFF)
                    {
                        idx = rand() % _num_threads;
                    }
                    else
                    {
                        if (_topology->sockets()[req_socket]->_num_threads > 0)
                            idx = _topology->sockets()[req_socket]->_threaded_lpus[rand() % _topology->sockets()[req_socket]->_num_threads]->sched_id;
                        else
                            idx = rand() % _num_threads;
                    }
                }
            }

            mc::WorkStealingThreadFunction<mc::ConcurrentDeque<mc::ThreadTask *> > * wfunc(_thread_fn[idx]);
            wfunc->enqueue(t_task);

            {
                Lock l(*_pool_sync->mutex);
                _pool_sync->barrier->broadcast();
            }

            return ticket;
        }

        virtual Ticket<tags::CPU::MultiCore> enqueue(const function<void ()> & task)
        {
            CONTEXT("When creating a ThreadTask:");

            Ticket<tags::CPU::MultiCore> ticket(policy().apply());
            mc::ThreadTask * t_task(new mc::ThreadTask(task, ticket));

            int idx(-1);
            int req_sched(ticket.req_sched());

            if (! _affinity)
            {
                idx = rand() % _num_threads;
            }
            else
            {
                if (req_sched != 0xFFFF)
                {
                    idx = req_sched;
                }
                else // req_sched == 0xFFFF
                {
                    int req_socket(ticket.req_socket());

                    if (req_socket == 0xFFFF)
                    {
                        idx = rand() % _num_threads;
                    }
                    else
                    {
                        if (_topology->sockets()[req_socket]->_num_threads > 0)
                            idx = _topology->sockets()[req_socket]->_threaded_lpus[rand() % _topology->sockets()[req_socket]->_num_threads]->sched_id;
                        else
                            idx = rand() % _num_threads;
                    }
                }
            }

            mc::WorkStealingThreadFunction<mc::ConcurrentDeque<mc::ThreadTask *> > * wfunc(_thread_fn[idx]);
            wfunc->enqueue(t_task);

            {
                Lock l(*_pool_sync->mutex);
                _pool_sync->barrier->broadcast();
            }

            return ticket;
        }
    };

    template <> struct WorkStealingImplementation<mc::CASDeque<mc::ThreadTask *> > :
        public Implementation<mc::ThreadPool>
    {
        /// List of user POSIX threads
        std::vector<mc::WorkStealingThreadFunction<mc::CASDeque<mc::ThreadTask *> > *> _thread_fn;

        /// Array of affinity masks for main process and all controlled threads
        cpu_set_t * _affinity_mask;

        volatile bool global_terminate;

        WorkStealingImplementation() :
            Implementation<mc::ThreadPool>(),
            _affinity_mask(NULL),
            global_terminate(false)
        {
            CONTEXT("When initializing the work stealing implementation thread pool:");

#ifdef DEBUG
            std::string msg;
#endif

            LPU * lpu = _topology->lpu(0);

            if (_affinity)
            {
                _affinity_mask = new cpu_set_t[_num_threads + 1];

                // set main threads' affinity first
                CPU_ZERO(&_affinity_mask[_num_threads]);
                CPU_SET(lpu->sched_id, &_affinity_mask[_num_threads]);
                if(sched_setaffinity(syscall(__NR_gettid), sizeof(cpu_set_t), &_affinity_mask[_num_threads]) != 0)
                    throw ExternalError("Unix: sched_setaffinity()", "could not set affinity! errno: " + stringify(errno));

#ifdef DEBUG
               std::string msg = "THREAD \t\t POOL_ID \t LPU \t NODE \n";
               msg += "MAIN \t\t - \t\t" + stringify(lpu->sched_id) + "\t\t" + stringify(lpu->socket_id) + " \n";
#endif
            }

            Lock l(*_pool_sync->steal_mutex); // Prevent threads from stealing before all threads are alive

            for (unsigned i(0) ; i < _num_threads ; ++i)
            {
                unsigned sched_id(_affinity ? lpu->sched_id : 0xFFFF);

                mc::ThreadData * td = new mc::ThreadData;

                mc::WorkStealingThreadFunction<mc::CASDeque<mc::ThreadTask *> > * tobj =
                    new mc::WorkStealingThreadFunction<mc::CASDeque<mc::ThreadTask *> >(_pool_sync, td, i, sched_id,
                            _thread_fn, _num_threads, global_terminate);

                Thread * t;
                {
                    Lock l(*_pool_sync->mutex);
                    t = new Thread(*tobj); // tobj will be copied here!
                    _pool_sync->barrier->wait(*_pool_sync->mutex); // Wait until the thread is really setup / got cpu time for the first time
                }

                _threads.push_back(t);
                _thread_data.push_back(td);
                _thread_fn.push_back(tobj);

                if (_affinity)
                {
                    CPU_ZERO(&_affinity_mask[i]);
                    CPU_SET(sched_id, &_affinity_mask[i]);
                    if(sched_setaffinity(tobj->tid(), sizeof(cpu_set_t), &_affinity_mask[i]) != 0)
                        throw ExternalError("Unix: sched_setaffinity()", "could not set affinity! errno: " + stringify(errno));
#ifdef DEBUG
                    msg += stringify(tobj->tid()) + "\t\t" + stringify(i) + "\t\t" + stringify(sched_id) + "\t\t" + stringify(lpu->socket_id) + " \n";
#endif

                    if (! lpu->has_thread) // Could otherwise cause problems
                    //- assigning the same LPU more than once to the array threaded_lpus due to multiple threads on it can possibly exceed the arrays size
                    {
                        lpu->has_thread = true;
                        Socket * sock = _topology->sockets()[lpu->socket_id];
                        sock->_threaded_lpus[sock->_num_threads] = lpu;
                        ++sock->_num_threads;
                    }

                    switch (_assign_policy)
                    {
                        case 0:
                            lpu = lpu->linear_succ;
                            break;

                        case 1:
                            lpu = lpu->alternating_succ;
                    }
                }
            }

            if (_affinity)
                set_lpu_successors();

#ifdef DEBUG
            LOGMESSAGE(lc_backend, msg);
#endif
        }

        ~WorkStealingImplementation()
        {
            {
                Lock l(*_pool_sync->steal_mutex);
                global_terminate = true;
            }

            // This will delete our copies - Thread has an own one!
            for (std::vector<mc::WorkStealingThreadFunction<mc::CASDeque<mc::ThreadTask *> > *>::iterator i(_thread_fn.begin()),
                    i_end(_thread_fn.end()) ; i != i_end ; ++i)
            {
                delete (*i);
            }

            delete[] _affinity_mask;
        }

        virtual Ticket<tags::CPU::MultiCore> enqueue(const function<void ()> & task, mc::DispatchPolicy p)
        {
            CONTEXT("When creating a ThreadTask:");

            Ticket<tags::CPU::MultiCore> ticket(p.apply());
            mc::ThreadTask * t_task(new mc::ThreadTask(task, ticket));

            int idx(-1);
            int req_sched(ticket.req_sched());

            if (! _affinity)
            {
                idx = rand() % _num_threads;
            }
            else
            {
                if (req_sched != 0xFFFF)
                {
                    idx = req_sched;
                }
                else // req_sched == 0xFFFF
                {
                    int req_socket(ticket.req_socket());

                    if (req_socket == 0xFFFF)
                    {
                        idx = rand() % _num_threads;
                    }
                    else
                    {
                        if (_topology->sockets()[req_socket]->_num_threads > 0)
                            idx = _topology->sockets()[req_socket]->_threaded_lpus[rand() % _topology->sockets()[req_socket]->_num_threads]->sched_id;
                        else
                            idx = rand() % _num_threads;
                    }
                }
            }

            mc::WorkStealingThreadFunction<mc::CASDeque<mc::ThreadTask *> > * wfunc(_thread_fn[idx]);
            wfunc->enqueue(t_task);

            {
                Lock l(*_pool_sync->mutex);
                _pool_sync->barrier->broadcast();
            }

            return ticket;
        }

        virtual Ticket<tags::CPU::MultiCore> enqueue(const function<void ()> & task)
        {
            CONTEXT("When creating a ThreadTask:");

            Ticket<tags::CPU::MultiCore> ticket(policy().apply());
            mc::ThreadTask * t_task(new mc::ThreadTask(task, ticket));

            int idx(-1);
            int req_sched(ticket.req_sched());

            if (! _affinity)
            {
                idx = rand() % _num_threads;
            }
            else
            {
                if (req_sched != 0xFFFF)
                {
                    idx = req_sched;
                }
                else // req_sched == 0xFFFF
                {
                    int req_socket(ticket.req_socket());

                    if (req_socket == 0xFFFF)
                    {
                        idx = rand() % _num_threads;
                    }
                    else
                    {
                        if (_topology->sockets()[req_socket]->_num_threads > 0)
                            idx = _topology->sockets()[req_socket]->_threaded_lpus[rand() % _topology->sockets()[req_socket]->_num_threads]->sched_id;
                        else
                            idx = rand() % _num_threads;
                    }
                }
            }

            mc::WorkStealingThreadFunction<mc::CASDeque<mc::ThreadTask *> > * wfunc(_thread_fn[idx]);
            wfunc->enqueue(t_task);

            {
                Lock l(*_pool_sync->mutex);
                _pool_sync->barrier->broadcast();
            }

            return ticket;
        }
    };

    template class InstantiationPolicy<mc::ThreadPool, Singleton>;
}

using namespace honei;
using namespace honei::mc;

Ticket<tags::CPU::MultiCore> DispatchPolicy::last;

ThreadPool::ThreadPool() :
    PrivateImplementationPattern<ThreadPool, Shared>(select_impl())
{
}

ThreadPool::~ThreadPool()
{
}

Implementation<ThreadPool> * ThreadPool::select_impl()
{
    bool works = Configuration::instance()->get_value("mc::work_stealing", false);

    bool affinity = Configuration::instance()->get_value("mc::affinity", true);

    int listtype = Configuration::instance()->get_value("mc::listtype", 0);

#ifndef linux
    affinity = false;
#endif

    if (works)
    {
        if (listtype == 2)
        {
            return new WorkStealingImplementation<mc::CASDeque<mc::ThreadTask *> >;
        }
        else if (listtype == 1)
        {
            return new WorkStealingImplementation<mc::ConcurrentDeque<mc::ThreadTask *> >;
        }
        else
        {
            return new WorkStealingImplementation<std::deque<mc::ThreadTask *> >;
        }

    }
    else if (affinity)
    {
        return new AffinityImplementation;
    }
    else
    {
        if (listtype == 2)
        {
            return new StandardImplementation<mc::CASDeque<mc::ThreadTask *> >;
        }
        else if (listtype == 1)
        {
            return new StandardImplementation<mc::ConcurrentDeque<mc::ThreadTask *> >;
        }
        else
        {
            return new StandardImplementation<std::deque<mc::ThreadTask *> >;
        }
    }
}

unsigned ThreadPool::num_threads() const
{
    return _imp->_num_threads;
}

Ticket<tags::CPU::MultiCore> ThreadPool::enqueue(const function<void ()> & task, DispatchPolicy p)
{
    return _imp->enqueue(task, p);
}

/// Use default policy
Ticket<tags::CPU::MultiCore> ThreadPool::enqueue(const function<void ()> & task)
{
    return _imp->enqueue(task);
}
