/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2009 Markus Geveler <apryde@gmx.de>
 *
 * This file is part of the Math C++ library. LibMath is free software;
 * you can redistribute it and/or modify it under the terms of the GNU General
 * Public License version 2, as published by the Free Software Foundation.
 *
 * LibMath is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <honei/math/vector_io.hh>
#include <honei/util/unittest.hh>
#include <honei/util/stringify.hh>
#include <honei/la/product.hh>
#include <iostream>

using namespace honei;
using namespace tests;
using namespace std;

template<typename DT_, typename Tag_>
class VectorIOTest:
    public BaseTest
{
    public:
        VectorIOTest(const std::string & tag) :
            BaseTest("EXP vector Format I/O test " + tag)
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            std::string filename(HONEI_SOURCEDIR);
            filename += "/honei/math/testdata/area51_rhs_0";

            DenseVector<DT_> data(VectorIO<io_formats::EXP>::read_vector(filename, DT_(0)));

            std::cout << "NZ: " << data.size() << std::endl;
            std::cout << data << std::endl;

            std::string filename2(HONEI_SOURCEDIR);
            filename2 += "/honei/math/testdata/area51_rhs_0.dv";

            DenseVector<DT_> data2(VectorIO<io_formats::DV>::read_vector(filename2, DT_(1)));;
            std::cout << "NZ: " << data2.size() << std::endl;
            std::cout << data2 << std::endl;

            TEST_CHECK_EQUAL(data2, data);
        }
};
VectorIOTest<float, tags::CPU> vecio_test_float_dense("float");
VectorIOTest<double, tags::CPU> vecio_test_double_dense("double");

