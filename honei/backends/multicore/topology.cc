/* vim: set sw=4 sts=4 et foldmethod=syntax : */
/*
 * Copyright (c) 2010, 2011 Sven Mallach <mallach@honei.org>
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

#include <honei/backends/multicore/numainfo.hh>
#include <honei/backends/multicore/topology.hh>
#if defined(__i386__) || defined(__x86_64__)
#include <honei/backends/multicore/x86_spec.hh>
#endif

#include <honei/util/exception.hh>
#include <honei/util/instantiation_policy-impl.hh>
#include <honei/util/log.hh>
#include <honei/util/stringify.hh>

#include <limits>
#include <sys/syscall.h>

using namespace honei;
using namespace honei::mc;

template class InstantiationPolicy<Topology, Singleton>;

Topology::Topology() :
    _num_lpus(1),
    _num_nodes(1),
    _lpus_per_node(1)
{
#if defined linux
    _num_lpus = sysconf(_SC_NPROCESSORS_CONF);

#ifdef DEBUG
    {
        CONTEXT("When investigating the system topology:");
        std::string msg = "Found " + stringify(_num_lpus) + " logical processing units \n";
        LOGMESSAGE(lc_backend, msg);
    }
#endif

    _num_nodes = intern::num_nodes();
#endif

    if (_num_nodes == 1)
    {
        cpu_to_node = new unsigned[_num_lpus];
        for (unsigned i(0) ; i < _num_lpus ; ++i)
            cpu_to_node[i] = 0;

        range_min = new unsigned[1];
        range_max = new unsigned[1];

        range_min[0] = 0;
        range_max[0] = _num_lpus - 1;
    }
    else
    {
        cpu_to_node = intern::cpu_to_node_array(_num_nodes, _num_lpus);

        range_min = new unsigned[_num_nodes];
        range_max = new unsigned[_num_nodes];

        for (unsigned i(0) ; i < _num_nodes ; ++i)
        {
            range_min[i] = std::numeric_limits<unsigned>::max();
            range_max[i] = std::numeric_limits<unsigned>::min();
        }

        for (unsigned i(0) ; i < _num_lpus ; ++i)
        {
            if (i < range_min[cpu_to_node[i]])
                range_min[cpu_to_node[i]] = i;

            if (i > range_max[cpu_to_node[i]])
                range_max[cpu_to_node[i]] = i;
        }
    }

#ifdef DEBUG
    for (unsigned i(0) ; i < _num_nodes ; ++i)
    {
        CONTEXT("When investigating the system topology:");
        std::string msg = "Node " + stringify(i) + " has logical processing units " + stringify(range_min[i]) + " to " + stringify(range_max[i]) + "\n";
        LOGMESSAGE(lc_backend, msg);
    }
#endif

#if defined(__i386__) || defined(__x86_64__)
    init_x86(_vendor, _num_cores, _ht_factor);

    // Problem here: num_cores can be > _num_lpus
    // if HT is available but disabled.
    // Temporary workaround:
    if (_num_cores > _num_lpus)
        _num_cores = _num_lpus;

    if (_vendor == UNDEFINED)
    {
#if defined linux
        _num_cores = sysconf(_SC_NPROCESSORS_CONF);
#else
        _num_cores = 1;
#endif
    }

    _num_cpus = _num_lpus / _num_cores;
#endif

    _lpus_per_node = _num_lpus / _num_nodes;
}

Topology::~Topology()
{
    delete[] cpu_to_node;
    delete[] range_min;
    delete[] range_max;
}

unsigned Topology::num_lpus() const
{
    return _num_lpus;
}

unsigned Topology::num_nodes() const
{
    return _num_nodes;
}

unsigned Topology::lpus_per_node() const
{
    return _lpus_per_node;
}

unsigned Topology::node_min(unsigned node) const
{
    return range_min[node];
}

unsigned Topology::node_max(unsigned node) const
{
    return range_max[node];
}

unsigned Topology::get_node(unsigned lpu) const
{
    return cpu_to_node[lpu];
}

unsigned Topology::main_node() const
{
    return cpu_to_node[_num_lpus - 1];
}

#if defined(__i386__) || defined(__x86_64__)

unsigned Topology::num_cores() const
{
    return _num_cores;
}

unsigned Topology::num_cpus() const
{
    return _num_cpus;
}

unsigned Topology::ht_factor() const
{
    return _ht_factor;
}

#endif
