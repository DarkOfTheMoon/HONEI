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

#include <honei/la/dense_matrix.hh>
#include <honei/la/difference.hh>
#include <honei/la/matrix_error.hh>
#include <honei/util/unittest.hh>

#include <limits>

using namespace honei;
using namespace tests;

// Test cases for matrix operations

template <typename Tag_, typename DT_>
class BandedMatrixDenseMatrixDifferenceTest :
    public BaseTest
{
    public:
        BandedMatrixDenseMatrixDifferenceTest(const std::string & type) :
            BaseTest("banded_matrix_dense_matrix_difference_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            for (unsigned long size(10) ; size < (1 << 9) ; size <<= 1)
            {
                DenseVector<DT_> dv1(size, DT_(2));
                BandedMatrix<DT_> bm1(size, dv1);
                bm1.insert_band(2,dv1);
                bm1.insert_band(-3,dv1);
                bm1.insert_band(4,dv1);
                bm1.insert_band(-5,dv1);
                DenseMatrix<DT_> dm2(size, size, DT_(1)), dm3(size, size, DT_(-1));

                typename DenseMatrix<DT_>::ElementIterator k(dm3.begin_elements());
                for (typename BandedMatrix<DT_>::ConstElementIterator i(bm1.begin_elements()),
                    i_end(bm1.end_elements()) ; i != i_end ; ++i, ++k)
                {
                    if (*i != DT_(0))
                    {
                        *k = DT_(1);
                    }
                }
                Difference<Tag_>::value(bm1, dm2);

                TEST_CHECK_EQUAL(dm2, dm3);
            }

            BandedMatrix<DT_> bm01(5);
            DenseMatrix<DT_> dm02(4, 4), dm03(5, 6);

            TEST_CHECK_THROWS(Difference<>::value(bm01, dm03), MatrixIsNotSquare);
            TEST_CHECK_THROWS(Difference<>::value(bm01, dm02), MatrixRowsDoNotMatch);
        }
};
BandedMatrixDenseMatrixDifferenceTest<tags::CPU, float> banded_matrix_dense_matrix_difference_test_float("float");
BandedMatrixDenseMatrixDifferenceTest<tags::CPU, double> banded_matrix_dense_matrix_difference_test_double("double");
BandedMatrixDenseMatrixDifferenceTest<tags::CPU::MultiCore, float> mc_banded_matrix_dense_matrix_difference_test_float("MC float");
BandedMatrixDenseMatrixDifferenceTest<tags::CPU::MultiCore, double> mc_banded_matrix_dense_matrix_difference_test_double("MC double");

template <typename Tag_, typename DT_>
class BandedMatrixDenseMatrixDifferenceQuickTest :
    public QuickTest
{
    public:
        BandedMatrixDenseMatrixDifferenceQuickTest(const std::string & type) :
            QuickTest("banded_matrix_dense_matrix_difference_quick_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            unsigned long size (11);
            DenseVector<DT_> dv1(size, DT_(2));
            BandedMatrix<DT_> bm1(size, dv1);
            bm1.insert_band(2,dv1);
            bm1.insert_band(-3,dv1);
            bm1.insert_band(4,dv1);
            bm1.insert_band(-5,dv1);
            DenseMatrix<DT_> dm2(size, size, DT_(1)), dm3(size, size, DT_(-1));

            typename DenseMatrix<DT_>::ElementIterator k(dm3.begin_elements());
            for (typename BandedMatrix<DT_>::ConstElementIterator i(bm1.begin_elements()),
                i_end(bm1.end_elements()) ; i != i_end ; ++i, ++k)
            {
                if (*i != DT_(0))
                {
                    *k = DT_(1);
                }
            }
            Difference<Tag_>::value(bm1, dm2);

            TEST_CHECK_EQUAL(dm2, dm3);

            BandedMatrix<DT_> bm01(5);
            DenseMatrix<DT_> dm02(6, 6), dm03(5, 6);

            TEST_CHECK_THROWS(Difference<>::value(bm01, dm03), MatrixIsNotSquare);
            TEST_CHECK_THROWS(Difference<>::value(bm01, dm02), MatrixRowsDoNotMatch);
        }
};
BandedMatrixDenseMatrixDifferenceQuickTest<tags::CPU, float> banded_matrix_dense_matrix_difference_quick_test_float("float");
BandedMatrixDenseMatrixDifferenceQuickTest<tags::CPU, double> banded_matrix_dense_matrix_difference_quick_test_double("double");
BandedMatrixDenseMatrixDifferenceQuickTest<tags::CPU::MultiCore, float>  mc_banded_matrix_dense_matrix_difference_quick_test_float("MC float");
BandedMatrixDenseMatrixDifferenceQuickTest<tags::CPU::MultiCore, double> mc_banded_matrix_dense_matrix_difference_quick_test_double("MC double");


template <typename Tag_, typename DT_>
class BandedMatrixDifferenceTest :
    public BaseTest
{
    public:
        BandedMatrixDifferenceTest(const std::string & type) :
            BaseTest("banded_matrix_difference_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            for (unsigned long size(10) ; size < (1 << 9) ; size <<= 1)
            {
                DenseVector<DT_> dv1(size, DT_(2));
                DenseVector<DT_> dv2(size, DT_(3));
                DenseVector<DT_> dv3(size, DT_(-1));
                DenseVector<DT_> dv4(size, DT_(1));
                BandedMatrix<DT_> bm1(size, dv1), bm2(size, dv2), bm3(size, dv3);
                bm1.insert_band(-1, dv1.copy());
                bm2.insert_band(1, dv3.copy());
                bm3.insert_band(-1, dv1.copy());
                bm3.insert_band(1, dv4.copy()); 
                Difference<Tag_>::value(bm1, bm2);

                TEST_CHECK_EQUAL(bm1, bm3);
            }

            BandedMatrix<DT_> bm01(5), bm02(6);

            TEST_CHECK_THROWS(Difference<Tag_>::value(bm02, bm01), MatrixSizeDoesNotMatch);
        }
};
BandedMatrixDifferenceTest<tags::CPU, float> banded_matrix_difference_test_float("float");
BandedMatrixDifferenceTest<tags::CPU, double> banded_matrix_difference_test_double("double");
BandedMatrixDifferenceTest<tags::CPU::MultiCore, float>  mc_banded_matrix_difference_test_float("MC float");
BandedMatrixDifferenceTest<tags::CPU::MultiCore, double> mc_banded_matrix_difference_test_double("MC double");



template <typename Tag_, typename DT_>
class BandedMatrixDifferenceQuickTest :
    public QuickTest
{
    public:
        BandedMatrixDifferenceQuickTest(const std::string & type) :
            QuickTest("banded_matrix_difference_quick_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            unsigned long size (5);
            DenseVector<DT_> dv1(size, DT_(2)), dv2(size, DT_(3)), dv3(size, DT_(-1)), dv4(size, DT_(1));
            BandedMatrix<DT_> bm1(size, dv1), bm2(size, dv2), bm3(size, dv3);
            bm1.insert_band(-1, dv1.copy());
            bm2.insert_band(1, dv3.copy());
            bm3.insert_band(-1, dv1.copy());
            bm3.insert_band(1, dv4.copy());

            Difference<Tag_>::value(bm1, bm2);

            for (typename BandedMatrix<DT_>::ConstElementIterator i(bm1.begin_elements()),
                    i_end(bm1.end_elements()), j(bm3.begin_elements()) ; i != i_end ; ++i, ++j)
            {
                TEST_CHECK_EQUAL_WITHIN_EPS(*i, *j, std::numeric_limits<DT_>::epsilon());
            }

            BandedMatrix<DT_> bm01(5), bm02(6);

            TEST_CHECK_THROWS(Difference<Tag_>::value(bm02, bm01), MatrixSizeDoesNotMatch);
        }
};
BandedMatrixDifferenceQuickTest<tags::CPU, float> banded_matrix_difference_quick_test_float("float");
BandedMatrixDifferenceQuickTest<tags::CPU, double> banded_matrix_difference_quick_test_double("double");
BandedMatrixDifferenceQuickTest<tags::CPU::MultiCore, float>  mc_banded_matrix_difference_quick_test_float("MC float");
BandedMatrixDifferenceQuickTest<tags::CPU::MultiCore, double> mc_banded_matrix_difference_quick_test_double("MC double");

template <typename Tag_, typename DT_>
class BandedMatrixSparseMatrixDifferenceTest :
    public BaseTest
{
    public:
        BandedMatrixSparseMatrixDifferenceTest(const std::string & type) :
            BaseTest("banded_matrix_sparse_matrix_difference_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            for (unsigned long size(10) ; size < (1 << 9) ; size <<= 1)
            {
                DenseVector<DT_> dv1(size, DT_(3));
                BandedMatrix<DT_> bm1(size, dv1);
                for (long band((-long(size-1)) + (rand() % 6)) ; (band < long(size)) ; band += (rand() % 6) + 1) 
                {
                    bm1.insert_band(band, dv1);
                }
                SparseMatrix<DT_> sm2(size, size, size / 8 + 1),
                        sm3(size, size, size / 8 + 1);

                for (typename SparseMatrix<DT_>::ElementIterator i(sm2.begin_elements()),
                        i_end(sm2.end_elements()), k(sm3.begin_elements()) ;
                        i != i_end ; ++i, ++k)
                {
                    if (i.index() % 10 == 0)
                    {
                        *i = DT_(2);
                        *k = DT_(-2);
                    }
                }

                typename SparseMatrix<DT_>::ElementIterator k(sm3.begin_elements());
                for (typename BandedMatrix<DT_>::ConstElementIterator i(bm1.begin_elements()),
                        i_end(bm1.end_elements()) ; i != i_end ; ++i, ++k)
                {
                    if (*i != DT_(0))
                    {
                        *k = DT_(3);
                        if (i.index() % 10 == 0)
                            *k = DT_(1);
                    }
                }
                Difference<Tag_>::value(bm1, sm2);

                TEST_CHECK_EQUAL(sm2, sm3);
            }

            BandedMatrix<DT_> bm01(5);
            SparseMatrix<DT_> sm02(6, 5, 1), sm03(6, 6, 1);

            TEST_CHECK_THROWS(Difference<>::value(bm01, sm03), MatrixRowsDoNotMatch);
            TEST_CHECK_THROWS(Difference<>::value(bm01, sm02), MatrixIsNotSquare);
        }
};
BandedMatrixSparseMatrixDifferenceTest<tags::CPU, float> banded_matrix_sparse_matrix_difference_test_float("float");
BandedMatrixSparseMatrixDifferenceTest<tags::CPU, double> banded_matrix_sparse_matrix_difference_test_double("double");
BandedMatrixSparseMatrixDifferenceTest<tags::CPU::MultiCore, float> mc_banded_matrix_sparse_matrix_difference_test_float("MC float");
BandedMatrixSparseMatrixDifferenceTest<tags::CPU::MultiCore, double> mc_banded_matrix_sparse_matrix_difference_test_double("MC double");

template <typename Tag_, typename DT_>
class BandedMatrixSparseMatrixDifferenceQuickTest :
    public QuickTest
{
    public:
        BandedMatrixSparseMatrixDifferenceQuickTest(const std::string & type) :
            QuickTest("banded_matrix_sparse_matrix_difference_quick_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            unsigned long size(11);
            DenseVector<DT_> dv1(size, DT_(2));
            BandedMatrix<DT_> bm1(size, dv1);
            SparseMatrix<DT_> sm2(size, size, size / 8 + 1),
                    sm3(size, size, size / 8 + 1);

            for (typename SparseMatrix<DT_>::ElementIterator i(sm2.begin_elements()),
                i_end(sm2.end_elements()), k(sm3.begin_elements()) ;
                i != i_end ; ++i, ++k)
            {
                if (i.index() % 10 == 0)
                {
                    *i = DT_(1);
                    *k = DT_(-1);
                }
            }

           typename SparseMatrix<DT_>::ElementIterator k(sm3.begin_elements());
            for (typename BandedMatrix<DT_>::ConstElementIterator i(bm1.begin_elements()),
                i_end(bm1.end_elements()) ; i != i_end ; ++i, ++k)
            {
                if (*i != DT_(0))
                {
                    *k = DT_(2);
                    if (i.index() % 10 == 0)
                        *k = DT_(1);
                }
            }

            Difference<Tag_>::value(bm1, sm2);
            TEST_CHECK_EQUAL(sm2, sm3);

            BandedMatrix<DT_> bm01(5);
            SparseMatrix<DT_> sm02(6, 5, 1), sm03(6, 6, 1);

            TEST_CHECK_THROWS(Difference<>::value(bm01, sm03), MatrixRowsDoNotMatch);
            TEST_CHECK_THROWS(Difference<>::value(bm01, sm02), MatrixIsNotSquare);
        }
};
BandedMatrixSparseMatrixDifferenceQuickTest<tags::CPU, float> banded_matrix_sparse_matrix_difference_quick_test_float("float");
BandedMatrixSparseMatrixDifferenceQuickTest<tags::CPU, double> banded_matrix_sparse_matrix_difference_quick_test_double("double");
BandedMatrixSparseMatrixDifferenceQuickTest<tags::CPU::MultiCore, float> mc_banded_matrix_sparse_matrix_difference_quick_test_float("MC float");
BandedMatrixSparseMatrixDifferenceQuickTest<tags::CPU::MultiCore, double> mc_banded_matrix_sparse_matrix_difference_quick_test_double("MC double");

template <typename Tag_, typename DT_>
class DenseMatrixDifferenceTest :
    public BaseTest
{
    public:
        DenseMatrixDifferenceTest(const std::string & type) :
            BaseTest("dense_matrix_difference_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            for (unsigned long size(10) ; size < (1 << 9) ; size <<= 1)
            {
                DenseMatrix<DT_> dm1(size+1, size, DT_(2)), dm2(size+1, size, DT_(3)),
                    dm3(size+1, size, DT_(-1));
                Difference<Tag_>::value(dm1, dm2);

                TEST(dm1.lock(lm_read_only), TEST_CHECK_EQUAL(dm1, dm3), dm1.unlock(lm_read_only));
            }

            DenseMatrix<DT_> dm01(5, 5), dm02(6, 6), dm03(5, 6);

            TEST_CHECK_THROWS(Difference<>::value(dm03, dm02), MatrixRowsDoNotMatch);
            TEST_CHECK_THROWS(Difference<>::value(dm03, dm01), MatrixColumnsDoNotMatch);
        }
};
DenseMatrixDifferenceTest<tags::CPU, float> dense_matrix_difference_test_float("float");
DenseMatrixDifferenceTest<tags::CPU, double> dense_matrix_difference_test_double("double");
DenseMatrixDifferenceTest<tags::CPU::MultiCore, float> mc_dense_matrix_difference_test_float("MC float");
DenseMatrixDifferenceTest<tags::CPU::MultiCore, double> mc_dense_matrix_difference_test_double("MC double");
#ifdef HONEI_SSE
DenseMatrixDifferenceTest<tags::CPU::SSE, float> sse_dense_matrix_difference_test_float("float");
DenseMatrixDifferenceTest<tags::CPU::SSE, double> sse_dense_matrix_difference_test_double("double");
#endif
#ifdef HONEI_CUDA
DenseMatrixDifferenceTest<tags::GPU::CUDA, float> cuda_dense_matrix_difference_test_float("float");
#endif
#ifdef HONEI_CELL
DenseMatrixDifferenceTest<tags::Cell, float> cell_dense_matrix_difference_test_float("Cell float");
#endif

template <typename Tag_, typename DT_>
class DenseMatrixDifferenceQuickTest :
    public QuickTest
{
    public:
        DenseMatrixDifferenceQuickTest(const std::string & type) :
            QuickTest("dense_matrix_difference_quick_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            unsigned long size(5);
            DenseMatrix<DT_> dm1(size+1, size, DT_(2)), dm2(size+1, size, DT_(3)),
                dm3(size+1, size, DT_(-1));
            Difference<Tag_>::value(dm1, dm2);

            TEST(dm1.lock(lm_read_only), TEST_CHECK_EQUAL(dm1, dm3), dm1.unlock(lm_read_only));

            DenseMatrix<DT_> dm01(5, 5), dm02(6, 6), dm03(6, 5);

            TEST_CHECK_THROWS(Difference<>::value(dm03, dm01), MatrixRowsDoNotMatch);
            TEST_CHECK_THROWS(Difference<>::value(dm03, dm02), MatrixColumnsDoNotMatch);
        }
};
DenseMatrixDifferenceQuickTest<tags::CPU, float> dense_matrix_difference_quick_test_float("float");
DenseMatrixDifferenceQuickTest<tags::CPU, double> dense_matrix_difference_quick_test_double("double");
DenseMatrixDifferenceQuickTest<tags::CPU::MultiCore, float> mc_dense_matrix_difference_quick_test_float("MC float");
DenseMatrixDifferenceQuickTest<tags::CPU::MultiCore, double> mc_dense_matrix_difference_quick_test_double("MC double");
#ifdef HONEI_SSE
DenseMatrixDifferenceQuickTest<tags::CPU::SSE, float> sse_dense_matrix_difference_quick_test_float("float");
DenseMatrixDifferenceQuickTest<tags::CPU::SSE, double> sse_dense_matrix_difference_quick_test_double("double");
#endif
#ifdef HONEI_CUDA
DenseMatrixDifferenceQuickTest<tags::GPU::CUDA, float> cuda_dense_matrix_difference_quick_test_float("float");
#endif
#ifdef HONEI_CELL
DenseMatrixDifferenceQuickTest<tags::Cell, float> cell_dense_matrix_difference_quick_test_float("Cell float");
#endif

template <typename Tag_, typename DT_>
class DenseMatrixSparseMatrixDifferenceTest :
    public BaseTest
{
    public:
        DenseMatrixSparseMatrixDifferenceTest(const std::string & type) :
            BaseTest("dense_matrix_sparse_matrix_difference_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            for (unsigned long size(10) ; size < (1 << 9) ; size <<= 1)
            {
                DenseMatrix<DT_> dm1(size+1, size, DT_(0)),
                        dm3(size+1, size, DT_(0));
                SparseMatrix<DT_> sm2(size+1, size, size / 7 + 1);
                typename SparseMatrix<DT_>::ElementIterator j(sm2.begin_elements());
                for (typename DenseMatrix<DT_>::ElementIterator i(dm1.begin_elements()),
                    i_end(dm1.end_elements()), k(dm3.begin_elements()) ;
                    i != i_end ; ++i, ++j, ++k)
                {
                    if (i.index() % 10 == 0)
                    {
                        *i = DT_((i.index() +1) * 2 / 1.23456789);
                        *k = DT_((i.index() +1) * 2 / 1.23456789);
                    }
                    if (i.index() % 7 == 0)
                    {
                        *j = DT_((i.index() +1) / 1.23456789);
                        *k = DT_((i.index() +1) / -1.23456789);
                    }
                    if (i.index() % 10 == 0 && i.index() % 7 == 0)
                    {
                        *k = DT_((i.index() +1) / 1.23456789);
                    }
                }
                Difference<Tag_>::value(dm1, sm2);

                TEST_CHECK_EQUAL(dm1, dm3);
            }

            SparseMatrix<DT_> sm01(5, 5, 1), sm02(6, 6, 1);
            DenseMatrix<DT_> dm03(6, 5);

            TEST_CHECK_THROWS(Difference<>::value(dm03, sm01), MatrixRowsDoNotMatch);
            TEST_CHECK_THROWS(Difference<>::value(dm03, sm02), MatrixColumnsDoNotMatch);
        }
};
DenseMatrixSparseMatrixDifferenceTest<tags::CPU, float> dense_matrix_sparse_matrix_difference_test_float("float");
DenseMatrixSparseMatrixDifferenceTest<tags::CPU, double> dense_matrix_sparse_matrix_difference_test_double("double");
DenseMatrixSparseMatrixDifferenceTest<tags::CPU::MultiCore, float>  mc_dense_matrix_sparse_matrix_difference_test_float("MC float");
DenseMatrixSparseMatrixDifferenceTest<tags::CPU::MultiCore, double> mc_dense_matrix_sparse_matrix_difference_test_double("MC double");

template <typename Tag_, typename DT_>
class DenseMatrixSparseMatrixDifferenceQuickTest :
    public QuickTest
{
    public:
        DenseMatrixSparseMatrixDifferenceQuickTest(const std::string & type) :
            QuickTest("dense_matrix_sparse_matrix_difference_quick_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            unsigned long size (10);
            DenseMatrix<DT_> dm1(size+1, size, DT_(0)),
                    dm3(size+1, size, DT_(0));
            SparseMatrix<DT_> sm2(size+1, size, size / 7 + 1);
            typename SparseMatrix<DT_>::ElementIterator j(sm2.begin_elements());
            for (typename DenseMatrix<DT_>::ElementIterator i(dm1.begin_elements()),
                i_end(dm1.end_elements()), k(dm3.begin_elements()) ;
                i != i_end ; ++i, ++j, ++k)
            {
                if (i.index() % 10 == 0)
                {
                    *i = DT_((i.index() +1) * 2 / 1.23456789);
                    *k = DT_((i.index() +1) * 2 / 1.23456789);
                }
                if (i.index() % 7 == 0)
                {
                    *j = DT_((i.index() +1) / 1.23456789);
                    *k = DT_((i.index() +1) / -1.23456789);
                }
                if (i.index() % 10 == 0 && i.index() % 7 == 0)
                {
                    *k = DT_((i.index() +1) / 1.23456789);
                }
            }
            Difference<Tag_>::value(dm1, sm2);

            TEST_CHECK_EQUAL(dm1, dm3);

            SparseMatrix<DT_> sm01(5, 5, 1), sm02(6, 6, 1);
            DenseMatrix<DT_> dm03(6, 5);

            TEST_CHECK_THROWS(Difference<>::value(dm03, sm01), MatrixRowsDoNotMatch);
            TEST_CHECK_THROWS(Difference<>::value(dm03, sm02), MatrixColumnsDoNotMatch);
        }
};
DenseMatrixSparseMatrixDifferenceQuickTest<tags::CPU, float> dense_matrix_sparse_matrix_difference_quick_test_float("float");
DenseMatrixSparseMatrixDifferenceQuickTest<tags::CPU, double> dense_matrix_sparse_matrix_difference_quick_test_double("double");
DenseMatrixSparseMatrixDifferenceQuickTest<tags::CPU::MultiCore, float>  mc_dense_matrix_sparse_matrix_difference_quick_test_float("MC float");
DenseMatrixSparseMatrixDifferenceQuickTest<tags::CPU::MultiCore, double> mc_dense_matrix_sparse_matrix_difference_quick_test_double("MC double");

template <typename Tag_, typename DT_>
class SparseMatrixDifferenceTest :
    public BaseTest
{
    public:
        SparseMatrixDifferenceTest(const std::string & type) :
            BaseTest("sparse_matrix_difference_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            for (unsigned long size(10) ; size < (1 << 9) ; size <<= 1)
            {
                SparseMatrix<DT_> sm1(size+1, size, size / 8 + 1),
                    sm2(size+1, size, size / 7 + 1), sm3(size+1, size, size / 8 + 1 );
                for (typename SparseMatrix<DT_>::ElementIterator i(sm1.begin_elements()),
                    i_end(sm1.end_elements()), j(sm2.begin_elements()), k(sm3.begin_elements()) ;
                    i != i_end ; ++i, ++j, ++k)
                {
                    if (i.index() % 10 == 0)
                    {
                        *i = DT_((i.index() +1) * 2 / 1.23456789);
                        *k = DT_((i.index() +1) * 2 / 1.23456789);
                    }
                    if (i.index() % 7 == 0)
                    {
                        *j = DT_((i.index() +1) / 1.23456789);
                        *k = DT_((i.index() +1) / -1.23456789);
                    }
                    if (i.index() % 10 == 0 && i.index() % 7 == 0)
                    {
                        *k = DT_((i.index() +1) / 1.23456789);
                    }
                }
                Difference<Tag_>::value(sm1, sm2);

                TEST_CHECK_EQUAL(sm1, sm3);
            }

            SparseMatrix<DT_> sm01(5, 5, 1), sm02(6, 6, 1), sm03(6, 5, 1);

            TEST_CHECK_THROWS(Difference<>::value(sm03, sm01), MatrixRowsDoNotMatch);
            TEST_CHECK_THROWS(Difference<>::value(sm03, sm02), MatrixColumnsDoNotMatch);
        }
};
SparseMatrixDifferenceTest<tags::CPU, float> sparse_matrix_difference_test_float("float");
SparseMatrixDifferenceTest<tags::CPU, double> sparse_matrix_difference_test_double("double");
SparseMatrixDifferenceTest<tags::CPU::MultiCore, float>  mc_sparse_matrix_difference_test_float("MC float");
SparseMatrixDifferenceTest<tags::CPU::MultiCore, double> mc_sparse_matrix_difference_test_double("MC double");

template <typename Tag_, typename DT_>
class SparseMatrixDifferenceQuickTest :
    public QuickTest
{
    public:
        SparseMatrixDifferenceQuickTest(const std::string & type) :
            QuickTest("sparse_matrix_difference_quick_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            unsigned long size (11);
            SparseMatrix<DT_> sm1(size+1, size, size / 8 + 1),
                sm2(size+1, size, size / 7 + 1), sm3(size+1, size, size / 8 + 1 );
            for (typename SparseMatrix<DT_>::ElementIterator i(sm1.begin_elements()),
                i_end(sm1.end_elements()), j(sm2.begin_elements()), k(sm3.begin_elements()) ;
                i != i_end ; ++i, ++j, ++k)
            {
                if (i.index() % 10 == 0)
                {
                    *i = DT_((i.index() +1) * 2 / 1.23456789);
                    *k = DT_((i.index() +1) * 2 / 1.23456789);
                }
                if (i.index() % 7 == 0)
                {
                    *j = DT_((i.index() +1) / 1.23456789);
                    *k = DT_((i.index() +1) / -1.23456789);
                }
                if (i.index() % 10 == 0 && i.index() % 7 == 0)
                {
                    *k = DT_((i.index() +1) / 1.23456789);
                }
            }
            Difference<Tag_>::value(sm1, sm2);

            TEST_CHECK_EQUAL(sm1, sm3);

            SparseMatrix<DT_> sm01(5, 5, 1), sm02(6, 6, 1), sm03(6, 5, 1);

            TEST_CHECK_THROWS(Difference<>::value(sm03, sm01), MatrixRowsDoNotMatch);
            TEST_CHECK_THROWS(Difference<>::value(sm03, sm02), MatrixColumnsDoNotMatch);
        }
};
SparseMatrixDifferenceQuickTest<tags::CPU, float> sparse_matrix_difference_quick_test_float("float");
SparseMatrixDifferenceQuickTest<tags::CPU, double> sparse_matrix_difference_quick_test_double("double");
SparseMatrixDifferenceQuickTest<tags::CPU::MultiCore, float>  mc_sparse_matrix_difference_quick_test_float("MC float");
SparseMatrixDifferenceQuickTest<tags::CPU::MultiCore, double> mc_sparse_matrix_difference_quick_test_double("MC double");

// Test cases for vector operations

template <typename Tag_, typename DT_>
class DenseVectorDifferenceTest :
    public BaseTest
{
    public:
        DenseVectorDifferenceTest(const std::string & type) :
            BaseTest("dense_vector_difference_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            for (unsigned long size(10) ; size < (1 << 10) ; size <<= 1)
            {
                DenseVector<DT_> dv1(size), dv2(size), dv3(size, DT_(0));

                for (typename DenseVector<DT_>::ElementIterator i(dv1.begin_elements()), i_end(dv1.end_elements()) ;
                        i != i_end ; ++i)
                {
                    *i = static_cast<DT_>((i.index() + 1) / 1.23456789);
                }
                for (typename DenseVector<DT_>::ElementIterator i(dv2.begin_elements()), i_end(dv2.end_elements()) ;
                        i != i_end ; ++i)
                {
                    *i = static_cast<DT_>((i.index() + 1) / 1.23456789);
                }
                Difference<Tag_>::value(dv1, dv2);

                TEST(dv1.lock(lm_read_only), TEST_CHECK_EQUAL(dv1, dv3), dv1.unlock(lm_read_only));
            }

            DenseVector<DT_> dv00(1, DT_(1));
            DenseVector<DT_> dv01(5, DT_(1));
            TEST_CHECK_THROWS(Difference<Tag_>::value(dv00, dv01), VectorSizeDoesNotMatch);
        }
};
DenseVectorDifferenceTest<tags::CPU, float> dense_vector_difference_test_float("float");
DenseVectorDifferenceTest<tags::CPU, double> dense_vector_difference_test_double("double");
DenseVectorDifferenceTest<tags::CPU::MultiCore, float>  mc_dense_vector_difference_test_float("MC float");
DenseVectorDifferenceTest<tags::CPU::MultiCore, double> mc_dense_vector_difference_test_double("MC double");
DenseVectorDifferenceTest<tags::CPU::Generic, float> generic_dense_vector_difference_test_float("float");
DenseVectorDifferenceTest<tags::CPU::Generic, double> generic_dense_vector_difference_test_double("double");
DenseVectorDifferenceTest<tags::CPU::MultiCore::Generic, float>  generic_mc_dense_vector_difference_test_float("MC float");
DenseVectorDifferenceTest<tags::CPU::MultiCore::Generic, double> generic_mc_dense_vector_difference_test_double("MC double");
#ifdef HONEI_SSE
DenseVectorDifferenceTest<tags::CPU::SSE, float> sse_dense_vector_difference_test_float("SSE float");
DenseVectorDifferenceTest<tags::CPU::SSE, double> sse_dense_vector_difference_test_double("SSE double");
DenseVectorDifferenceTest<tags::CPU::MultiCore::SSE, float>  mc_sse_dense_vector_difference_test_float("MC SSE float");
DenseVectorDifferenceTest<tags::CPU::MultiCore::SSE, double> mc_sse_dense_vector_difference_test_double("MC SSE double");
#endif
#ifdef HONEI_CUDA
DenseVectorDifferenceTest<tags::GPU::CUDA, float> cuda_dense_vector_difference_test_float("float");
DenseVectorDifferenceTest<tags::GPU::MultiCore::CUDA, float> mc_cuda_dense_vector_difference_test_float("float");
#ifdef HONEI_CUDA_DOUBLE
DenseVectorDifferenceTest<tags::GPU::CUDA, double> cuda_dense_vector_difference_test_double("double");
DenseVectorDifferenceTest<tags::GPU::MultiCore::CUDA, double> mc_cuda_dense_vector_difference_test_double("double");
#endif
#endif
#ifdef HONEI_CELL
DenseVectorDifferenceTest<tags::Cell, float> cell_dense_vector_difference_test_float("Cell float");
DenseVectorDifferenceTest<tags::Cell, double> cell_dense_vector_difference_test_double("Cell double");
#endif
#ifdef HONEI_OPENCL
DenseVectorDifferenceTest<tags::OpenCL::CPU, float> ocl_cpu_dense_vector_difference_test_float("float");
DenseVectorDifferenceTest<tags::OpenCL::CPU, double> ocl_cpu_dense_vector_difference_test_double("double");
DenseVectorDifferenceTest<tags::OpenCL::GPU, float> ocl_gpu_dense_vector_difference_test_float("float");
#ifdef HONEI_CUDA_DOUBLE
DenseVectorDifferenceTest<tags::OpenCL::GPU, double> ocl_gpu_dense_vector_difference_test_double("double");
#endif
#endif

template <typename Tag_, typename DT_>
class DenseVectorDifferenceQuickTest :
    public QuickTest
{
    public:
        DenseVectorDifferenceQuickTest(const std::string & type) :
            QuickTest("dense_vector_difference_quick_test<" + type + ">")
    {
            register_tag(Tag_::name);
    }

        virtual void run() const
        {
            unsigned long size(120);
            DenseVector<DT_> dv1(size), dv2(size), dv3(size, DT_(0));

            for (typename DenseVector<DT_>::ElementIterator i(dv1.begin_elements()), i_end(dv1.end_elements()) ;
                    i != i_end ; ++i)
            {
                *i = static_cast<DT_>((i.index() + 1) / 1.23456789);
            }
            for (typename DenseVector<DT_>::ElementIterator i(dv2.begin_elements()), i_end(dv2.end_elements()) ;
                    i != i_end ; ++i)
            {
                *i = static_cast<DT_>((i.index() + 1) / 1.23456789);
            }
            Difference<Tag_>::value(dv1, dv2);
            TEST(dv1.lock(lm_read_only), TEST_CHECK_EQUAL(dv1, dv3), dv1.unlock(lm_read_only));

            DenseVector<DT_> dv00(1, DT_(1));
            DenseVector<DT_> dv01(5, DT_(1));

            TEST_CHECK_THROWS(Difference<Tag_>::value(dv00, dv01), VectorSizeDoesNotMatch);
        }
};
DenseVectorDifferenceQuickTest<tags::CPU, float>  dense_vector_difference_quick_test_float("float");
DenseVectorDifferenceQuickTest<tags::CPU, double> dense_vector_difference_quick_test_double("double");
DenseVectorDifferenceQuickTest<tags::CPU::MultiCore, float>  mc_dense_vector_difference_quick_test_float("MC float");
DenseVectorDifferenceQuickTest<tags::CPU::MultiCore, double> mc_dense_vector_difference_quick_test_double("MC double");
#ifdef HONEI_SSE
DenseVectorDifferenceQuickTest<tags::CPU::SSE, float>  sse_dense_vector_difference_quick_test_float("SSE float");
DenseVectorDifferenceQuickTest<tags::CPU::SSE, double> sse_dense_vector_difference_quick_test_double("SSE double");
DenseVectorDifferenceQuickTest<tags::CPU::MultiCore::SSE, float>  mc_sse_dense_vector_difference_quick_test_float("MC SSE float");
DenseVectorDifferenceQuickTest<tags::CPU::MultiCore::SSE, double> mc_sse_dense_vector_difference_quick_test_double("MC SSE double");
#endif
#ifdef HONEI_CUDA
DenseVectorDifferenceQuickTest<tags::GPU::CUDA, float>  cuda_dense_vector_difference_quick_test_float("float");
DenseVectorDifferenceQuickTest<tags::GPU::MultiCore::CUDA, float>  mc_cuda_dense_vector_difference_quick_test_float("float");
#ifdef HONEI_CUDA_DOUBLE
DenseVectorDifferenceQuickTest<tags::GPU::CUDA, double>  cuda_dense_vector_difference_quick_test_double("double");
DenseVectorDifferenceQuickTest<tags::GPU::MultiCore::CUDA, double>  mc_cuda_dense_vector_difference_quick_test_double("double");
#endif
#endif
#ifdef HONEI_CELL
DenseVectorDifferenceQuickTest<tags::Cell, float> cell_dense_vector_difference_quick_test_float("Cell float");
DenseVectorDifferenceQuickTest<tags::Cell, double> cell_dense_vector_difference_quick_test_double("Cell double");
#endif
#ifdef HONEI_OPENCL
DenseVectorDifferenceQuickTest<tags::OpenCL::CPU, float> ocl_cpu_dense_vector_difference_quick_test_float("float");
DenseVectorDifferenceQuickTest<tags::OpenCL::CPU, double> ocl_cpu_dense_vector_difference_quick_test_double("double");
DenseVectorDifferenceQuickTest<tags::OpenCL::GPU, float> ocl_gpu_dense_vector_difference_quick_test_float("float");
#ifdef HONEI_CUDA_DOUBLE
DenseVectorDifferenceQuickTest<tags::OpenCL::GPU, double> ocl_gpu_dense_vector_difference_quick_test_double("double");
#endif
#endif


template <typename Tag_, typename DT_>
class DenseVectorDifferenceResultTest :
    public BaseTest
{
    public:
        DenseVectorDifferenceResultTest(const std::string & type) :
            BaseTest("dense_vector_difference_result_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            for (unsigned long size(10) ; size < (1 << 10) ; size <<= 1)
            {
                DenseVector<DT_> dv1(size), dv2(size), dv3(size, DT_(0));

                for (typename DenseVector<DT_>::ElementIterator i(dv1.begin_elements()), i_end(dv1.end_elements()) ;
                        i != i_end ; ++i)
                {
                    *i = static_cast<DT_>((i.index() + 1) / 1.23456789);
                }
                for (typename DenseVector<DT_>::ElementIterator i(dv2.begin_elements()), i_end(dv2.end_elements()) ;
                        i != i_end ; ++i)
                {
                    *i = static_cast<DT_>((i.index() + 1) / 1.23456789);
                }

                DenseVector<DT_> result(size);
                Difference<Tag_>::value(result, dv1, dv2);

                TEST(result.lock(lm_read_only), TEST_CHECK_EQUAL(result, dv3), result.unlock(lm_read_only));
            }

            DenseVector<DT_> dv00(1, DT_(1));
            DenseVector<DT_> dv01(5, DT_(1));
            DenseVector<DT_> dv02(3, DT_(1));
            TEST_CHECK_THROWS(Difference<Tag_>::value(dv02, dv00, dv01), VectorSizeDoesNotMatch);
        }
};
DenseVectorDifferenceResultTest<tags::CPU, float> dense_vector_difference_result_test_float("float");
DenseVectorDifferenceResultTest<tags::CPU, double> dense_vector_difference_result_test_double("double");
DenseVectorDifferenceResultTest<tags::CPU::MultiCore, float> mc_dense_vector_difference_result_test_float("float");
DenseVectorDifferenceResultTest<tags::CPU::MultiCore, double> mc_dense_vector_difference_result_test_double("double");
DenseVectorDifferenceResultTest<tags::CPU::Generic, float> generic_dense_vector_difference_result_test_float("float");
DenseVectorDifferenceResultTest<tags::CPU::Generic, double> generic_dense_vector_difference_result_test_double("double");
DenseVectorDifferenceResultTest<tags::CPU::MultiCore::Generic, float> generic_mc_dense_vector_difference_result_test_float("float");
DenseVectorDifferenceResultTest<tags::CPU::MultiCore::Generic, double> generic_mc_dense_vector_difference_result_test_double("double");
#ifdef HONEI_SSE
DenseVectorDifferenceResultTest<tags::CPU::SSE, float> sse_dense_vector_difference_result_test_float("float");
DenseVectorDifferenceResultTest<tags::CPU::SSE, double> sse_dense_vector_difference_result_test_double("double");
DenseVectorDifferenceResultTest<tags::CPU::MultiCore::SSE, float> mc_sse_dense_vector_difference_result_test_float("float");
DenseVectorDifferenceResultTest<tags::CPU::MultiCore::SSE, double> mc_sse_dense_vector_difference_result_test_double("double");
#endif
#ifdef HONEI_CUDA
DenseVectorDifferenceResultTest<tags::GPU::CUDA, float> cuda_dense_vector_difference_result_test_float("float");
DenseVectorDifferenceResultTest<tags::GPU::MultiCore::CUDA, float> mc_cuda_dense_vector_difference_result_test_float("float");
#ifdef HONEI_CUDA_DOUBLE
DenseVectorDifferenceResultTest<tags::GPU::CUDA, double> cuda_dense_vector_difference_result_test_double("double");
DenseVectorDifferenceResultTest<tags::GPU::MultiCore::CUDA, double> mc_cuda_dense_vector_difference_result_test_double("double");
#endif
#endif
#ifdef HONEI_OPENCL
DenseVectorDifferenceResultTest<tags::OpenCL::CPU, float> ocl_cpu_dense_vector_difference_result_test_float("float");
DenseVectorDifferenceResultTest<tags::OpenCL::CPU, double> ocl_cpu_dense_vector_difference_result_test_double("double");
DenseVectorDifferenceResultTest<tags::OpenCL::GPU, float> ocl_gpu_dense_vector_difference_result_test_float("float");
#ifdef HONEI_CUDA_DOUBLE
DenseVectorDifferenceResultTest<tags::OpenCL::GPU, double> ocl_gpu_dense_vector_difference_result_test_double("double");
#endif
#endif

template <typename Tag_, typename DT_>
class DenseVectorDifferenceResultQuickTest :
    public QuickTest
{
    public:
        DenseVectorDifferenceResultQuickTest(const std::string & type) :
            QuickTest("dense_vector_difference_result_quick_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            unsigned long size(4711);
            DenseVector<DT_> dv1(size), dv2(size), dv3(size, DT_(0));

            for (typename DenseVector<DT_>::ElementIterator i(dv1.begin_elements()), i_end(dv1.end_elements()) ;
                    i != i_end ; ++i)
            {
                *i = static_cast<DT_>((i.index() + 1) / 1.23456789);
            }
            for (typename DenseVector<DT_>::ElementIterator i(dv2.begin_elements()), i_end(dv2.end_elements()) ;
                    i != i_end ; ++i)
            {
                *i = static_cast<DT_>((i.index() + 1) / 1.23456789);
            }

            DenseVector<DT_> result(size);
            Difference<Tag_>::value(result, dv1, dv2);

            TEST(result.lock(lm_read_only), TEST_CHECK_EQUAL(result, dv3), result.unlock(lm_read_only));

            DenseVector<DT_> dv00(1, DT_(1));
            DenseVector<DT_> dv01(5, DT_(1));
            DenseVector<DT_> dv02(3, DT_(1));
            TEST_CHECK_THROWS(Difference<Tag_>::value(dv02, dv00, dv01), VectorSizeDoesNotMatch);
        }
};
DenseVectorDifferenceResultQuickTest<tags::CPU, float> dense_vector_difference_result_quick_test_float("float");
DenseVectorDifferenceResultQuickTest<tags::CPU, double> dense_vector_difference_result_quick_test_double("double");
DenseVectorDifferenceResultQuickTest<tags::CPU::MultiCore, float> mc_dense_vector_difference_result_quick_test_float("float");
DenseVectorDifferenceResultQuickTest<tags::CPU::MultiCore, double> mc_dense_vector_difference_result_quick_test_double("double");
DenseVectorDifferenceResultQuickTest<tags::CPU::Generic, float> generic_dense_vector_difference_result_quick_test_float("float");
DenseVectorDifferenceResultQuickTest<tags::CPU::Generic, double> generic_dense_vector_difference_result_quick_test_double("double");
DenseVectorDifferenceResultQuickTest<tags::CPU::MultiCore::Generic, float> generic_mc_dense_vector_difference_result_quick_test_float("float");
DenseVectorDifferenceResultQuickTest<tags::CPU::MultiCore::Generic, double> generic_mc_dense_vector_difference_result_quick_test_double("double");
#ifdef HONEI_SSE
DenseVectorDifferenceResultQuickTest<tags::CPU::SSE, float> sse_dense_vector_difference_result_quick_test_float("float");
DenseVectorDifferenceResultQuickTest<tags::CPU::SSE, double> sse_dense_vector_difference_result_quick_test_double("double");
DenseVectorDifferenceResultQuickTest<tags::CPU::MultiCore::SSE, float> mc_sse_dense_vector_difference_result_quick_test_float("float");
DenseVectorDifferenceResultQuickTest<tags::CPU::MultiCore::SSE, double> mc_sse_dense_vector_difference_result_quick_test_double("double");
#endif
#ifdef HONEI_CUDA
DenseVectorDifferenceResultQuickTest<tags::GPU::CUDA, float> cuda_dense_vector_difference_result_quick_test_float("float");
DenseVectorDifferenceResultQuickTest<tags::GPU::MultiCore::CUDA, float> mc_cuda_dense_vector_difference_result_quick_test_float("float");
#ifdef HONEI_CUDA_DOUBLE
DenseVectorDifferenceResultQuickTest<tags::GPU::CUDA, double> cuda_dense_vector_difference_result_quick_test_double("double");
DenseVectorDifferenceResultQuickTest<tags::GPU::MultiCore::CUDA, double> mc_cuda_dense_vector_difference_result_quick_test_double("double");
#endif
#endif
#ifdef HONEI_OPENCL
DenseVectorDifferenceResultQuickTest<tags::OpenCL::CPU, float> ocl_cpu_dense_vector_difference_result_quick_test_float("float");
DenseVectorDifferenceResultQuickTest<tags::OpenCL::CPU, double> ocl_cpu_dense_vector_difference_result_quick_test_double("double");
DenseVectorDifferenceResultQuickTest<tags::OpenCL::GPU, float> ocl_gpu_dense_vector_difference_result_quick_test_float("float");
#ifdef HONEI_CUDA_DOUBLE
DenseVectorDifferenceResultQuickTest<tags::OpenCL::GPU, double> ocl_gpu_dense_vector_difference_result_quick_test_double("double");
#endif
#endif

template <typename Tag_, typename DT_>
class DenseVectorRangeDifferenceTest :
    public BaseTest
{
    public:
        DenseVectorRangeDifferenceTest(const std::string & type) :
            BaseTest("dense_vector_range_difference_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            for (unsigned long size(10) ; size < (1 << 10) ; size <<= 1)
            {
                for (int i(0) ; i < 4 ; ++i)
                {
                    for (int j(0) ; j < 4 ; ++j)
                    {
                        DenseVector<DT_> dv1(size+3), dv2(size+3), dv3(size, DT_(i-j));
                        for (typename DenseVector<DT_>::ElementIterator k(dv1.begin_elements()), l(dv2.begin_elements()),
                                    k_end(dv1.end_elements()), l_end(dv2.end_elements()) ; k != k_end ; ++k, ++l)
                        {
                            *k = k.index();
                            *l = k.index();
                        }

                        DenseVectorRange<DT_> dvr1(dv1, size, i), dvr2(dv2, size, j);
                        Difference<Tag_>::value(dvr1, dvr2);
                        TEST(dvr1.lock(lm_read_only), TEST_CHECK_EQUAL(dvr1, dv3), dvr1.unlock(lm_read_only));
                    }
                }
            }

            DenseVector<DT_> dv1(4, DT_(3)), dv2(4, DT_(2));
            DenseVectorRange<DT_> dvr1(dv1, 3, 1), dvr2(dv2, 2, 2);
            TEST_CHECK_THROWS(Difference<Tag_>::value(dvr1, dvr2), VectorSizeDoesNotMatch);
        }
};
DenseVectorRangeDifferenceTest<tags::CPU, float> dense_vector_range_difference_test_float("float");
DenseVectorRangeDifferenceTest<tags::CPU, double> dense_vector_range_difference_test_double("double");
DenseVectorRangeDifferenceTest<tags::CPU::MultiCore, float> mc_dense_vector_range_difference_test_float("MC float");
DenseVectorRangeDifferenceTest<tags::CPU::MultiCore, double> mc_dense_vector_range_difference_test_double("MC double");
#ifdef HONEI_SSE
DenseVectorRangeDifferenceTest<tags::CPU::SSE, float> sse_dense_vector_range_difference_test_float("SSE float");
DenseVectorRangeDifferenceTest<tags::CPU::SSE, double> sse_dense_vector_range_difference_test_double("SSE double");
DenseVectorRangeDifferenceTest<tags::CPU::MultiCore::SSE, float> mc_sse_dense_vector_range_difference_test_float("MC SSE float");
DenseVectorRangeDifferenceTest<tags::CPU::MultiCore::SSE, double> mc_sse_dense_vector_range_difference_test_double("MC SSE double");
#endif
#ifdef HONEI_CUDA
DenseVectorRangeDifferenceTest<tags::GPU::CUDA, float> cuda_dense_vector_range_difference_test_float("float");
DenseVectorRangeDifferenceTest<tags::GPU::MultiCore::CUDA, float> mc_cuda_dense_vector_range_difference_test_float("float");
#ifdef HONEI_CUDA_DOUBLE
DenseVectorRangeDifferenceTest<tags::GPU::CUDA, double> cuda_dense_vector_range_difference_test_double("double");
DenseVectorRangeDifferenceTest<tags::GPU::MultiCore::CUDA, double> mc_cuda_dense_vector_range_difference_test_double("double");
#endif
#endif
#ifdef HONEI_CELL
DenseVectorRangeDifferenceTest<tags::Cell, float> cell_dense_vector_range_difference_test_float("Cell float");
DenseVectorRangeDifferenceTest<tags::Cell, double> cell_dense_vector_range_difference_test_double("Cell double");
#endif
#ifdef HONEI_OPENCL
DenseVectorRangeDifferenceTest<tags::OpenCL::CPU, float> ocl_cpu_dense_vector_range_difference_test_float("float");
DenseVectorRangeDifferenceTest<tags::OpenCL::CPU, double> ocl_cpu_dense_vector_range_difference_test_double("double");
DenseVectorRangeDifferenceTest<tags::OpenCL::GPU, float> ocl_gpu_dense_vector_range_difference_test_float("float");
#ifdef HONEI_CUDA_DOUBLE
DenseVectorRangeDifferenceTest<tags::OpenCL::GPU, double> ocl_gpu_dense_vector_range_difference_test_double("double");
#endif
#endif

template <typename Tag_, typename DT_>
class DenseVectorRangeDifferenceQuickTest :
    public QuickTest
{
    public:
        DenseVectorRangeDifferenceQuickTest(const std::string & type) :
            QuickTest("dense_vector_range_difference_quick_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            unsigned long size(123);
            for (int i(0) ; i < 4 ; ++i)
            {
                for (int j(0) ; j < 4 ; ++j)
                {
                    DenseVector<DT_> dv1(size+3), dv2(size+3), dv3(size, DT_(i-j));
                    for (typename DenseVector<DT_>::ElementIterator k(dv1.begin_elements()), l(dv2.begin_elements()),
                                k_end(dv1.end_elements()), l_end(dv2.end_elements()) ; k != k_end ; ++k, ++l)
                    {
                        *k = k.index();
                        *l = k.index();
                    }

                    DenseVectorRange<DT_> dvr1(dv1, size, i), dvr2(dv2, size, j);
                    Difference<Tag_>::value(dvr1, dvr2);
                    TEST(dvr1.lock(lm_read_only), TEST_CHECK_EQUAL(dvr1, dv3), dvr1.unlock(lm_read_only));
                }
            }

            DenseVector<DT_> dv1(4, DT_(3)), dv2(4, DT_(2));
            DenseVectorRange<DT_> dvr1(dv1, 3, 1), dvr2(dv2, 2, 2);
            TEST_CHECK_THROWS(Difference<Tag_>::value(dvr1, dvr2), VectorSizeDoesNotMatch);
        }
};
DenseVectorRangeDifferenceQuickTest<tags::CPU, float> dense_vector_range_difference_quick_test_float("float");
DenseVectorRangeDifferenceQuickTest<tags::CPU, double> dense_vector_range_difference_quick_test_double("double");
DenseVectorRangeDifferenceQuickTest<tags::CPU::MultiCore, float> mc_dense_vector_range_difference_quick_test_float("MC float");
DenseVectorRangeDifferenceQuickTest<tags::CPU::MultiCore, double> mc_dense_vector_range_difference_quick_test_double("MC double");
#ifdef HONEI_SSE
DenseVectorRangeDifferenceQuickTest<tags::CPU::SSE, float> sse_dense_vector_range_difference_quick_test_float("SSE float");
DenseVectorRangeDifferenceQuickTest<tags::CPU::SSE, double> sse_dense_vector_range_difference_quick_test_double("SSE double");
DenseVectorRangeDifferenceQuickTest<tags::CPU::MultiCore::SSE, float> mc_sse_dense_vector_range_difference_quick_test_float("MC SSE float");
DenseVectorRangeDifferenceQuickTest<tags::CPU::MultiCore::SSE, double> mc_sse_dense_vector_range_difference_quick_test_double("MC SSE double");
#endif
#ifdef HONEI_CUDA
DenseVectorRangeDifferenceQuickTest<tags::GPU::CUDA, float> cuda_dense_vector_range_difference_quick_test_float("float");
DenseVectorRangeDifferenceQuickTest<tags::GPU::MultiCore::CUDA, float> mc_cuda_dense_vector_range_difference_quick_test_float("float");
#ifdef HONEI_CUDA_DOUBLE
DenseVectorRangeDifferenceQuickTest<tags::GPU::CUDA, double> cuda_dense_vector_range_difference_quick_test_double("double");
DenseVectorRangeDifferenceQuickTest<tags::GPU::MultiCore::CUDA, double> mc_cuda_dense_vector_range_difference_quick_test_double("double");
#endif
#endif
#ifdef HONEI_CELL
DenseVectorRangeDifferenceQuickTest<tags::Cell, float> cell_dense_vector_range_difference_quick_test_float("Cell float");
DenseVectorRangeDifferenceQuickTest<tags::Cell, double> cell_dense_vector_range_difference_quick_test_double("Cell double");
#endif
#ifdef HONEI_OPENCL
DenseVectorRangeDifferenceQuickTest<tags::OpenCL::CPU, float> ocl_cpu_dense_vector_range_difference_quick_test_float("float");
DenseVectorRangeDifferenceQuickTest<tags::OpenCL::CPU, double> ocl_cpu_dense_vector_range_difference_quick_test_double("double");
DenseVectorRangeDifferenceQuickTest<tags::OpenCL::GPU, float> ocl_gpu_dense_vector_range_difference_quick_test_float("float");
#ifdef HONEI_CUDA_DOUBLE
DenseVectorRangeDifferenceQuickTest<tags::OpenCL::GPU, double> ocl_gpu_dense_vector_range_difference_quick_test_double("double");
#endif
#endif

template <typename Tag_, typename DT_>
class DenseVectorSparseVectorDifferenceTest :
    public BaseTest
{
    public:
        DenseVectorSparseVectorDifferenceTest(const std::string & type) :
            BaseTest("dense_vector_sparse_vector_difference_test<" + type + ">")
    {
            register_tag(Tag_::name);
    }

        virtual void run() const
        {
            for (unsigned long size(1) ; size < (1 << 10) ; size <<= 1)
            {
                DenseVector<DT_> dv1(size, DT_(0)), dv3(size, DT_(0));
                SparseVector<DT_> sv2(size, size / 7 + 1);
                typename DenseVector<DT_>::ElementIterator i(dv1.begin_elements()), i_end(dv1.end_elements()), k(dv3.begin_elements());
                for (typename SparseVector<DT_>::ElementIterator j(sv2.begin_elements()) ; i != i_end ; ++i, ++j, ++k)
                {
                    if (i.index() % 10 == 0)
                    {
                        *i = static_cast<DT_>(((i.index() +1) * 2) / 1.23456789);
                        *k = static_cast<DT_>(((i.index() +1) * 2) / 1.23456789);
                    }
                    if (i.index() % 7 == 0)
                    {
                        *j = static_cast<DT_>((i.index() +1) / 1.23456789);
                        *k = static_cast<DT_>((i.index() +1) / -1.23456789);
                    }
                    if (i.index() % 7 == 0 && i.index() % 10 == 0)
                    {
                        *k = static_cast<DT_>((i.index() +1) / 1.23456789);
                    }
                }
                Difference<Tag_>::value(dv1, sv2);
                TEST_CHECK_EQUAL(dv1, dv3);
            }

            DenseVector<DT_> dv00(1);
            SparseVector<DT_> sv01(5, 1);
            TEST_CHECK_THROWS(Difference<>::value(dv00, sv01), VectorSizeDoesNotMatch);
        }
};
DenseVectorSparseVectorDifferenceTest<tags::CPU, float> dense_vector_sparse_vector_difference_test_float("float");
DenseVectorSparseVectorDifferenceTest<tags::CPU, double> dense_vector_sparse_vector_difference_test_double("double");
DenseVectorSparseVectorDifferenceTest<tags::CPU::MultiCore, float>  mc_dense_vector_sparse_vector_difference_test_float("MC float");
DenseVectorSparseVectorDifferenceTest<tags::CPU::MultiCore, double> mc_dense_vector_sparse_vector_difference_test_double("MC double");

template <typename Tag_, typename DT_>
class DenseVectorSparseVectorDifferenceQuickTest :
    public QuickTest
{
    public:
        DenseVectorSparseVectorDifferenceQuickTest(const std::string & type) :
            QuickTest("dense_vector_sparse_vector_difference_quick_test<" + type + ">")
    {
            register_tag(Tag_::name);
    }

        virtual void run() const
        {
            unsigned long size(22);
            DenseVector<DT_> dv1(size, DT_(0)), dv3(size, DT_(0));
            SparseVector<DT_> sv2(size, size / 7 + 1);
            typename DenseVector<DT_>::ElementIterator i(dv1.begin_elements()), i_end(dv1.end_elements()), k(dv3.begin_elements());
            for (typename SparseVector<DT_>::ElementIterator j(sv2.begin_elements()) ; i != i_end ; ++i, ++j, ++k)
            {
                if (i.index() % 10 == 0)
                {
                    *i = static_cast<DT_>(((i.index() +1) * 2) / 1.23456789);
                    *k = static_cast<DT_>(((i.index() +1) * 2) / 1.23456789);
                }
                if (i.index() % 7 == 0)
                {
                    *j = static_cast<DT_>((i.index() +1) / 1.23456789);
                    *k = static_cast<DT_>((i.index() +1) / -1.23456789);
                }
                if (i.index() % 7 == 0 && i.index() % 10 == 0)
                {
                    *k = static_cast<DT_>((i.index() +1) / 1.23456789);
                }
            }
            Difference<>::value(dv1, sv2);
            TEST_CHECK_EQUAL(dv1, dv3);

            DenseVector<DT_> dv00(1);
            SparseVector<DT_> sv01(5, 1);
            TEST_CHECK_THROWS(Difference<>::value(dv00, sv01), VectorSizeDoesNotMatch);
        }
};
DenseVectorSparseVectorDifferenceQuickTest<tags::CPU, float> dense_vector_sparse_vector_difference_quick_test_float("float");
DenseVectorSparseVectorDifferenceQuickTest<tags::CPU, double> dense_vector_sparse_vector_difference_quick_test_double("double");
DenseVectorSparseVectorDifferenceQuickTest<tags::CPU::MultiCore, float>  mc_dense_vector_sparse_vector_difference_quick_test_float("MC float");
DenseVectorSparseVectorDifferenceQuickTest<tags::CPU::MultiCore, double> mc_dense_vector_sparse_vector_difference_quick_test_double("MC double");

template <typename DT_>
class SparseVectorDifferenceTest :
    public BaseTest
{
    public:
        SparseVectorDifferenceTest(const std::string & type) :
            BaseTest("sparse_vector_difference_test<" + type + ">")
        {
        }

        virtual void run() const
        {
            for (unsigned long size(1) ; size < (1 << 10) ; size <<= 1)
            {
                SparseVector<DT_> sv1(size, size / 8 + 1), sv2(size, size / 7 + 1), sv3(size, size / 8 + 1);
                for (typename SparseVector<DT_>::ElementIterator i(sv1.begin_elements()), i_end(sv1.end_elements()),
                    j(sv2.begin_elements()), k(sv3.begin_elements()) ; i != i_end ; ++i, ++j, ++k)
                {
                    if ((i.index() % 7 == 0) && (i.index() % 10 == 0))
                    {
                        *i = DT_((i.index() + 1) / 1.23456789);
                        *j = DT_(((i.index() +1) * 2) / 1.23456789);
                        *k = -*i;
                    }
                    else if (i.index() % 7 == 0)
                    {
                        *i = DT_((i.index() +1) / 1.23456789);
                        *k = *i;
                    }
                    else if (i.index() % 10 == 0)
                    {
                        *j = DT_(((i.index() +1) * 2) / 1.23456789);
                        *k = -*j;
                    }
                }

                Difference<>::value(sv1, sv2);
                TEST_CHECK_EQUAL(sv1, sv3);
            }

            SparseVector<DT_> sv00(1, 1), sv01(5, 1);
            TEST_CHECK_THROWS(Difference<>::value(sv00, sv01), VectorSizeDoesNotMatch);
        }
};
SparseVectorDifferenceTest<float> sparse_vector_difference_test_float("float");
SparseVectorDifferenceTest<double> sparse_vector_difference_test_double("double");

template <typename DT_>
class SparseVectorDifferenceQuickTest :
    public QuickTest
{
    public:
        SparseVectorDifferenceQuickTest(const std::string & type) :
            QuickTest("sparse_vector_difference_quick_test<" + type + ">")
        {
        }

        virtual void run() const
        {
            unsigned long size(25);
            SparseVector<DT_> sv1(size, size / 8 + 1), sv2(size, size / 7 + 1), sv3(size, size / 8 + 1);
            for (typename SparseVector<DT_>::ElementIterator i(sv1.begin_elements()), i_end(sv1.end_elements()),
                j(sv2.begin_elements()), k(sv3.begin_elements()) ; i != i_end ; ++i, ++j, ++k)
            {
                if ((i.index() % 7 == 0) && (i.index() % 10 == 0))
                {
                    *i = DT_((i.index() + 1) / 1.23456789);
                    *j = DT_(((i.index() +1) * 2) / 1.23456789);
                    *k = -*i;
                }
                else if (i.index() % 7 == 0)
                {
                    *i = DT_((i.index() +1) / 1.23456789);
                    *k = *i;
                }
                else if (i.index() % 10 == 0)
                {
                    *j = DT_(((i.index() +1) * 2) / 1.23456789);
                    *k = -*j;
                }
            }

            Difference<>::value(sv1, sv2);
            TEST_CHECK_EQUAL(sv1, sv3);

            SparseVector<DT_> sv00(1, 1), sv01(5, 1);
            TEST_CHECK_THROWS(Difference<>::value(sv00, sv01), VectorSizeDoesNotMatch);
        }
};
SparseVectorDifferenceQuickTest<float> sparse_vector_difference_quick_test_float("float");
SparseVectorDifferenceQuickTest<double> sparse_vector_difference_quick_test_double("double");

template <typename Tag_, typename DT_>
class SparseVectorDenseVectorDifferenceTest :
    public BaseTest
{
    public:
        SparseVectorDenseVectorDifferenceTest(const std::string & type) :
            BaseTest("sparse_vector_dense_vector_difference_test<" + type + ">")
    {
        register_tag(Tag_::name);
    }

        virtual void run() const
        {
            for (unsigned long size(1) ; size < (1 << 10) ; size <<= 1)
            {
                SparseVector<DT_> sv1(size, size / 7 + 1);
                DenseVector<DT_> dv2(size, DT_(0)), dv3(size, DT_(0));

                typename SparseVector<DT_>::ElementIterator i(sv1.begin_elements()), i_end(sv1.end_elements());
                for (typename DenseVector<DT_>::ElementIterator j(dv2.begin_elements()), k(dv3.begin_elements()) ; i != i_end ; ++i, ++j, ++k)
                {
                    if (i.index() % 10 == 0)
                    {
                        *i = static_cast<DT_>(((i.index() +1) * 2) / 1.23456789);
                        *k = static_cast<DT_>(((i.index() +1) * 2) / 1.23456789);
                    }
                    if (i.index() % 7 == 0)
                    {
                        *j = static_cast<DT_>((i.index() +1) / 1.23456789);
                        *k = static_cast<DT_>((i.index() +1) / -1.23456789);
                    }
                    if (i.index() % 7 == 0 && i.index() % 10 == 0)
                    {
                        *k = static_cast<DT_>((i.index() +1) / 1.23456789);
                    }
                }
               // CPU: dv2 is changed. Cell: new DV is returned.
                Difference<Tag_>::value(sv1, dv2);
                TEST_CHECK_EQUAL(dv2, dv3);
            }

            SparseVector<DT_> sv00(5, 1);
            DenseVector<DT_> dv01(6);
            TEST_CHECK_THROWS(Difference<Tag_>::value(sv00, dv01), VectorSizeDoesNotMatch);
        }
};
SparseVectorDenseVectorDifferenceTest<tags::CPU, float> sparse_vector_dense_vector_difference_test_float("float");
SparseVectorDenseVectorDifferenceTest<tags::CPU, double> sparse_vector_dense_vector_difference_test_double("double");

template <typename Tag_, typename DT_>
class SparseVectorDenseVectorDifferenceQuickTest :
    public QuickTest
{
    public:
        SparseVectorDenseVectorDifferenceQuickTest(const std::string & type) :
            QuickTest("sparse_vector_dense_vector_difference_quick_test<" + type + ">")
    {
        register_tag(Tag_::name);
    }

        virtual void run() const
        {
           unsigned long size(22);

            SparseVector<DT_> sv1(size, size / 7 + 1);
            DenseVector<DT_> dv2(size, DT_(0)), dv3(size, DT_(0));
            typename SparseVector<DT_>::ElementIterator i(sv1.begin_elements()), i_end(sv1.end_elements());
            for (typename DenseVector<DT_>::ElementIterator j(dv2.begin_elements()), k(dv3.begin_elements()) ; i != i_end ; ++i, ++j, ++k)
            {
                if (i.index() % 10 == 0)
                {
                    *i = static_cast<DT_>(((i.index() +1) * 2) / 1.23456789);
                    *k = static_cast<DT_>(((i.index() +1) * 2) / 1.23456789);
                }
                if (i.index() % 7 == 0)
                {
                    *j = static_cast<DT_>((i.index() +1) / 1.23456789);
                    *k = static_cast<DT_>((i.index() +1) / -1.23456789);
                }
                if (i.index() % 7 == 0 && i.index() % 10 == 0)
                {
                    *k = static_cast<DT_>((i.index() +1) / 1.23456789);
                }
            }

            // CPU: dv2 is changed. Cell: new DV is returned.
            Difference<Tag_>::value(sv1, dv2);
            TEST_CHECK_EQUAL(dv2, dv3);

            SparseVector<DT_> sv00(5, 1);
            DenseVector<DT_> dv01(6);
            TEST_CHECK_THROWS(Difference<Tag_>::value(sv00, dv01), VectorSizeDoesNotMatch);
        }
};
SparseVectorDenseVectorDifferenceQuickTest<tags::CPU, float> sparse_vector_dense_vector_difference_quick_test_float("float");
SparseVectorDenseVectorDifferenceQuickTest<tags::CPU, double> sparse_vector_dense_vector_difference_quick_test_double("double");

template <typename Tag_, typename DT_>
class SparseVectorDenseVectorDifferenceCellTest :
    public BaseTest
{
    public:
        SparseVectorDenseVectorDifferenceCellTest(const std::string & type) :
            BaseTest("sparse_vector_dense_vector_difference_cell_test<" + type + ">")
    {
        register_tag(Tag_::name);
    }

        virtual void run() const
        {
            for (unsigned long size(1) ; size < (1 << 10) ; size <<= 1)
            {
                SparseVector<DT_> sv1(size, size / 7 + 1);
                DenseVector<DT_> dv2(size, DT_(0)), dv3(size, DT_(0));

                typename DenseVector<DT_>::ElementIterator j(dv2.begin_elements()), k(dv3.begin_elements());
                for (typename SparseVector<DT_>::ElementIterator i(sv1.begin_elements()), i_end(sv1.end_elements()) ;
                        i != i_end ; ++i, ++j, ++k)
                {
                    if (i.index() % 10 == 0)
                    {
                        *i = static_cast<DT_>(((i.index() +1) * 2) / 1.23456789);
                        *k = static_cast<DT_>(((i.index() +1) * 2) / 1.23456789);
                    }
                    if (i.index() % 7 == 0)
                    {
                        *j = static_cast<DT_>((i.index() +1) / 1.23456789);
                        *k = static_cast<DT_>((i.index() +1) / -1.23456789);
                    }
                    if (i.index() % 7 == 0 && i.index() % 10 == 0)
                    {
                        *k = static_cast<DT_>((i.index() +1) / 1.23456789);
                    }
                }
               // CPU: dv2 is changed. Cell: new DV is returned.
                DenseVector<DT_> diff(Difference<Tag_>::value(sv1, dv2));
                TEST_CHECK_EQUAL(diff, dv3);
            }

            SparseVector<DT_> sv00(5, 1);
            DenseVector<DT_> dv01(6);
            TEST_CHECK_THROWS(Difference<Tag_>::value(sv00, dv01), VectorSizeDoesNotMatch);
        }
};
#ifdef HONEI_CELL
SparseVectorDenseVectorDifferenceCellTest<tags::Cell, float> cell_sparse_vector_dense_vector_difference_test_float("Cell float");
#endif


template <typename Tag_, typename DT_>
class SparseVectorDenseVectorDifferenceCellQuickTest :
    public QuickTest
{
    public:
        SparseVectorDenseVectorDifferenceCellQuickTest(const std::string & type) :
            QuickTest("sparse_vector_dense_vector_difference_cell_quick_test<" + type + ">")
    {
        register_tag(Tag_::name);
    }

        virtual void run() const
        {
           unsigned long size(22);

            SparseVector<DT_> sv1(size, size / 7 + 1);
            DenseVector<DT_> dv2(size, DT_(0)), dv3(size, DT_(0));

            typename DenseVector<DT_>::ElementIterator j(dv2.begin_elements()), k(dv3.begin_elements());
            for (typename SparseVector<DT_>::ElementIterator i(sv1.begin_elements()), i_end(sv1.end_elements()) ;
                   i != i_end ; ++i, ++j, ++k)
            {
                if (i.index() % 10 == 0)
                {
                    *i = static_cast<DT_>(((i.index() +1) * 2) / 1.23456789);
                    *k = static_cast<DT_>(((i.index() +1) * 2) / 1.23456789);
                }
                if (i.index() % 7 == 0)
                {
                    *j = static_cast<DT_>((i.index() +1) / 1.23456789);
                    *k = static_cast<DT_>((i.index() +1) / -1.23456789);
                }
                if (i.index() % 7 == 0 && i.index() % 10 == 0)
                {
                    *k = static_cast<DT_>((i.index() +1) / 1.23456789);
                }
            }

            // CPU: dv2 is changed. Cell: new DV is returned.
            DenseVector<DT_> diff(Difference<Tag_>::value(sv1, dv2));
            TEST_CHECK_EQUAL(diff, dv3);

            SparseVector<DT_> sv00(5, 1);
            DenseVector<DT_> dv01(6);
            TEST_CHECK_THROWS(Difference<Tag_>::value(sv00, dv01), VectorSizeDoesNotMatch);
        }
};
#ifdef HONEI_CELL
SparseVectorDenseVectorDifferenceCellQuickTest<tags::Cell, float> cell_sparse_vector_dense_vector_difference_quick_test_float("Cell float");
#endif

