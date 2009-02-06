/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2007 Andre Matuschek <andre@matuschek.org>
 * Copyright (c) 2007 David Gies      <david-gies@gmx.de>
 * Copyright (c) 2009 Dirk Ribbrock   <dirk.ribbrock@uni-dortmund.de>
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

#ifndef LIBUTIL_GUARD_BENCHMARK_INFO_HH
#define LIBUTIL_GUARD_BENCHMARK_INFO_HH 1

#include <list>
#include <iostream>

struct BenchmarkInfo
{
    unsigned long flops;
    unsigned long load;
    unsigned long store;
    std::list<unsigned long> size;
    double scale;
    std::string scaleinfo;

    BenchmarkInfo() :
        flops(0),
        load(0),
        store(0),
        scale(1),
        scaleinfo(" ")
    {
    }

    BenchmarkInfo operator+(const BenchmarkInfo a) 
    {
        flops += a.flops;
        load += a.load;
        store += a.store;
        return *this;
    }

    BenchmarkInfo operator*(unsigned long a) 
    {
        flops *= a;
        load *= a;
        store *= a;
        return *this;
    }
};

struct LBMBenchmarkInfo :
    public BenchmarkInfo
{
    unsigned long lups;
    unsigned long flups;
    LBMBenchmarkInfo() :
        BenchmarkInfo(),
        lups(0),
        flups(0)
    {
    }
};

#endif
