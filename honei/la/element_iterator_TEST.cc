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

#include <honei/la/element_iterator.hh>
#include <honei/la/banded_matrix.hh>
#include <const_vector.hh>
#include <honei/la/dense_matrix.hh>
#include <honei/la/sparse_vector.hh>
#include <honei/util/unittest.hh>

#include <string>
#include <limits>

using namespace honei;
using namespace tests;

template <typename DataType_>
class BandedMatrixElementIterationTest :
    public BaseTest
{
    public:
        BandedMatrixElementIterationTest(const std::string & type) :
            BaseTest("banded_matrix_element_iteration_test<" + type + ">")
        {
        }

        virtual void run() const
        {
            for (unsigned long size(1) ; size < (1 << 8) ; size <<= 1)
            {
                BandedMatrix<DataType_> bm(size);

                typename BandedMatrix<DataType_>::ConstElementIterator ce(bm.begin_elements()), ce_end(bm.end_elements());
                for (unsigned long i(0) ; i < (size * size) ; ++i, ++ce)
                {
                    TEST_CHECK_EQUAL(ce.index(), i);
                    TEST_CHECK_EQUAL_WITHIN_EPS(*ce, 0, std::numeric_limits<DataType_>::epsilon());
                }

                BandedMatrix<DataType_> bm2(size);
                for (typename BandedMatrix<DataType_>::ConstBandIterator cb(bm2.begin_bands()), cb_end(bm2.end_bands()) ;
                    cb != cb_end ; ++cb)
                {
                    DenseVector<DataType_>  dv = *cb;
                        for (typename DenseVector<DataType_>::ConstElementIterator i(dv.begin_elements()),
                            i_end(dv.end_elements()) ; i != i_end ; ++i)
                        {
                            TEST_CHECK_EQUAL_WITHIN_EPS(*i, 0, std::numeric_limits<DataType_>::epsilon());
                        }
                }

                // Test the assignment operator and proper iterator deletion
                typename BandedMatrix<DataType_>::ElementIterator i(bm.begin_elements());
                typename BandedMatrix<DataType_>::ConstElementIterator j(bm.begin_elements());
                i = bm.end_elements();
                j = bm.end_elements();
            }
        }
};

BandedMatrixElementIterationTest<float> banded_matrix_element_iteration_test_float("float");
BandedMatrixElementIterationTest<double> banded_matrix_element_iteration_test_double("double");

template <typename DataType_>
class ConstVectorElementIterationTest :
    public BaseTest
{
    public:
        ConstVectorElementIterationTest(const std::string & type) :
            BaseTest("const_vector_element_iteration_test<" + type + ">")
        {
        }

        virtual void run() const
        {
            for (unsigned long size(1) ; size < (1 << 8) ; size <<= 1)
            {
                DenseVector<DataType_> dv(size, DataType_(10));

                typename DenseVector<DataType_>::ElementIterator e(dv.begin_elements()), e_end(dv.end_elements()) ;
                for (unsigned long i(0) ; i < size ; ++i, ++e)
                {
                    *e = 222;
                }

                ConstVector<DataType_> cv(dv);
                typename ConstVector<DataType_>::ConstElementIterator ce(cv.begin_elements()), ce_end(cv.end_elements()) ;
                for (unsigned long i(0) ; i < size ; ++i, ++ce)
                {
                    TEST_CHECK_EQUAL(ce.index(), i);
                    TEST_CHECK_EQUAL_WITHIN_EPS(*ce, 222, std::numeric_limits<DataType_>::epsilon());
                }

                // Test the assignment operator and proper iterator deletion
                typename ConstVector<DataType_>::ConstElementIterator i(cv.element_at(size / 2));
                i = cv.element_at(i.index());
            }
        }
};

ConstVectorElementIterationTest<float> const_vector_element_iteration_test_float("float");
ConstVectorElementIterationTest<double> const_vector_element_iteration_test_double("double");

template <typename DataType_>
class DenseMatrixElementIterationTest :
    public BaseTest
{
    public:
        DenseMatrixElementIterationTest(const std::string & type) :
            BaseTest("dense_matrix_element_iteration_test<" + type + ">")
        {
        }

        virtual void run() const
        {
            for (unsigned long size(1) ; size < (1 << 8) ; size <<= 1)
            {
                DenseMatrix<DataType_> dm(size, size, DataType_(10));

                typename DenseMatrix<DataType_>::ElementIterator e(dm.begin_elements()), e_end(dm.end_elements()) ;
                for (unsigned long i(0) ; i < (size * size) ; ++i, ++e)
                {
                    TEST_CHECK_EQUAL(e.index(), i);
                    TEST_CHECK_EQUAL_WITHIN_EPS(*e, 10, std::numeric_limits<DataType_>::epsilon());
                    *e = 333;
                }

                for (typename DenseMatrix<DataType_>::ElementIterator me(dm.begin_elements()),
                        me_end(dm.end_elements()) ; me != me_end ; ++me)
                {
                    TEST_CHECK_EQUAL_WITHIN_EPS(*me, 333, std::numeric_limits<DataType_>::epsilon());
                }

                typename DenseMatrix<DataType_>::ConstElementIterator ce(dm.begin_elements()), ce_end(dm.end_elements()) ;
                for (unsigned long i(0) ; i < (size * size) ; ++i, ++ce)
                {
                    TEST_CHECK_EQUAL(ce.index(), i);
                    TEST_CHECK_EQUAL_WITHIN_EPS(*ce, 333, std::numeric_limits<DataType_>::epsilon());
                }

                // Test the assignment operator and proper iterator deletion
                typename DenseMatrix<DataType_>::ElementIterator i(dm.element_at(size / 2));
                typename DenseMatrix<DataType_>::ConstElementIterator j(dm.element_at(size / 2));
                i = dm.element_at(i.index());
                j = dm.element_at(j.index());
            }
        }
};

DenseMatrixElementIterationTest<float> dense_matrix_element_iteration_test_float("float");
DenseMatrixElementIterationTest<double> dense_matrix_element_iteration_test_double("double");

template <typename DataType_>
class DenseVectorElementIterationTest :
    public BaseTest
{
    public:
        DenseVectorElementIterationTest(const std::string & type) :
            BaseTest("dense_vector_element_iteration_test<" + type + ">")
        {
        }

        virtual void run() const
        {
            for (unsigned long size(1) ; size < (1 << 8) ; size <<= 1)
            {
                DenseVector<DataType_> dv(size, DataType_(10));

                typename DenseVector<DataType_>::ElementIterator e(dv.begin_elements()), e_end(dv.end_elements()) ;
                for (unsigned long i(0) ; i < size ; ++i, ++e)
                {
                    TEST_CHECK_EQUAL(e.index(), i);
                    TEST_CHECK_EQUAL_WITHIN_EPS(*e, 10, std::numeric_limits<DataType_>::epsilon());
                    *e = 222;
                }

                typename DenseVector<DataType_>::ConstElementIterator ce(dv.begin_elements()), ce_end(dv.end_elements()) ;
                for (unsigned long i(0) ; i < size ; ++i, ++ce)
                {
                    TEST_CHECK_EQUAL(ce.index(), i);
                    TEST_CHECK_EQUAL_WITHIN_EPS(*ce, 222, std::numeric_limits<DataType_>::epsilon());
                }

                // Test the assignment operator and proper iterator deletion
                typename DenseVector<DataType_>::ElementIterator i(dv.element_at(size / 2));
                typename DenseVector<DataType_>::ConstElementIterator j(dv.element_at(size / 2));
                i = dv.element_at(i.index());
                j = dv.element_at(j.index());
            }
        }
};

DenseVectorElementIterationTest<float> dense_vector_element_iteration_test_float("float");
DenseVectorElementIterationTest<double> dense_vector_element_iteration_test_double("double");

template <typename DataType_>
class SparseVectorElementIterationTest :
    public BaseTest
{
    public:
        SparseVectorElementIterationTest(const std::string & type) :
            BaseTest("sparse_vector_element_iteration_test<" + type + ">")
        {
        }

        virtual void run() const
        {
            for (unsigned long size(1) ; size < (1 << 8) ; size <<= 1)
            {
                SparseVector<DataType_> sv(size, (size / 5) + 1);

                typename SparseVector<DataType_>::ElementIterator f(sv.begin_elements()), f_end(sv.end_elements()) ;
                for (unsigned long i(0) ; i < size ; ++i, ++f)
                {
                    if (i % 10 == 0)
                        *f = 10;
                }

                typename SparseVector<DataType_>::ElementIterator e(sv.begin_elements()), e_end(sv.end_elements()) ;
                for (unsigned long i(0) ; i < size ; ++i, ++e)
                {
                    TEST_CHECK_EQUAL(e.index(), i);
                    if (i % 10 == 0)
                    {
                        TEST_CHECK_EQUAL_WITHIN_EPS(*e, 10, std::numeric_limits<DataType_>::epsilon());
                        *e = 222;
                    }
                    else
                    {
                        typename SparseVector<DataType_>::ConstElementIterator ce(e);
                        TEST_CHECK_EQUAL_WITHIN_EPS(*ce, 0, std::numeric_limits<DataType_>::epsilon());
                    }
                }

                typename SparseVector<DataType_>::ConstElementIterator ce(sv.begin_elements()), ce_end(sv.end_elements()) ;
                for (unsigned long i(0) ; i < size ; ++i, ++ce)
                {
                    TEST_CHECK_EQUAL(ce.index(), i);
                    if (i % 10 == 0)
                    {
                        TEST_CHECK_EQUAL_WITHIN_EPS(*ce, 222, std::numeric_limits<DataType_>::epsilon());
                    }
                    else
                    {
                        TEST_CHECK_EQUAL_WITHIN_EPS(*ce, 0, std::numeric_limits<DataType_>::epsilon());
                    }
                }

                unsigned long count(0);
                for (typename SparseVector<DataType_>::NonZeroConstElementIterator nz(sv.begin_non_zero_elements()),
                        nz_end(sv.end_non_zero_elements()) ; nz != nz_end ; ++nz, ++count)
                {
                    TEST_CHECK_EQUAL_WITHIN_EPS(*nz, 222, std::numeric_limits<DataType_>::epsilon());
                }

                TEST_CHECK_EQUAL(count, sv.used_elements());

                // Test the assignment operator and proper iterator deletion
                typename SparseVector<DataType_>::ElementIterator i(sv.begin_elements());
                typename SparseVector<DataType_>::ConstElementIterator j(sv.begin_elements());
                i = sv.end_elements();
                j = sv.end_elements();
            }
        }
};

SparseVectorElementIterationTest<float> sparse_vector_element_iteration_test_float("float");
SparseVectorElementIterationTest<double> sparse_vector_element_iteration_test_double("double");
