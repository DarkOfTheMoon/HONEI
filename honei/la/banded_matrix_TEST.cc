/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2007, 2008 Danny van Dyk <danny.dyk@uni-dortmund.de>
 * Copyright (c) 2007 Dirk Ribbrock <dirk.ribbrock@uni-dortmund.de>
 *
 * This file is part of the LA C++ library. LibLa is free software;
 * you can redistribute it and/or modify it under the terms of the GNU General
 * Public License version 2, as published by the Free Software Foundation.
 *
 * LibLa is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <honei/la/banded_matrix.hh>
#include <honei/la/vector_error.hh>
#include <honei/util/unittest.hh>

#include <string>

using namespace honei;
using  namespace tests;

template <typename DataType_>
class BandedMatrixCreationTest :
    public BaseTest
{
    public:
        BandedMatrixCreationTest(const std::string & type) :
            BaseTest("banded_matrix_creation_test<" + type + ">")
        {
        }

        virtual void run() const
        {
            for (unsigned long size(1) ; size < (1 << 8) ; size <<= 1)
            {
                BandedMatrix<DataType_> bm1(size);
                TEST_CHECK(true);

                DenseVector<DataType_> dv1(size, DataType_(1));
                BandedMatrix<DataType_> bm2(size, dv1);
                TEST_CHECK(true);
            }

            DenseVector<DataType_> dv2(5, DataType_(1));
            TEST_CHECK_THROWS(BandedMatrix<DataType_> bm3(6, dv2), VectorSizeDoesNotMatch);
        }
};

BandedMatrixCreationTest<float> banded_matrix_creation_test_float("float");
BandedMatrixCreationTest<double> banded_matrix_creation_test_double("double");

template <typename DataType_>
class BandedMatrixQuickTest :
    public QuickTest
{
    public:
        BandedMatrixQuickTest(const std::string & type) :
            QuickTest("banded_matrix_quick_test<" + type + ">")
        {
        }

        virtual void run() const
        {
            unsigned long size(11);

            DenseVector<DataType_> dv1(size, DataType_(1));
            BandedMatrix<DataType_> bm1(size), bm2(size, dv1);

            DenseVector<DataType_> dv4(size, DataType_(0));
            dv4[size / 2] = DataType_(5);
            DenseVector<DataType_> dv3 = bm2.band(3);
            dv3[size / 2] = DataType_(5);

            TEST_CHECK_EQUAL(bm2.band(0), dv1);
            TEST_CHECK_EQUAL(bm2.band(3), dv4);
            TEST_CHECK_EQUAL(bm2.rows(), size);
            TEST_CHECK_EQUAL(bm2.columns(), size);

            DenseVector<DataType_> dv2(5, DataType_(1));
            TEST_CHECK_THROWS(BandedMatrix<DataType_> bm3(6, dv2), VectorSizeDoesNotMatch);

            //use one dense-vector-pointer in two banded-matrices
            DenseVector<DataType_> dv11(size, DataType_(1));
            BandedMatrix<DataType_> bm11(size, dv11), bm12(size, dv11);
            TEST_CHECK(true);
        }
};
BandedMatrixQuickTest<float>  banded_matrix_quick_test_float("float");
BandedMatrixQuickTest<double> banded_matrix_quick_test_double("double");
