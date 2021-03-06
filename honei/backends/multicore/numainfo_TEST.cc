/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2010 - 2012 Sven Mallach <mallach@honei.org>
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
#include <honei/util/unittest.hh>
#include <iostream>

using namespace honei::mc;
using namespace tests;

class NumaInfoTest :
    public BaseTest
{
    public:
        NumaInfoTest() :
            BaseTest("numainfo_test")
        {
        }

        virtual void run() const
        {
#if defined linux
            unsigned nnodes = intern::num_nodes();

            if (nnodes < 1)
            {
                TEST_CHECK(nnodes == 0);
                std::cout << "NumaInfo TEST cannot be performed - NUMA filesystem not available on this system" << std::endl;
                TEST_CHECK(true);
                return;
            }
            else
            {
                unsigned lpus(sysconf(_SC_NPROCESSORS_CONF));
                unsigned * res = intern::cpu_to_node_array(nnodes, lpus);

                TEST_CHECK(res != NULL);

                for (unsigned i(0) ; i < lpus ; ++i)
                {
                    TEST_CHECK(res[i] < lpus);
                }

                delete[] res;
            }

#else
            TEST_CHECK(true);
#endif
        }
} numainfo_test;
