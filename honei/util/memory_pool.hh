/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2010 Dirk Ribbrock <dirk.ribbrock@uni-dortmund.de>
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

#pragma once
#ifndef UTIL_GUARD_MEMORY_POOL_HH
#define UTIL_GUARD_MEMORY_POOL_HH 1

#include <honei/util/instantiation_policy.hh>
#include <honei/util/private_implementation_pattern.hh>
#include <honei/util/tags.hh>
#include <honei/util/mutex.hh>
#include <honei/util/lock.hh>
#include <honei/util/exception.hh>
#include <honei/util/type_traits.hh>

#include <cstring>
#include <map>
#include <string>

namespace honei
{
    template<typename Tag_>
        class MemoryPool
        {
        };

    template<>
        class MemoryPool<tags::CPU> :
        public InstantiationPolicy<MemoryPool<tags::CPU>, Singleton>
        {
        private:
            /// Our mutex.
            Mutex * const _mutex;

            std::multimap<unsigned long, void *> _free_chunks;
            std::multimap<void*, unsigned long> _used_chunks;

            MemoryPool() :
                _mutex(new Mutex)
            {
            }

            ~MemoryPool()
            {
                {
                    Lock l(*_mutex);
                    _release_free();
                }
                if (_used_chunks.size() != 0)
                    //throw InternalError("MemoryPool deconstructor called with elements still unfreeed");
                delete _mutex;
            }

            void _release_free()
            {
                std::multimap<unsigned long, void*>::iterator free_it;
                for (free_it = _free_chunks.begin() ; free_it != _free_chunks.end() ; ++free_it)
                {
                    ::free(free_it->second);
                }
                _free_chunks.clear();
            }

        public:
            friend class InstantiationPolicy<MemoryPool, Singleton>;

            void * alloc(unsigned long bytes)
            {
                unsigned long real_size(intern::multiple_of_sixteen(bytes));
                CONTEXT("When allocating data (CPU):");
                Lock l(*_mutex);

                std::multimap<unsigned long, void*>::iterator free_it;
                std::multimap<void*, unsigned long>::iterator used_it;

                free_it = _free_chunks.find(real_size);
                if (free_it != _free_chunks.end())
                {
                    used_it = _used_chunks.insert (std::pair<void*, unsigned long>(free_it->second, free_it->first));
                    _free_chunks.erase(free_it);
                    return used_it->first;
                }
                else
                {
                    void * result(0);
                    int status(0);

                    status = posix_memalign(&result, 16, real_size);
                    if (status == 0)
                    {
                        _used_chunks.insert (std::pair<void*, unsigned long>(result, real_size));
                        return result;
                    }
                    else
                    {
                        _release_free();
                        status = posix_memalign(&result, 16, real_size);
                    }
                    if (status == 0)
                    {
                        _used_chunks.insert (std::pair<void*, unsigned long>(result, real_size));
                        return result;
                    }
                    else
                        throw InternalError("MemoryPool: bad alloc or out of memory!");
                }

            }

            void free(void * memid)
            {
                CONTEXT("When freeing data (CPU):");
                Lock l(*_mutex);
                std::multimap<unsigned long, void*>::iterator free_it;
                std::multimap<void*, unsigned long>::iterator used_it;

                used_it = _used_chunks.find(memid);
                if (used_it != _used_chunks.end())
                {
                    free_it = _free_chunks.insert(std::pair<unsigned long, void*>(used_it->second, used_it->first));
                    _used_chunks.erase(used_it);
                }
                else
                {
                    //throw InternalError("MemoryPool: memory chunk not found!");
                }

            }

            void release_free()
            {
                CONTEXT("When releasing all memory chunks (CPU):");
                Lock l(*_mutex);
                _release_free();
            }
        };

    /*template<>
      class MemoryBackend<tags::GPU::CUDA> :
      public MemoryBackendBase,
      public InstantiationPolicy<MemoryBackend<tags::GPU::CUDA>, Singleton>,
      public PrivateImplementationPattern<MemoryBackend<tags::GPU::CUDA>, Single>
      {
      public:
    /// Constructor.
    MemoryBackend<tags::GPU::CUDA>();

    /// Destructor.
    virtual ~MemoryBackend<tags::GPU::CUDA>();

    virtual void * upload(void * memid, void * address, unsigned long bytes);

    virtual void download(void * memid, void * address, unsigned long bytes);

    virtual void * alloc(void * memid, void * address, unsigned long bytes);

    virtual void free(void * memid);

    virtual void copy(void * src_id, void * src_address, void * dest_id,
    void * dest_address, unsigned long bytes);

    virtual void convert_float_double(void * src_id, void * src_address, void * dest_id,
    void * dest_address, unsigned long bytes);

    virtual void convert_double_float(void * src_id, void * src_address, void * dest_id,
    void * dest_address, unsigned long bytes);

    virtual void fill(void * memid, void * address, unsigned long bytes, float proto);

    virtual bool knows(void * memid, void * address);
    };

    template<>
    class MemoryBackend<tags::OpenCL::CPU> :
    public MemoryBackendBase,
    public InstantiationPolicy<MemoryBackend<tags::OpenCL::CPU>, Singleton>,
    public PrivateImplementationPattern<MemoryBackend<tags::OpenCL::CPU>, Single>
    {
    public:
    /// Constructor.
    MemoryBackend<tags::OpenCL::CPU>();

    /// Destructor.
    virtual ~MemoryBackend<tags::OpenCL::CPU>();

    virtual void * upload(void * memid, void * address, unsigned long bytes);

    virtual void download(void * memid, void * address, unsigned long bytes);

    virtual void * alloc(void * memid, void * address, unsigned long bytes);

    virtual void free(void * memid);

    virtual void copy(void * src_id, void * src_address, void * dest_id,
    void * dest_address, unsigned long bytes);

    virtual void convert_float_double(void * src_id, void * src_address, void * dest_id,
    void * dest_address, unsigned long bytes);

    virtual void convert_double_float(void * src_id, void * src_address, void * dest_id,
    void * dest_address, unsigned long bytes);

    virtual void fill(void * memid, void * address, unsigned long bytes, float proto);

    virtual bool knows(void * memid, void * address);
    };

    template<>
    class MemoryBackend<tags::OpenCL::GPU> :
        public MemoryBackendBase,
               public InstantiationPolicy<MemoryBackend<tags::OpenCL::GPU>, Singleton>,
               public PrivateImplementationPattern<MemoryBackend<tags::OpenCL::GPU>, Single>
               {
                   public:
                       /// Constructor.
                       MemoryBackend<tags::OpenCL::GPU>();

                       /// Destructor.
                       virtual ~MemoryBackend<tags::OpenCL::GPU>();

                       virtual void * upload(void * memid, void * address, unsigned long bytes);

                       virtual void download(void * memid, void * address, unsigned long bytes);

                       virtual void * alloc(void * memid, void * address, unsigned long bytes);

                       virtual void free(void * memid);

                       virtual void copy(void * src_id, void * src_address, void * dest_id,
                               void * dest_address, unsigned long bytes);

                       virtual void convert_float_double(void * src_id, void * src_address, void * dest_id,
                               void * dest_address, unsigned long bytes);

                       virtual void convert_double_float(void * src_id, void * src_address, void * dest_id,
                               void * dest_address, unsigned long bytes);

                       virtual void fill(void * memid, void * address, unsigned long bytes, float proto);

                       virtual bool knows(void * memid, void * address);
               };

    template<>
        class MemoryBackend<tags::OpenCL::Accelerator> :
        public MemoryBackendBase,
               public InstantiationPolicy<MemoryBackend<tags::OpenCL::Accelerator>, Singleton>,
               public PrivateImplementationPattern<MemoryBackend<tags::OpenCL::Accelerator>, Single>
               {
                   public:
                       /// Constructor.
                       MemoryBackend<tags::OpenCL::Accelerator>();

                       /// Destructor.
                       virtual ~MemoryBackend<tags::OpenCL::Accelerator>();

                       virtual void * upload(void * memid, void * address, unsigned long bytes);

                       virtual void download(void * memid, void * address, unsigned long bytes);

                       virtual void * alloc(void * memid, void * address, unsigned long bytes);

                       virtual void free(void * memid);

                       virtual void copy(void * src_id, void * src_address, void * dest_id,
                               void * dest_address, unsigned long bytes);

                       virtual void convert_float_double(void * src_id, void * src_address, void * dest_id,
                               void * dest_address, unsigned long bytes);

                       virtual void convert_double_float(void * src_id, void * src_address, void * dest_id,
                               void * dest_address, unsigned long bytes);

                       virtual void fill(void * memid, void * address, unsigned long bytes, float proto);

                       virtual bool knows(void * memid, void * address);
               };*/
}
#endif