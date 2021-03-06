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
#include <honei/la/reduction.hh>
#include <matrix_error.hh>
#include <honei/util/unittest.hh>

#include <limits>
#include <cstdlib>
#include <cmath>

using namespace honei;
using namespace tests;

template <typename Tag_, typename DT_>
class BandedMatrixReductionToSumTest :
    public BaseTest
{
    public:
        BandedMatrixReductionToSumTest(const std::string & type) :
            BaseTest("banded_matrix_reduction_to_sum_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            for (unsigned long size(10) ; size < (1 << 9) ; size <<= 1)
            {
                DenseVector<DT_> dv1(size, DT_(2));
                DenseVector<DT_> dv2(size, DT_(-2));
                BandedMatrix<DT_> bm1(size, dv1);
                bm1.insert_band(1, dv1);
                bm1.insert_band((size - 1), dv2);
                bm1.insert_band(-1, dv2);
                bm1.insert_band(-size +1, dv1);
                bm1.insert_band(2, dv1);
                bm1.insert_band(size - 2, dv2);
                bm1.insert_band(-2, dv2);
                bm1.insert_band(-size + 2, dv1);
                DenseVector<DT_> sum(Reduction<rt_sum, Tag_>::value(bm1));

                for(typename DenseVector<DT_>::ConstElementIterator i(sum.begin_elements()),
                        i_end(sum.end_elements()), j(dv1.begin_elements()) ; i != i_end ; ++i, ++j)
                {
                    TEST_CHECK_EQUAL_WITHIN_EPS(*i, *j, std::numeric_limits<DT_>::epsilon());
                }
            }
        }
};
BandedMatrixReductionToSumTest<tags::CPU, float> banded_matrix_reduction_to_sum_test_float("float");
BandedMatrixReductionToSumTest<tags::CPU, double> banded_matrix_reduction_to_sum_test_double("double");
BandedMatrixReductionToSumTest<tags::CPU::MultiCore, float> mc_banded_matrix_reduction_to_sum_test_float("MC float");
BandedMatrixReductionToSumTest<tags::CPU::MultiCore, double> mc_banded_matrix_reduction_to_sum_test_double("MC double");

template <typename Tag_, typename DT_>
class BandedMatrixReductionQuickTest :
    public QuickTest
{
    public:
        BandedMatrixReductionQuickTest(const std::string & type) :
            QuickTest("banded_matrix_reduction_to_sum_quick_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            unsigned long size(20);
            DenseVector<DT_> dv1(size, DT_(2));
            DenseVector<DT_> dv2(size, DT_(-2));
            BandedMatrix<DT_> bm1(size, dv1);
            bm1.insert_band(1, dv1);
            bm1.insert_band((size - 1), dv2);
            bm1.insert_band(-1, dv2);
            bm1.insert_band(-size +1, dv1);
            bm1.insert_band(2, dv1);
            bm1.insert_band(size - 2, dv2);
            bm1.insert_band(-2, dv2);
            bm1.insert_band(-size + 2, dv1);
            DenseVector<DT_> sum(Reduction<rt_sum, Tag_>::value(bm1));

            for(typename DenseVector<DT_>::ConstElementIterator i(sum.begin_elements()),
                    i_end(sum.end_elements()), j(dv1.begin_elements()) ; i != i_end ; ++i, ++j)
            {
                TEST_CHECK_EQUAL_WITHIN_EPS(*i, *j, std::numeric_limits<DT_>::epsilon());
            }
        }
};
BandedMatrixReductionQuickTest<tags::CPU, float> banded_matrix_reduction_to_sum_quick_test_float("float");
BandedMatrixReductionQuickTest<tags::CPU, double> banded_matrix_reduction_to_sum_quick_test_double("double");
BandedMatrixReductionQuickTest<tags::CPU::MultiCore, float> mc_banded_matrix_reduction_to_sum_quick_test_float("MC float");
BandedMatrixReductionQuickTest<tags::CPU::MultiCore, double> mc_banded_matrix_reduction_to_sum_quick_test_double("MC double");

template <typename Tag_, typename DT_>
class DenseMatrixReductionToSumTest :
    public BaseTest
{
    public:
        DenseMatrixReductionToSumTest(const std::string & type) :
            BaseTest("dense_matrix_reduction_to_sum_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            for (unsigned long size(1) ; size < (1 << 9) ; size <<= 1)
            {
                DenseMatrix<DT_> dm1(size, size);
                DenseVector<DT_> dv1(size, DT_(0));
                for(typename DenseMatrix<DT_>::ElementIterator i(dm1.begin_elements()),
                        i_end(dm1.end_elements()) ; i != i_end ; ++i)
                {
                    *i = i.index();
                    dv1[i.row()] += i.index();
                }

                DenseVector<DT_> sum(Reduction<rt_sum, Tag_>::value(dm1));

                for(typename DenseVector<DT_>::ConstElementIterator i(sum.begin_elements()),
                        i_end(sum.end_elements()), j(dv1.begin_elements()) ; i != i_end ; ++i, ++j)
                {
                    TEST_CHECK_EQUAL_WITHIN_EPS(*i, *j, std::numeric_limits<DT_>::epsilon());
                }
            }
        }
};

DenseMatrixReductionToSumTest<tags::CPU, float> dense_matrix_reduction_to_sum_test_float("float");
DenseMatrixReductionToSumTest<tags::CPU, double> dense_matrix_reduction_to_sum_test_double("double");
DenseMatrixReductionToSumTest<tags::CPU::MultiCore, float> mc_dense_matrix_reduction_to_sum_test_float("MC float");
DenseMatrixReductionToSumTest<tags::CPU::MultiCore, double> mc_dense_matrix_reduction_to_sum_test_double("MC double");
#ifdef HONEI_SSE
DenseMatrixReductionToSumTest<tags::CPU::SSE, float> sse_dense_matrix_reduction_to_sum_test_float("SSE float");
DenseMatrixReductionToSumTest<tags::CPU::SSE, double> sse_dense_matrix_reduction_to_sum_test_double("SSE double");
DenseMatrixReductionToSumTest<tags::CPU::MultiCore::SSE, float> sse_mc_dense_matrix_reduction_to_sum_test_float("MC SSE float");
DenseMatrixReductionToSumTest<tags::CPU::MultiCore::SSE, double> sse_mc_dense_matrix_reduction_to_sum_test_double("MC SSE double");
#endif
#ifdef HONEI_CELL
DenseMatrixReductionToSumTest<tags::Cell, float> cell_dense_matrix_reduction_to_sum_test_float("Cell float");
DenseMatrixReductionToSumTest<tags::Cell, double> cell_dense_matrix_reduction_to_sum_test_double("Cell double");
#endif

template <typename Tag_, typename DT_>
class DenseMatrixReductionQuickTest :
    public QuickTest
{
    public:
        DenseMatrixReductionQuickTest(const std::string & type) :
            QuickTest("dense_matrix_reduction_to_sum_quick_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            unsigned long size(5);
            DenseMatrix<DT_> dm1(size, size);
            DenseVector<DT_> dv1(size, DT_(0));
            for(typename DenseMatrix<DT_>::ElementIterator i(dm1.begin_elements()), 
                    i_end(dm1.end_elements()) ; i != i_end ; ++i)
            {
                *i = i.index();
                dv1[i.row()] += i.index();
            }
            DenseVector<DT_> sum(Reduction<rt_sum, Tag_>::value(dm1));

            for(typename DenseVector<DT_>::ConstElementIterator i(sum.begin_elements()), i_end(sum.end_elements()), j(dv1.begin_elements()) ;
                    i != i_end ; ++i, ++j)
            {
                TEST_CHECK_EQUAL_WITHIN_EPS(*i, *j, std::numeric_limits<DT_>::epsilon());
            }
        }
};
DenseMatrixReductionQuickTest<tags::CPU, float> dense_matrix_reduction_to_sum_quick_test_float("float");
DenseMatrixReductionQuickTest<tags::CPU, double> dense_matrix_reduction_to_sum_quick_test_double("double");
DenseMatrixReductionQuickTest<tags::CPU::MultiCore, float> mc_dense_matrix_reduction_to_sum_quick_test_float("MC float");
DenseMatrixReductionQuickTest<tags::CPU::MultiCore, double> mc_dense_matrix_reduction_to_sum_quick_test_double("MC double");
#ifdef HONEI_SSE
DenseMatrixReductionQuickTest<tags::CPU::SSE, float> sse_dense_matrix_reduction_to_sum_quick_test_float("SSE float");
DenseMatrixReductionQuickTest<tags::CPU::SSE, double> sse_dense_matrix_reduction_to_sum_quick_test_double("SSE double");
DenseMatrixReductionQuickTest<tags::CPU::MultiCore::SSE, float> sse_mc_dense_matrix_reduction_to_sum_quick_test_float("MC SSE float");
DenseMatrixReductionQuickTest<tags::CPU::MultiCore::SSE, double> sse_mc_dense_matrix_reduction_to_sum_quick_test_double("MC SSE double");
#endif
#ifdef HONEI_CELL
DenseMatrixReductionQuickTest<tags::Cell, float> cell_dense_matrix_reduction_to_sum_quick_test_float("Cell float");
DenseMatrixReductionQuickTest<tags::Cell, double> cell_dense_matrix_reduction_to_sum_quick_test_double("Cell double");
#endif

template <typename Tag_, typename DT_>
class SparseMatrixReductionToSumTest :
    public BaseTest
{
    public:
        SparseMatrixReductionToSumTest(const std::string & type) :
            BaseTest("sparse_matrix_reduction_to_sum_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            for (unsigned long size(11) ; size < (1 << 9) ; size <<= 1)
            {
                SparseMatrix<DT_> sm1(size+1, size, size / 8 + 1);
                DenseVector<DT_> dv1(size + 1, DT_(0));
                for (typename SparseMatrix<DT_>::ElementIterator i(sm1.begin_elements()),
                    i_end(sm1.end_elements()) ; i != i_end ; ++i)
                {
                    if (i.index() % 10 == 0)
                    {
                        *i = DT_(2);
                        dv1[i.row()] += DT_(2);
                    }

                    if (i.index() % 4 == 0)
                    {
                        *i = DT_(-5);
                        dv1[i.row()] += DT_(-5);
                    }

                    if ((i.index() % 10 == 0) && (i.index() % 4 == 0))
                        dv1[i.row()] -= DT_(2);

                }
                DenseVector<DT_> sum(Reduction<rt_sum, Tag_>::value(sm1));

                for (typename DenseVector<DT_>::ConstElementIterator i(sum.begin_elements()), i_end(sum.end_elements()), j(dv1.begin_elements()) ;
                    i != i_end ; ++i, ++j)
                {
                    TEST_CHECK_EQUAL_WITHIN_EPS(*i, *j, std::numeric_limits<DT_>::epsilon());
                }
            }
        }
};
SparseMatrixReductionToSumTest<tags::CPU, float> sparse_matrix_reduction_to_sum_test_float("float");
SparseMatrixReductionToSumTest<tags::CPU, double> sparse_matrix_reduction_to_sum_test_double("double");
SparseMatrixReductionToSumTest<tags::CPU::MultiCore, float> mc_sparse_matrix_reduction_to_sum_test_float("MC float");
SparseMatrixReductionToSumTest<tags::CPU::MultiCore, double> mc_sparse_matrix_reduction_to_sum_test_double("MC double");
#ifdef HONEI_SSE
SparseMatrixReductionToSumTest<tags::CPU::SSE, float> sse_sparse_matrix_reduction_to_sum_test_float("SSE float");
SparseMatrixReductionToSumTest<tags::CPU::SSE, double> sse_sparse_matrix_reduction_to_sum_test_double("SSE double");
SparseMatrixReductionToSumTest<tags::CPU::MultiCore::SSE, float> sse_mc_sparse_matrix_reduction_to_sum_test_float("MC SSE float");
SparseMatrixReductionToSumTest<tags::CPU::MultiCore::SSE, double> sse_mc_sparse_matrix_reduction_to_sum_test_double("MC SSE double");
#endif
#ifdef HONEI_CELL
//SparseMatrixReductionToSumTest<tags::Cell, float> sparse_matrix_reduction_to_sum_test_float_cell("Cell float");
//SparseMatrixReductionToSumTest<tags::Cell, double> sparse_matrix_reduction_to_sum_test_double_cell("Cell double");
#endif

template <typename Tag_, typename DT_>
class SparseMatrixReductionQuickTest :
    public QuickTest
{
    public:
        SparseMatrixReductionQuickTest(const std::string & type) :
            QuickTest("sparse_matrix_reduction_to_sum_quick_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            unsigned long size(22);
            SparseMatrix<DT_> sm1(size+1, size, size / 8 + 1);
            DenseVector<DT_> dv1(size + 1, DT_(0));
            for (typename SparseMatrix<DT_>::ElementIterator i(sm1.begin_elements()),
                i_end(sm1.end_elements()) ; i != i_end ; ++i)
            {
                if (i.index() % 10 == 0)
                {
                    *i = DT_(2);
                    dv1[i.row()] += DT_(2);
                }

                if (i.index() % 4 == 0)
                {
                    *i = DT_(-5);
                    dv1[i.row()] += DT_(-5);
                }

                if ((i.index() % 10 == 0) && (i.index() % 4 == 0))
                        dv1[i.row()] -= DT_(2);

            }
            DenseVector<DT_> sum(Reduction<rt_sum, Tag_>::value(sm1));

            for (typename DenseVector<DT_>::ConstElementIterator i(sum.begin_elements()), i_end(sum.end_elements()), j(dv1.begin_elements()) ;
                    i != i_end ; ++i, ++j)
            {
                TEST_CHECK_EQUAL_WITHIN_EPS(*i, *j, std::numeric_limits<DT_>::epsilon());
            }
        }
};
SparseMatrixReductionQuickTest<tags::CPU, float> sparse_matrix_reduction_to_sum_quick_test_float("float");
SparseMatrixReductionQuickTest<tags::CPU, double> sparse_matrix_reduction_to_sum_quick_test_double("double");
SparseMatrixReductionQuickTest<tags::CPU::MultiCore, float> mc_sparse_matrix_reduction_to_sum_quick_test_float("MC float");
SparseMatrixReductionQuickTest<tags::CPU::MultiCore, double> mc_sparse_matrix_reduction_to_sum_quick_test_double("MC double");
#ifdef HONEI_SSE
SparseMatrixReductionQuickTest<tags::CPU::SSE, float> sse_sparse_matrix_reduction_to_sum_quick_test_float("SSE float");
SparseMatrixReductionQuickTest<tags::CPU::SSE, double> sse_sparse_matrix_reduction_to_sum_quick_test_double("SSE double");
SparseMatrixReductionQuickTest<tags::CPU::MultiCore::SSE, float> sse_mc_sparse_matrix_reduction_to_sum_quick_test_float("MC SSE float");
SparseMatrixReductionQuickTest<tags::CPU::MultiCore::SSE, double> sse_mc_sparse_matrix_reduction_to_sum_quick_test_double("MC SSE double");
#endif
#ifdef HONEI_CELL
//SparseMatrixReductionQuickTest<tags::Cell, float> sparse_matrix_reduction_to_sum_quick_test_float_cell("Cell float");
//SparseMatrixReductionQuickTest<tags::Cell, double> sparse_matrix_reduction_to_sum_quick_test_double_cell("Cell double");
#endif

template <typename Tag_, typename DT_>
class DenseVectorRangeReductionToSumTest :
    public BaseTest
{
    public:
        DenseVectorRangeReductionToSumTest(const std::string & type) :
            BaseTest("dense_vector_range_reduction_to_sum_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            for (unsigned long size(1) ; size < (1 << 16) ; size <<= 1)
            {
                DenseVector<DT_> dv(size*4, DT_(100));
                for(int i(0) ; i < 4 ; i++)
                {
                    DenseVectorRange<DT_> dvr(dv, size, i);
                    for (typename DenseVectorRange<DT_>::ElementIterator i(dvr.begin_elements()), i_end(dvr.end_elements()) ;
                            i != i_end ; ++i)
                    {
                        *i = static_cast<DT_>((i.index() + 1) / 1.23456789);
                    }

                    DT_ v1(Reduction<rt_sum, Tag_>::value(dvr));
                    DT_ s1(size * (size + 1) / 2 / 1.23456789);
                    DT_ eps(exp(-20 + (log(log(size + 5)) * 13)));
                    TEST_CHECK_EQUAL_WITHIN_EPS(v1, s1, eps);
                }
            }
        }
};

DenseVectorRangeReductionToSumTest<tags::CPU, float> dense_vector_range_reduction_to_sum_test_float("float");
DenseVectorRangeReductionToSumTest<tags::CPU, double> dense_vector_range_reduction_to_sum_test_double("double");
DenseVectorRangeReductionToSumTest<tags::CPU::MultiCore, float> mc_dense_vector_range_reduction_to_sum_test_float("MC float");
DenseVectorRangeReductionToSumTest<tags::CPU::MultiCore, double> mc_dense_vector_range_reduction_to_sum_test_double("MC double");

#ifdef HONEI_SSE
DenseVectorRangeReductionToSumTest<tags::CPU::SSE, float> sse_dense_vector_range_reduction_to_sum_test_float("SSE float");
DenseVectorRangeReductionToSumTest<tags::CPU::SSE, double> sse_dense_vector_range_reduction_to_sum_test_double("SSE double");
DenseVectorRangeReductionToSumTest<tags::CPU::MultiCore::SSE, float> sse_mc_dense_vector_range_reduction_to_sum_test_float("MC SSE float");
DenseVectorRangeReductionToSumTest<tags::CPU::MultiCore::SSE, double> sse_mc_dense_vector_range_reduction_to_sum_test_double("MC SSE double");
#endif

#ifdef HONEI_CELL
DenseVectorRangeReductionToSumTest<tags::Cell, float> dense_vector_range_reduction_to_sum_test_float_cell("Cell float");
DenseVectorRangeReductionToSumTest<tags::Cell, double> dense_vector_range_reduction_to_sum_test_double_cell("Cell double");
#endif

template <typename Tag_, typename DT_>
class DenseVectorRangeReductionToSumQuickTest :
    public QuickTest
{
    public:
        DenseVectorRangeReductionToSumQuickTest(const std::string & type) :
            QuickTest("dense_vector_range_reduction_to_sum_quick_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            unsigned long size(19);
            DenseVector<DT_> dv(size*4, DT_(100));
            for (int i(0) ; i < 4 ; i++)
            {
                DenseVectorRange<DT_> dvr(dv, size, i);
                for (typename DenseVectorRange<DT_>::ElementIterator i(dvr.begin_elements()), i_end(dvr.end_elements()) ;
                        i != i_end ; ++i)
                {
                    *i = static_cast<DT_>((i.index() + 1) / 1.23456789);
                }

                DT_ v1(Reduction<rt_sum, Tag_>::value(dvr));
                DT_ s1(size * (size + 1) / 2 / 1.23456789);
                //DT_ eps(exp(-19.72 + log(size) * 2.92257));
                DT_ eps(exp(-20 + log(size) * 4));
                TEST_CHECK_EQUAL_WITHIN_EPS(v1, s1, eps);
            }
        }
};
DenseVectorRangeReductionToSumQuickTest<tags::CPU, float>  dense_vector_range_reduction_to_sum_quick_test_float("float");
DenseVectorRangeReductionToSumQuickTest<tags::CPU, double> dense_vector_range_reduction_to_sum_quick_test_double("double");
DenseVectorRangeReductionToSumQuickTest<tags::CPU::MultiCore, float>  mc_dense_vector_range_reduction_to_sum_quick_test_float("MC float");
DenseVectorRangeReductionToSumQuickTest<tags::CPU::MultiCore, double> mc_dense_vector_range_reduction_to_sum_quick_test_double("MC double");

#ifdef HONEI_SSE
DenseVectorRangeReductionToSumQuickTest<tags::CPU::SSE, float>  sse_dense_vector_range_reduction_to_sum_quick_test_float("SSE float");
DenseVectorRangeReductionToSumQuickTest<tags::CPU::SSE, double> sse_dense_vector_range_reduction_to_sum_quick_test_double("SSE double");
DenseVectorRangeReductionToSumQuickTest<tags::CPU::MultiCore::SSE, float>  sse_mc_dense_vector_range_reduction_to_sum_quick_test_float("MC SSE float");
DenseVectorRangeReductionToSumQuickTest<tags::CPU::MultiCore::SSE, double> sse_mc_dense_vector_range_reduction_to_sum_quick_test_double("MC SSE double");
#endif

#ifdef HONEI_CELL
DenseVectorRangeReductionToSumQuickTest<tags::Cell, float> dense_vector_range_reduction_to_sum_quick_test_float_cell("Cell float");
DenseVectorRangeReductionToSumQuickTest<tags::Cell, double> dense_vector_range_reduction_to_sum_quick_test_double_cell("Cell double");
#endif


template <typename Tag_, typename DT_>
class DenseVectorReductionToSumTest :
    public BaseTest
{
    public:
        DenseVectorReductionToSumTest(const std::string & type) :
            BaseTest("dense_vector_reduction_to_sum_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            for (unsigned long size(1) ; size < (1 << 16) ; size <<= 1)
            {
                DenseVector<DT_> dv(size);
                for (typename DenseVector<DT_>::ElementIterator i(dv.begin_elements()), i_end(dv.end_elements()) ;
                        i != i_end ; ++i)
                {
                    *i = static_cast<DT_>((i.index() + 1) / 1.23456789);
                }

                DT_ v1(Reduction<rt_sum, Tag_>::value(dv));
                DT_ s1(size * (size + 1) / 2 / 1.23456789);
                DT_ eps(exp(-20 + (log(log(size + 5)) * 13)));
                TEST_CHECK_EQUAL_WITHIN_EPS(v1, s1, eps);
            }
        }
};

DenseVectorReductionToSumTest<tags::CPU, float> dense_vector_reduction_to_sum_test_float("float");
DenseVectorReductionToSumTest<tags::CPU, double> dense_vector_reduction_to_sum_test_double("double");
DenseVectorReductionToSumTest<tags::CPU::MultiCore, float> mc_dense_vector_reduction_to_sum_test_float("MC float");
DenseVectorReductionToSumTest<tags::CPU::MultiCore, double> mc_dense_vector_reduction_to_sum_test_double("MC double");
#ifdef HONEI_SSE
DenseVectorReductionToSumTest<tags::CPU::SSE, float> sse_dense_vector_reduction_to_sum_test_float("SSE float");
DenseVectorReductionToSumTest<tags::CPU::SSE, double> sse_dense_vector_reduction_to_sum_test_double("SSE double");
DenseVectorReductionToSumTest<tags::CPU::MultiCore::SSE, float> sse_mc_dense_vector_reduction_to_sum_test_float("MC SSE float");
DenseVectorReductionToSumTest<tags::CPU::MultiCore::SSE, double> sse_mc_dense_vector_reduction_to_sum_test_double("MC SSE double");
#endif
#ifdef HONEI_CELL
DenseVectorReductionToSumTest<tags::Cell, float> dense_vector_reduction_to_sum_test_float_cell("Cell float");
DenseVectorReductionToSumTest<tags::Cell, double> dense_vector_reduction_to_sum_test_double_cell("Cell double");
#endif

template <typename Tag_, typename DT_>
class DenseVectorReductionToSumQuickTest :
    public QuickTest
{
    public:
        DenseVectorReductionToSumQuickTest(const std::string & type) :
            QuickTest("dense_vector_reduction_to_sum_quick_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            unsigned long size(22);
            DenseVector<DT_> dv(size);
            for (typename DenseVector<DT_>::ElementIterator i(dv.begin_elements()), i_end(dv.end_elements()) ;
                    i != i_end ; ++i)
            {
                *i = static_cast<DT_>((i.index() + 1) / 1.23456789);
            }

            DT_ v1(Reduction<rt_sum, Tag_>::value(dv));
            DT_ s1(size * (size + 1) / 2 / 1.23456789);
            //DT_ eps(exp(-19.72 + log(size) * 2.92257));
            DT_ eps(exp(-20 + log(size) * 4));
            TEST_CHECK_EQUAL_WITHIN_EPS(v1, s1, eps);
        }
};
DenseVectorReductionToSumQuickTest<tags::CPU, float>  dense_vector_reduction_to_sum_quick_test_float("float");
DenseVectorReductionToSumQuickTest<tags::CPU, double> dense_vector_reduction_to_sum_quick_test_double("double");
DenseVectorReductionToSumQuickTest<tags::CPU::MultiCore, float>  mc_dense_vector_reduction_to_sum_quick_test_float("MC float");
DenseVectorReductionToSumQuickTest<tags::CPU::MultiCore, double> mc_dense_vector_reduction_to_sum_quick_test_double("MC double");
#ifdef HONEI_SSE
DenseVectorReductionToSumQuickTest<tags::CPU::SSE, float>  sse_dense_vector_reduction_to_sum_quick_test_float("SSE float");
DenseVectorReductionToSumQuickTest<tags::CPU::SSE, double> sse_dense_vector_reduction_to_sum_quick_test_double("SSE double");
DenseVectorReductionToSumQuickTest<tags::CPU::MultiCore::SSE, float>  sse_mc_dense_vector_reduction_to_sum_quick_test_float("MC SSE float");
DenseVectorReductionToSumQuickTest<tags::CPU::MultiCore::SSE, double> sse_mc_dense_vector_reduction_to_sum_quick_test_double("MC SSE double");
#endif
#ifdef HONEI_CELL
DenseVectorReductionToSumQuickTest<tags::Cell, float> dense_vector_reduction_to_sum_quick_test_float_cell("Cell float");
DenseVectorReductionToSumQuickTest<tags::Cell, double> dense_vector_reduction_to_sum_quick_test_double_cell("Cell double");
#endif


template <typename Tag_, typename DT_>
class SparseVectorReductionToSumTest :
    public BaseTest
{
    public:
        SparseVectorReductionToSumTest(const std::string & type) :
            BaseTest("sparse_vector_reduction_to_sum_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            for (unsigned long size(1) ; size < (1 << 10) ; size <<= 1)
            {
                DT_ s1(0);
                SparseVector<DT_> sv1(size, size / 8 + 1);
                for (typename SparseVector<DT_>::ElementIterator i(sv1.begin_elements()), i_end(sv1.end_elements()) ;
                        i != i_end ; ++i)
                {
                    if (i.index() % 10 == 0)
                    {
                        *i = static_cast<DT_>((i.index() +1) / 1.23456789);
                        s1 += *i;
                    }
                }

                DT_ v1(Reduction<rt_sum, Tag_>::value(sv1));
                DT_ eps1(exp(-20 + (log(log(sv1.used_elements() + 5)) * 13)));
                TEST_CHECK_EQUAL_WITHIN_EPS(v1, s1, eps1);
            }
        }
};

SparseVectorReductionToSumTest<tags::CPU, float> sparse_vector_reduction_to_sum_test_float("float");
SparseVectorReductionToSumTest<tags::CPU, double> sparse_vector_reduction_to_sum_test_double("double");
SparseVectorReductionToSumTest<tags::CPU::MultiCore, float> mc_sparse_vector_reduction_to_sum_test_float("MC float");
SparseVectorReductionToSumTest<tags::CPU::MultiCore, double> mc_sparse_vector_reduction_to_sum_test_double("MC double");
#ifdef HONEI_SSE
SparseVectorReductionToSumTest<tags::CPU::SSE, float> sse_sparse_vector_reduction_to_sum_test_float("SSE float");
SparseVectorReductionToSumTest<tags::CPU::SSE, double> sse_sparse_vector_reduction_to_sum_test_double("SSE double");
SparseVectorReductionToSumTest<tags::CPU::MultiCore::SSE, float> sse_mc_sparse_vector_reduction_to_sum_test_float("MC SSE float");
SparseVectorReductionToSumTest<tags::CPU::MultiCore::SSE, double> sse_mc_sparse_vector_reduction_to_sum_test_double("MC SSE double");
#endif
#ifdef HONEI_CELL
SparseVectorReductionToSumTest<tags::Cell, float> sparse_vector_reduction_to_sum_test_float_cell("Cell float");
SparseVectorReductionToSumTest<tags::Cell, double> sparse_vector_reduction_to_sum_test_double_cell("Cell double");
#endif

template <typename Tag_, typename DT_>
class SparseVectorReductionToSumQuickTest :
    public QuickTest
{
    public:
        SparseVectorReductionToSumQuickTest(const std::string & type) :
            QuickTest("sparse_vector_reduction_to_sum_quick_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            unsigned long size(22);
            DT_ s1(0);
            SparseVector<DT_> sv1(size, size / 8 + 1);
            for (typename SparseVector<DT_>::ElementIterator i(sv1.begin_elements()), i_end(sv1.end_elements()) ;
                    i != i_end ; ++i)
            {
                if (i.index() % 10 == 0)
                {
                    *i = static_cast<DT_>((i.index() +1) / 1.23456789);
                    s1 += *i;
                }
            }

            DT_ v1(Reduction<rt_sum, Tag_>::value(sv1));
            DT_ eps1(s1 * 10 * std::numeric_limits<DT_>::epsilon());
            TEST_CHECK_EQUAL_WITHIN_EPS(v1, s1, eps1);
        }
};

SparseVectorReductionToSumQuickTest<tags::CPU, float> sparse_reduction_to_sum_quick_test_float("float");
SparseVectorReductionToSumQuickTest<tags::CPU, double> sparse_reduction_to_sum_quick_test_double("double");
SparseVectorReductionToSumQuickTest<tags::CPU::MultiCore, float> mc_sparse_reduction_to_sum_quick_test_float("MC float");
SparseVectorReductionToSumQuickTest<tags::CPU::MultiCore, double> mc_sparse_reduction_to_sum_quick_test_double("MC double");
#ifdef HONEI_SSE
SparseVectorReductionToSumQuickTest<tags::CPU::SSE, float> sse_sparse_reduction_to_sum_quick_test_float("SSE float");
SparseVectorReductionToSumQuickTest<tags::CPU::SSE, double> sse_sparse_reduction_to_sum_quick_test_double("SSE double");
SparseVectorReductionToSumQuickTest<tags::CPU::MultiCore::SSE, float> sse_mc_sparse_reduction_to_sum_quick_test_float("MC SSE float");
SparseVectorReductionToSumQuickTest<tags::CPU::MultiCore::SSE, double> sse_mc_sparse_reduction_to_sum_quick_test_double("MC SSE double");
#endif
#ifdef HONEI_CELL
SparseVectorReductionToSumQuickTest<tags::Cell, float> sparse_vector_reduction_to_sum_quick_test_float_cell("Cell float");
SparseVectorReductionToSumQuickTest<tags::Cell, double> sparse_vector_reduction_to_sum_quick_test_double_cell("Cell double");
#endif

template <typename Tag_, typename DT_>
class DenseMatrixReductionToMinTest :
    public BaseTest
{
    public:
        DenseMatrixReductionToMinTest(const std::string & type) :
            BaseTest("dense_matrix_reduction_to_min_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            for (unsigned long size(1) ; size < (1 << 10) ; size <<= 1)
            {
                DenseMatrix<DT_> dm1(size, size);
                for (typename DenseMatrix<DT_>::ElementIterator i(dm1.begin_elements()), i_end(dm1.end_elements()) ;
                        i != i_end ; ++i)
                {
                    if (i.index() % 2 == 0)
                        *i = i.index();
                    else
                        *i = i.index() * DT_(-1);
                }

                DT_ should;
                if ((size % 2 == 0) || (size == 1))
                    should = dm1[size-1][size-1];
                else
                    should = dm1[size-1][size-2];

            DenseVector<DT_> v1(Reduction<rt_min, Tag_>::value(dm1));
            TEST_CHECK_EQUAL_WITHIN_EPS(v1[size-1], should, std::numeric_limits<DT_>::epsilon());
            }
        }
};

DenseMatrixReductionToMinTest<tags::CPU, float> dense_matrix_reduction_to_min_test_float("float");
DenseMatrixReductionToMinTest<tags::CPU, double> dense_matrix_reduction_to_min_test_double("double");
DenseMatrixReductionToMinTest<tags::CPU::MultiCore, float> mc_dense_matrix_reduction_to_min_test_float("MC float");
DenseMatrixReductionToMinTest<tags::CPU::MultiCore, double> mc_dense_matrix_reduction_to_min_test_double("MC double");

#ifdef HONEI_CELL
DenseMatrixReductionToMinTest<tags::Cell, float> cell_dense_matrix_reduction_to_min_test_float("Cell float");
#endif

template <typename Tag_, typename DT_>
class DenseMatrixReductionToMinQuickTest :
    public QuickTest
{
    public:
        DenseMatrixReductionToMinQuickTest(const std::string & type) :
            QuickTest("dense_matrix_reduction_to_min_quick_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            unsigned long size(23);
            DenseMatrix<DT_> dm1(size, size);
            for (typename DenseMatrix<DT_>::ElementIterator i(dm1.begin_elements()), i_end(dm1.end_elements()) ;
                    i != i_end ; ++i)
                {
                    if (i.index() % 2 == 0)
                        *i = i.index();
                    else
                        *i = i.index() * DT_(-1);
                }

                DT_ should;
                if ((size % 2 == 0) || (size == 1))
                    should = dm1[size-1][size-1];
                else
                    should = dm1[size-1][size-2];

            DenseVector<DT_> v1(Reduction<rt_min, Tag_>::value(dm1));
            TEST_CHECK_EQUAL_WITHIN_EPS(v1[size-1], should, std::numeric_limits<DT_>::epsilon());
        }
};

DenseMatrixReductionToMinQuickTest<tags::CPU, float> dense_matrix_reduction_to_min_quick_test_float("float");
DenseMatrixReductionToMinQuickTest<tags::CPU, double> dense_matrix_reduction_to_min_quick_test_double("double");
DenseMatrixReductionToMinQuickTest<tags::CPU::MultiCore, float> mc_dense_matrix_reduction_to_min_quick_test_float("MC float");
DenseMatrixReductionToMinQuickTest<tags::CPU::MultiCore, double> mc_dense_matrix_reduction_to_min_quick_test_double("MC double");

#ifdef HONEI_CELL
DenseMatrixReductionToMinQuickTest<tags::Cell, float> cell_dense_matrix_reduction_to_min_quick_test_float("Cell float");
#endif

template <typename Tag_, typename DT_>
class SparseMatrixReductionToMinTest :
    public BaseTest
{
    public:
        SparseMatrixReductionToMinTest(const std::string & type) :
            BaseTest("sparse_matrix_reduction_to_min_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            for (unsigned long size(1) ; size < (1 << 10) ; size <<= 1)
            {
                SparseMatrix<DT_> sm1(size, size);
                for (typename SparseMatrix<DT_>::ElementIterator i(sm1.begin_elements()), i_end(sm1.end_elements()) ;
                        i != i_end ; ++i)
            {
                if (i.index() % 2 == 0)
                    *i = i.index();
                else
                    *i = i.index() * DT_(-1);
            }

            DT_ should;
            if (size % 2 == 0 || size == 1)
                should = sm1[size-1][size-1];
            else
                should = sm1[size-1][size-2];

                DenseVector<DT_> v1(Reduction<rt_min, Tag_>::value(sm1));
                TEST_CHECK_EQUAL_WITHIN_EPS(v1[size-1], should, std::numeric_limits<DT_>::epsilon());
            }
        }
};
SparseMatrixReductionToMinTest<tags::CPU, float> sparse_matrix_reduction_to_min_test_float("float");
SparseMatrixReductionToMinTest<tags::CPU, double> sparse_matrix_reduction_to_min_test_double("double");
SparseMatrixReductionToMinTest<tags::CPU::MultiCore, float> mc_sparse_matrix_reduction_to_min_test_float("MC float");
SparseMatrixReductionToMinTest<tags::CPU::MultiCore, double> mc_sparse_matrix_reduction_to_min_test_double("MC double");
#ifdef HONEI_CELL
//SparseMatrixReductionToMinTest<tags::Cell, float> sparse_matrix_reduction_to_min_test_float_cell("Cell float");
#endif

template <typename Tag_, typename DT_>
class SparseMatrixReductionToMinQuickTest :
    public QuickTest
{
    public:
        SparseMatrixReductionToMinQuickTest(const std::string & type) :
            QuickTest("sparse_matrix_reduction_to_min_quick_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            unsigned long size(22);
            SparseMatrix<DT_> sm1(size, size);
            for (typename SparseMatrix<DT_>::ElementIterator i(sm1.begin_elements()), i_end(sm1.end_elements()) ;
                    i != i_end ; ++i)
            {
                if (i.index() % 2 == 0)
                    *i = i.index();
                else
                    *i = i.index() * DT_(-1);
            }

            DT_ should;
            if (size % 2 == 0 || size == 1)
                should = sm1[size-1][size-1];
            else
                should = sm1[size-1][size-2];

            DenseVector<DT_> v1(Reduction<rt_min, Tag_>::value(sm1));
            TEST_CHECK_EQUAL_WITHIN_EPS(v1[size-1], should, std::numeric_limits<DT_>::epsilon());
        }
};
SparseMatrixReductionToMinQuickTest<tags::CPU, float> sparse_matrix_reduction_to_min_quick_test_float("float");
SparseMatrixReductionToMinQuickTest<tags::CPU, double> sparse_matrix_reduction_to_min_quick_test_double("double");
SparseMatrixReductionToMinQuickTest<tags::CPU::MultiCore, float> mc_sparse_matrix_reduction_to_min_quick_test_float("MC float");
SparseMatrixReductionToMinQuickTest<tags::CPU::MultiCore, double> mc_sparse_matrix_reduction_to_min_quick_test_double("MC double");
#ifdef HONEI_CELL
//SparseMatrixReductionToMinQuickTest<tags::Cell, float> sparse_matrix_reduction_to_min_quick_test_float_cell("Cell float");
#endif

template <typename Tag_, typename DT_>
class BandedMatrixReductionToMinTest :
    public BaseTest
{
    public:
        BandedMatrixReductionToMinTest(const std::string & type) :
            BaseTest("banded_matrix_reduction_to_min_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {

            for (unsigned long size(4) ; size < (1 << 10) ; size <<= 1)
            {
                DenseVector<DT_>  result(size);
                typename DenseVector<DT_>::ElementIterator resultIter(result.begin_elements());
                BandedMatrix<DT_> bm1(size);
                DT_ rand(0);
                for (typename BandedMatrix<DT_>::ElementIterator i(bm1.begin_elements()), i_end(bm1.end_elements()) ;
                        i != i_end ; ++i)
                {
                    rand = DT_(std::rand()) / DT_(std::rand()-RAND_MAX / 2);
                    *i = rand;

                    if (i.index() == 0) 
                    {
                       *resultIter = rand;
                    }

                    if ((i.index() != 0) && ((i.index() % size) == 0))
                    {
                        ++resultIter;
                        *resultIter = rand;
                    }

                    if  (rand < *resultIter)
                    {
                        *resultIter = rand;
                    }
                }

                DenseVector<DT_> v1(Reduction<rt_min, Tag_>::value(bm1));
               
                for (typename DenseVector<DT_>::ElementIterator i(v1.begin_elements()), i_end(v1.end_elements()), r(result.begin_elements()) ; 
                    i != i_end ; )
                {
                    TEST_CHECK_EQUAL_WITHIN_EPS(*i, *r, std::numeric_limits<DT_>::epsilon());
                    ++r;
                    ++i;
                }
            }
        }
};

BandedMatrixReductionToMinTest<tags::CPU, float> banded_matrix_reduction_to_min_test_float("float");
BandedMatrixReductionToMinTest<tags::CPU, double> banded_matrix_reduction_to_min_test_double("double");
BandedMatrixReductionToMinTest<tags::CPU::MultiCore, float> mc_banded_matrix_reduction_to_min_test_float("MC float");
BandedMatrixReductionToMinTest<tags::CPU::MultiCore, double> mc_banded_matrix_reduction_to_min_test_double("MC double");

template <typename Tag_, typename DT_>
class BandedMatrixReductionToMinQuickTest :
    public QuickTest
{
    public:
        BandedMatrixReductionToMinQuickTest(const std::string & type) :
            QuickTest("banded_matrix_reduction_to_min_quick_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            unsigned long size(22);
            DenseVector<DT_>  result(size);
            typename DenseVector<DT_>::ElementIterator resultIter(result.begin_elements());
            BandedMatrix<DT_> bm1(size);
            DT_ rand(0);
            for (typename BandedMatrix<DT_>::ElementIterator i(bm1.begin_elements()), i_end(bm1.end_elements()) ;
                i != i_end ; ++i)
            {
                rand = DT_(std::rand()) / DT_(std::rand()-RAND_MAX / 2);
                *i = rand;

                if (i.index() == 0) 
                {
                   *resultIter = rand;
                }

                if ((i.index() != 0) && ((i.index() % size) == 0))
                {
                    ++resultIter;
                    *resultIter = rand;
                }

                if  (rand < *resultIter)
                {
                    *resultIter = rand;
                }
            }

            DenseVector<DT_> v1(Reduction<rt_min, Tag_>::value(bm1));
               
            for (typename DenseVector<DT_>::ElementIterator i(v1.begin_elements()), i_end(v1.end_elements()), r(result.begin_elements()) ; 
                i != i_end ; )
            {
                TEST_CHECK_EQUAL_WITHIN_EPS(*i, *r, std::numeric_limits<DT_>::epsilon());
                ++r;
                ++i;
            }
        }
    
};
BandedMatrixReductionToMinQuickTest<tags::CPU, float> banded_matrix_reduction_to_min_quick_test_float("float");
BandedMatrixReductionToMinQuickTest<tags::CPU, double> banded_matrix_reduction_to_min_quick_test_double("double");
BandedMatrixReductionToMinQuickTest<tags::CPU::MultiCore, float> mc_banded_matrix_reduction_to_min_quick_test_float("MC float");
BandedMatrixReductionToMinQuickTest<tags::CPU::MultiCore, double> mc_banded_matrix_reduction_to_min_quick_test_double("MC double");

template <typename Tag_, typename DT_>
class DenseVectorRangeReductionToMinTest :
    public BaseTest
{
    public:
        DenseVectorRangeReductionToMinTest(const std::string & type) :
            BaseTest("dense_vector_range_reduction_to_min_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            for (unsigned long size(1) ; size < (1 << 16) ; size <<= 1)
            {
                DenseVector<DT_> dv(size*4, DT_(100));
                for(int i(0) ; i < 4 ; i++)
                {
                    DenseVectorRange<DT_> dvr(dv, size, i);
                    for (typename DenseVectorRange<DT_>::ElementIterator i(dvr.begin_elements()), i_end(dvr.end_elements()) ;
                            i != i_end ; ++i)
                    {

                        if (i.index() % 2 == 0)
                            *i = i.index();
                        else
                            *i = i.index() * DT_(-1);
                    }

                    DT_ should;
                    if ((size % 2 == 0) || (size == 1))
                        should = dvr[size-1];
                    else
                        should = dvr[size-2];

                    DT_ v1(Reduction<rt_min, Tag_>::value(dvr));
                    TEST_CHECK_EQUAL_WITHIN_EPS(v1, should, std::numeric_limits<DT_>::epsilon());
                }
            }
        }
};

DenseVectorRangeReductionToMinTest<tags::CPU, float> dense_vector_range_reduction_to_min_test_float("float");
DenseVectorRangeReductionToMinTest<tags::CPU, double> dense_vector_range_reduction_to_min_test_double("double");
DenseVectorRangeReductionToMinTest<tags::CPU::MultiCore, float> mc_dense_vector_range_reduction_to_min_test_float("MC float");
DenseVectorRangeReductionToMinTest<tags::CPU::MultiCore, double> mc_dense_vector_range_reduction_to_min_test_double("MC double");
/*
#ifdef HONEI_SSE
DenseVectorRangeReductionToMinTest<tags::CPU::SSE, float> sse_dense_vector_range_reduction_to_min_test_float("SSE float");
DenseVectorRangeReductionToMinTest<tags::CPU::SSE, double> sse_dense_vector_range_reduction_to_min_test_double("SSE double");
DenseVectorRangeReductionToMinTest<tags::CPU::MultiCore::SSE, float> sse_mc_dense_vector_range_reduction_to_min_test_float("MC SSE float");
DenseVectorRangeReductionToMinTest<tags::CPU::MultiCore::SSE, double> sse_mc_dense_vector_range_reduction_to_min_test_double("MC SSE double");
#endif
*/
#ifdef HONEI_CELL
DenseVectorRangeReductionToMinTest<tags::Cell, float> dense_vector_range_reduction_to_min_test_float_cell("Cell float");
#endif

template <typename Tag_, typename DT_>
class DenseVectorRangeReductionToMinQuickTest :
    public QuickTest
{
    public:
        DenseVectorRangeReductionToMinQuickTest(const std::string & type) :
            QuickTest("dense_vector_range_reduction_to_min_quick_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            unsigned long size(22);
            DenseVector<DT_> dv(size*4, DT_(100));
            for(int i(0) ; i < 4 ; i++)
            {
                DenseVectorRange<DT_> dvr(dv, size, i);
                for (typename DenseVectorRange<DT_>::ElementIterator i(dvr.begin_elements()), i_end(dvr.end_elements()) ;
                        i != i_end ; ++i)
                {

                    if (i.index() % 2 == 0)
                        *i = i.index();
                    else
                        *i = i.index() * DT_(-1);
                }

                DT_ should;
                if ((size % 2 == 0) || (size == 1))
                    should = dvr[size-1];
                else
                    should = dvr[size-2];

                DT_ v1(Reduction<rt_min, Tag_>::value(dvr));
                TEST_CHECK_EQUAL_WITHIN_EPS(v1, should, std::numeric_limits<DT_>::epsilon());
            }
        }
};
DenseVectorRangeReductionToMinQuickTest<tags::CPU, float>  dense_vector_range_reduction_to_min_quick_test_float("float");
DenseVectorRangeReductionToMinQuickTest<tags::CPU, double> dense_vector_range_reduction_to_min_quick_test_double("double");
DenseVectorRangeReductionToMinQuickTest<tags::CPU::MultiCore, float>  mc_dense_vector_range_reduction_to_min_quick_test_float("MC float");
DenseVectorRangeReductionToMinQuickTest<tags::CPU::MultiCore, double> mc_dense_vector_range_reduction_to_min_quick_test_double("MC double");
/*
#ifdef HONEI_SSE
DenseVectorRangeReductionToMinQuickTest<tags::CPU::SSE, float>  sse_dense_vector_range_reduction_to_min_quick_test_float("SSE float");
DenseVectorRangeReductionToMinQuickTest<tags::CPU::SSE, double> sse_dense_vector_range_reduction_to_min_quick_test_double("SSE double");
DenseVectorRangeReductionToMinQuickTest<tags::CPU::MultiCore::SSE, float>  sse_mc_dense_vector_range_reduction_to_min_quick_test_float("MC SSE float");
DenseVectorRangeReductionToMinQuickTest<tags::CPU::MultiCore::SSE, double> sse_mc_dense_vector_range_reduction_to_min_quick_test_double("MC SSE double");
#endif
*/
#ifdef HONEI_CELL
DenseVectorRangeReductionToMinQuickTest<tags::Cell, float> dense_vector_range_reduction_to_min_quick_test_float_cell("Cell float");
#endif

template <typename Tag_, typename DT_>
class DenseVectorReductionToMinTest :
    public BaseTest
{
    public:
        DenseVectorReductionToMinTest(const std::string & type) :
            BaseTest("dense_vector_reduction_to_min_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            for (unsigned long size(1) ; size < (1 << 10) ; size <<= 1)
            {
                DenseVector<DT_> dv1(size);
                for (typename DenseVector<DT_>::ElementIterator i(dv1.begin_elements()), i_end(dv1.end_elements()) ;
                        i != i_end ; ++i)
                {
                    *i = i.index() + 500;
                }

                DT_ v1(Reduction<rt_min, Tag_>::value(dv1));
                TEST_CHECK_EQUAL_WITHIN_EPS(v1, 500, std::numeric_limits<DT_>::epsilon());
            }
        }
};

DenseVectorReductionToMinTest<tags::CPU, float> dense_vector_reduction_to_min_test_float("float");
DenseVectorReductionToMinTest<tags::CPU, double> dense_vector_reduction_to_min_test_double("double");
DenseVectorReductionToMinTest<tags::CPU::MultiCore, float> mc_dense_vector_reduction_to_min_test_float("MC float");
DenseVectorReductionToMinTest<tags::CPU::MultiCore, double> mc_dense_vector_reduction_to_min_test_double("MC double");
#ifdef HONEI_CELL
DenseVectorReductionToMinTest<tags::Cell, float> dense_vector_reduction_to_min_test_float_cell("Cell float");
#endif


template <typename Tag_, typename DT_>
class DenseVectorReductionToMinQuickTest :
    public QuickTest
{
    public:
        DenseVectorReductionToMinQuickTest(const std::string & type) :
            QuickTest("dense_vector_reduction_to_min_quick_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            unsigned long size(22);
            DenseVector<DT_> dv1(size);
            for (typename DenseVector<DT_>::ElementIterator i(dv1.begin_elements()), i_end(dv1.end_elements()) ;
                    i != i_end ; ++i)
            {
                *i = i.index() + 500;
            }

            DT_ v1(Reduction<rt_min, Tag_>::value(dv1));
            TEST_CHECK_EQUAL_WITHIN_EPS(v1, 500, std::numeric_limits<DT_>::epsilon());
        }
};

DenseVectorReductionToMinQuickTest<tags::CPU, float> dense_reduction_to_min_quick_test_float("float");
DenseVectorReductionToMinQuickTest<tags::CPU, double> dense_reduction_to_min_quick_test_double("double");
DenseVectorReductionToMinQuickTest<tags::CPU::MultiCore, float> mc_dense_reduction_to_min_quick_test_float("MC float");
DenseVectorReductionToMinQuickTest<tags::CPU::MultiCore, double> mc_dense_reduction_to_min_quick_test_double("MC double");
#ifdef HONEI_CELL
DenseVectorReductionToMinQuickTest<tags::Cell, float> dense_vector_reduction_to_min_quick_test_float_cell("Cell float");
#endif

template <typename Tag_, typename DT_>
class SparseVectorReductionToMinTest :
    public BaseTest
{
    public:
        SparseVectorReductionToMinTest(const std::string & type) :
            BaseTest("sparse_vector_reduction_to_min_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            for (unsigned long size(1) ; size < (1 << 10) ; size <<= 1)
            {
                SparseVector<DT_> sv1(size, size / 8 + 1);
                for (typename SparseVector<DT_>::ElementIterator i(sv1.begin_elements()), i_end(sv1.end_elements()) ;
                        i != i_end ; ++i)
                {
                    *i = i.index();
                }

                DT_ v1(Reduction<rt_min, Tag_>::value(sv1));
                TEST_CHECK_EQUAL_WITHIN_EPS(v1, 0, std::numeric_limits<DT_>::epsilon());
            }
        }
};

SparseVectorReductionToMinTest<tags::CPU, float> sparse_vector_reduction_to_min_test_float("float");
SparseVectorReductionToMinTest<tags::CPU, double> sparse_vector_reduction_to_min_test_double("double");
SparseVectorReductionToMinTest<tags::CPU::MultiCore, float> mc_sparse_vector_reduction_to_min_test_float("MC float");
SparseVectorReductionToMinTest<tags::CPU::MultiCore, double> mc_sparse_vector_reduction_to_min_test_double("MC double");
#ifdef HONEI_CELL
SparseVectorReductionToMinTest<tags::Cell, float> sparse_vector_reduction_to_min_test_float_cell("Cell float");
#endif

template <typename Tag_, typename DT_>
class SparseVectorReductionToMinQuickTest :
    public QuickTest
{
    public:
        SparseVectorReductionToMinQuickTest(const std::string & type) :
            QuickTest("sparse_vector_reduction_to_min_quick_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            unsigned long size(22);
            SparseVector<DT_> sv1(size, size / 8 + 1);
            for (typename SparseVector<DT_>::ElementIterator i(sv1.begin_elements()), i_end(sv1.end_elements()) ;
                    i != i_end ; ++i)
            {
                *i = i.index();
            }

            DT_ v1(Reduction<rt_min, Tag_>::value(sv1));
            TEST_CHECK_EQUAL_WITHIN_EPS(v1, 0, std::numeric_limits<DT_>::epsilon());
        }
};

SparseVectorReductionToMinQuickTest<tags::CPU, float> sparse_reduction_to_min_quick_test_float("float");
SparseVectorReductionToMinQuickTest<tags::CPU, double> sparse_reduction_to_min_quick_test_double("double");
SparseVectorReductionToMinQuickTest<tags::CPU::MultiCore, float> mc_sparse_reduction_to_min_quick_test_float("MC float");
SparseVectorReductionToMinQuickTest<tags::CPU::MultiCore, double> mc_sparse_reduction_to_min_quick_test_double("MC double");
#ifdef HONEI_CELL
SparseVectorReductionToMinQuickTest<tags::Cell, float> sparse_vector_reduction_to_min_quick_test_float_cell("Cell float");
#endif

template <typename Tag_, typename DT_>
class DenseMatrixReductionToMaxTest :
    public BaseTest
{
    public:
        DenseMatrixReductionToMaxTest(const std::string & type) :
            BaseTest("dense_matrix_reduction_to_max_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            for (unsigned long size(10) ; size < (1 << 10) ; size <<= 1)
            {
                DenseMatrix<DT_> dm1(size, size);
                for (typename DenseMatrix<DT_>::ElementIterator i(dm1.begin_elements()), i_end(dm1.end_elements()) ;
                        i != i_end ; ++i)
            {
                if (i.index() % 2 == 0)
                    *i = i.index();
                else
                    *i = i.index() * DT_(-1);
            }

            DT_ should;
            if (size % 2 == 0)
                should = dm1[size-1][size-2];
            else
                should = dm1[size-1][size-1];

            DenseVector<DT_> v1(Reduction<rt_max, Tag_>::value(dm1));
            TEST_CHECK_EQUAL_WITHIN_EPS(v1[size-1], should, std::numeric_limits<DT_>::epsilon());
            }
        }
};

DenseMatrixReductionToMaxTest<tags::CPU, float> dense_matrix_reduction_to_max_test_float("float");
DenseMatrixReductionToMaxTest<tags::CPU, double> dense_matrix_reduction_to_max_test_double("double");
DenseMatrixReductionToMaxTest<tags::CPU::MultiCore, float> mc_dense_matrix_reduction_to_max_test_float("MC float");
DenseMatrixReductionToMaxTest<tags::CPU::MultiCore, double> mc_dense_matrix_reduction_to_max_test_double("MC double");

#ifdef HONEI_CELL
DenseMatrixReductionToMaxTest<tags::Cell, float> cell_dense_matrix_reduction_to_max_test_float("Cell float");
#endif

template <typename Tag_, typename DT_>
class DenseMatrixReductionToMaxQuickTest :
    public QuickTest
{
    public:
        DenseMatrixReductionToMaxQuickTest(const std::string & type) :
            QuickTest("dense_matrix_reduction_to_max_quick_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            unsigned long size(23);
            DenseMatrix<DT_> dm1(size, size);
            for (typename DenseMatrix<DT_>::ElementIterator i(dm1.begin_elements()), i_end(dm1.end_elements()) ;
                    i != i_end ; ++i)
            {
                if (i.index() % 2 == 0)
                    *i = i.index();
                else
                    *i = i.index() * DT_(-1);
            }

            DT_ should;
            if (size % 2 == 0)
                should = dm1[size-1][size-2];
            else
                should = dm1[size-1][size-1];

            DenseVector<DT_> v1(Reduction<rt_max, Tag_>::value(dm1));
            TEST_CHECK_EQUAL_WITHIN_EPS(v1[size-1], should, std::numeric_limits<DT_>::epsilon());
        }
};

DenseMatrixReductionToMaxQuickTest<tags::CPU, float> dense_matrix_reduction_to_max_quick_test_float("float");
DenseMatrixReductionToMaxQuickTest<tags::CPU, double> dense_matrix_reduction_to_max_quick_test_double("double");
DenseMatrixReductionToMaxQuickTest<tags::CPU::MultiCore, float> mc_dense_matrix_reduction_to_max_quick_test_float("MC float");
DenseMatrixReductionToMaxQuickTest<tags::CPU::MultiCore, double> mc_dense_matrix_reduction_to_max_quick_test_double("MC double");
#ifdef HONEI_CELL
DenseMatrixReductionToMaxQuickTest<tags::Cell, float> cell_dense_matrix_reduction_to_max_quick_test_float("Cell float");
#endif


template <typename Tag_, typename DT_>
class SparseMatrixReductionToMaxTest :
    public BaseTest
{
    public:
        SparseMatrixReductionToMaxTest(const std::string & type) :
            BaseTest("sparse_matrix_reduction_to_max_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            for (unsigned long size(1) ; size < (1 << 10) ; size <<= 1)
            {
                SparseMatrix<DT_> sm1(size, size);
                for (typename SparseMatrix<DT_>::ElementIterator i(sm1.begin_elements()), i_end(sm1.end_elements()) ;
                        i != i_end ; ++i)
            {
                if (i.index() % 2 == 0)
                    *i = i.index();
                else
                    *i = i.index() * DT_(-1);
            }

            DT_ should;
            if (size % 2 == 0)
                should = sm1[size-1][size-2];
            else
                should = sm1[size-1][size-1];

                DenseVector<DT_> v1(Reduction<rt_max, Tag_>::value(sm1));
                TEST_CHECK_EQUAL_WITHIN_EPS(v1[size-1], should, std::numeric_limits<DT_>::epsilon());
            }
        }
};

SparseMatrixReductionToMaxTest<tags::CPU, float> sparse_matrix_reduction_to_max_test_float("float");
SparseMatrixReductionToMaxTest<tags::CPU, double> sparse_matrix_reduction_to_max_test_double("double");
SparseMatrixReductionToMaxTest<tags::CPU::MultiCore, float> mc_sparse_matrix_reduction_to_max_test_float("MC float");
SparseMatrixReductionToMaxTest<tags::CPU::MultiCore, double> mc_sparse_matrix_reduction_to_max_test_double("MC double");
#ifdef HONEI_CELL
//SparseMatrixReductionToMaxTest<tags::Cell, float> sparse_matrix_reduction_to_max_test_float_cell("Cell float");
#endif

template <typename Tag_, typename DT_>
class SparseMatrixReductionToMaxQuickTest :
    public QuickTest
{
    public:
        SparseMatrixReductionToMaxQuickTest(const std::string & type) :
            QuickTest("sparse_matrix_reduction_to_max_quick_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            unsigned long size(22);
            SparseMatrix<DT_> sm1(size, size);
            for (typename SparseMatrix<DT_>::ElementIterator i(sm1.begin_elements()), i_end(sm1.end_elements()) ;
                    i != i_end ; ++i)
            {
                    if (i.index() % 2 == 0)
                        *i = i.index();
                    else
                        *i = i.index() * DT_(-1);
            }

                DT_ should;
                if (size % 2 == 0)
                    should = sm1[size-1][size-2];
                else
                    should = sm1[size-1][size-1];

            DenseVector<DT_> v1(Reduction<rt_max, Tag_>::value(sm1));
            TEST_CHECK_EQUAL_WITHIN_EPS(v1[size-1], should, std::numeric_limits<DT_>::epsilon());
        }
};

SparseMatrixReductionToMaxQuickTest<tags::CPU, float> sparse_matrix_reduction_to_max_quick_test_float("float");
SparseMatrixReductionToMaxQuickTest<tags::CPU, double> sparse_matrix_reduction_to_max_quick_test_double("double");
SparseMatrixReductionToMaxQuickTest<tags::CPU::MultiCore, float> mc_sparse_matrix_reduction_to_max_quick_test_float("MC float");
SparseMatrixReductionToMaxQuickTest<tags::CPU::MultiCore, double> mc_sparse_matrix_reduction_to_max_quick_test_double("MC double");
#ifdef HONEI_CELL
//SparseMatrixReductionToMaxQuickTest<tags::Cell, float> sparse_matrix_reduction_to_max_quick_test_float_cell("Cell float");
#endif

template <typename Tag_, typename DT_>
class BandedMatrixReductionToMaxTest :
    public BaseTest
{
    public:
        BandedMatrixReductionToMaxTest(const std::string & type) :
            BaseTest("banded_matrix_reduction_to_max_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            for (unsigned long size(4) ; size < (1 << 10) ; size <<= 1)
            {
                DenseVector<DT_>  result(size);
                typename DenseVector<DT_>::ElementIterator resultIter(result.begin_elements());
                BandedMatrix<DT_> bm1(size);
                DT_ rand(0);
                for (typename BandedMatrix<DT_>::ElementIterator i(bm1.begin_elements()), i_end(bm1.end_elements()) ;
                        i != i_end ; ++i)
                {
                    rand = DT_(std::rand()) / DT_(std::rand()-RAND_MAX / 2);
                    *i = rand;

                    if (i.index() == 0) 
                    {
                       *resultIter = rand;
                    }
                    
                    if ((i.index() != 0) && ((i.index() % size) == 0))
                    {
                        ++resultIter;
                        *resultIter = rand;
                    }

                    if  (rand > *resultIter)
                    {
                        *resultIter = rand;
                    }
                }

                DenseVector<DT_> v1(Reduction<rt_max, Tag_>::value(bm1));
               
                for (typename DenseVector<DT_>::ElementIterator i(v1.begin_elements()), i_end(v1.end_elements()), r(result.begin_elements()) ; 
                    i != i_end ; )
                {
                    TEST_CHECK_EQUAL_WITHIN_EPS(*i, *r, std::numeric_limits<DT_>::epsilon());
                    ++r;
                    ++i;
                }
            }
        }
};

BandedMatrixReductionToMaxTest<tags::CPU, float> banded_matrix_reduction_to_max_test_float("float");
BandedMatrixReductionToMaxTest<tags::CPU, double> banded_matrix_reduction_to_max_test_double("double");
BandedMatrixReductionToMaxTest<tags::CPU::MultiCore, float> mc_banded_matrix_reduction_to_max_test_float("MC float");
BandedMatrixReductionToMaxTest<tags::CPU::MultiCore, double> mc_banded_matrix_reduction_to_max_test_double("MC double");

template <typename Tag_, typename DT_>
class BandedMatrixReductionToMaxQuickTest :
    public QuickTest
{
    public:
        BandedMatrixReductionToMaxQuickTest(const std::string & type) :
            QuickTest("banded_matrix_reduction_to_max_quick_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            unsigned long size(22);
            DenseVector<DT_>  result(size);
            typename DenseVector<DT_>::ElementIterator resultIter(result.begin_elements());
            BandedMatrix<DT_> bm1(size);
            DT_ rand(0);
            for (typename BandedMatrix<DT_>::ElementIterator i(bm1.begin_elements()), i_end(bm1.end_elements()) ;
                i != i_end ; ++i)
            {
                rand = DT_(std::rand()) / DT_(std::rand()-RAND_MAX / 2);
                *i = rand;

                if (i.index() == 0) 
                {
                   *resultIter = rand;
                }

                if ((i.index() != 0) && ((i.index() % size) == 0))
                {
                    ++resultIter;
                    *resultIter = rand;
                }

                if  (rand > *resultIter)
                {
                    *resultIter = rand;
                }
            }

            DenseVector<DT_> v1(Reduction<rt_max, Tag_>::value(bm1));
                           
            for (typename DenseVector<DT_>::ElementIterator i(v1.begin_elements()), i_end(v1.end_elements()), r(result.begin_elements()) ; 
                i != i_end ; )
            {
                TEST_CHECK_EQUAL_WITHIN_EPS(*i, *r, std::numeric_limits<DT_>::epsilon());
                ++r;
                ++i;
            }
        }
};

BandedMatrixReductionToMaxQuickTest<tags::CPU, float> banded_matrix_reduction_to_max_quick_test_float("float");
BandedMatrixReductionToMaxQuickTest<tags::CPU, double> banded_matrix_reduction_to_max_quick_test_double("double");
BandedMatrixReductionToMaxQuickTest<tags::CPU::MultiCore, float> mc_banded_matrix_reduction_to_max_quick_test_float("MC float");
BandedMatrixReductionToMaxQuickTest<tags::CPU::MultiCore, double> mc_banded_matrix_reduction_to_max_quick_test_double("MC double");

template <typename Tag_, typename DT_>
class DenseVectorRangeReductionToMaxTest :
    public BaseTest
{
    public:
        DenseVectorRangeReductionToMaxTest(const std::string & type) :
            BaseTest("dense_vector_range_reduction_to_max_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            for (unsigned long size(1) ; size < (1 << 16) ; size <<= 1)
            {
                DenseVector<DT_> dv(size*4, DT_(100));
                for(int j(0) ; j < 4 ; j++)
                {
                    DenseVectorRange<DT_> dvr(dv, size, j);
                    for (typename DenseVectorRange<DT_>::ElementIterator i(dvr.begin_elements()), i_end(dvr.end_elements()) ;
                            i != i_end ; ++i)
                    {

                        if (i.index() % 2 == 0)
                            *i = i.index();
                        else
                            *i = i.index() * DT_(-1);
                    }

                    DT_ should;
                    if (size % 2 == 0)
                        should = dvr[size-2];
                    else
                        should = dvr[size-1];

                    DT_ v1(Reduction<rt_max, Tag_>::value(dvr));
                    TEST_CHECK_EQUAL_WITHIN_EPS(v1, should, std::numeric_limits<DT_>::epsilon());
                }
            }
        }
};

DenseVectorRangeReductionToMaxTest<tags::CPU, float> dense_vector_range_reduction_to_max_test_float("float");
DenseVectorRangeReductionToMaxTest<tags::CPU, double> dense_vector_range_reduction_to_max_test_double("double");
DenseVectorRangeReductionToMaxTest<tags::CPU::MultiCore, float> mc_dense_vector_range_reduction_to_max_test_float("MC float");
DenseVectorRangeReductionToMaxTest<tags::CPU::MultiCore, double> mc_dense_vector_range_reduction_to_max_test_double("MC double");
/*
#ifdef HONEI_SSE
DenseVectorRangeReductionToMaxTest<tags::CPU::SSE, float> sse_dense_vector_range_reduction_to_max_test_float("SSE float");
DenseVectorRangeReductionToMaxTest<tags::CPU::SSE, double> sse_dense_vector_range_reduction_to_max_test_double("SSE double");
DenseVectorRangeReductionToMaxTest<tags::CPU::MultiCore::SSE, float> sse_mc_dense_vector_range_reduction_to_max_test_float("MC SSE float");
DenseVectorRangeReductionToMaxTest<tags::CPU::MultiCore::SSE, double> sse_mc_dense_vector_range_reduction_to_max_test_double("MC SSE double");
#endif
*/
#ifdef HONEI_CELL
DenseVectorRangeReductionToMaxTest<tags::Cell, float> dense_vector_range_reduction_to_max_test_float_cell("Cell float");
#endif

template <typename Tag_, typename DT_>
class DenseVectorRangeReductionToMaxQuickTest :
    public QuickTest
{
    public:
        DenseVectorRangeReductionToMaxQuickTest(const std::string & type) :
            QuickTest("dense_vector_range_reduction_to_max_quick_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            unsigned long size(22);
            DenseVector<DT_> dv(size*4, DT_(100));
            for(int j(0) ; j < 4 ; j++)
            {
                DenseVectorRange<DT_> dvr(dv, size, j);
                for (typename DenseVectorRange<DT_>::ElementIterator i(dvr.begin_elements()), i_end(dvr.end_elements()) ;
                        i != i_end ; ++i)
                {

                    if (i.index() % 2 == 0)
                        *i = i.index();
                    else
                        *i = i.index() * DT_(-1);
                }

                DT_ should;
                if (size % 2 == 0)
                    should = dvr[size-2];
                else
                    should = dvr[size-1];

                DT_ v1(Reduction<rt_max, Tag_>::value(dvr));
                TEST_CHECK_EQUAL_WITHIN_EPS(v1, should, std::numeric_limits<DT_>::epsilon());
            }
        }
};
DenseVectorRangeReductionToMaxQuickTest<tags::CPU, float>  dense_vector_range_reduction_to_max_quick_test_float("float");
DenseVectorRangeReductionToMaxQuickTest<tags::CPU, double> dense_vector_range_reduction_to_max_quick_test_double("double");
DenseVectorRangeReductionToMaxQuickTest<tags::CPU::MultiCore, float>  mc_dense_vector_range_reduction_to_max_quick_test_float("MC float");
DenseVectorRangeReductionToMaxQuickTest<tags::CPU::MultiCore, double> mc_dense_vector_range_reduction_to_max_quick_test_double("MC double");
/*
#ifdef HONEI_SSE
DenseVectorRangeReductionToMaxQuickTest<tags::CPU::SSE, float>  sse_dense_vector_range_reduction_to_max_quick_test_float("SSE float");
DenseVectorRangeReductionToMaxQuickTest<tags::CPU::SSE, double> sse_dense_vector_range_reduction_to_max_quick_test_double("SSE double");
DenseVectorRangeReductionToMaxQuickTest<tags::CPU::MultiCore::SSE, float>  sse_mc_dense_vector_range_reduction_to_max_quick_test_float("MC SSE float");
DenseVectorRangeReductionToMaxQuickTest<tags::CPU::MultiCore::SSE, double> sse_mc_dense_vector_range_reduction_to_max_quick_test_double("MC SSE double");
#endif
*/
#ifdef HONEI_CELL
DenseVectorRangeReductionToMaxQuickTest<tags::Cell, float> dense_vector_range_reduction_to_max_quick_test_float_cell("Cell float");
#endif


template <typename Tag_, typename DT_>
class DenseVectorReductionToMaxTest :
    public BaseTest
{
    public:
        DenseVectorReductionToMaxTest(const std::string & type) :
            BaseTest("dense_vector_reduction_to_max_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            for (unsigned long size(1) ; size < (1 << 10) ; size <<= 1)
            {
                DenseVector<DT_> dv1(size);
                for (typename DenseVector<DT_>::ElementIterator i(dv1.begin_elements()), i_end(dv1.end_elements()) ;
                        i != i_end ; ++i)
                {
                    *i = i.index();
                }

                DT_ v1(Reduction<rt_max, Tag_>::value(dv1));
                TEST_CHECK_EQUAL_WITHIN_EPS(v1, size-1, std::numeric_limits<DT_>::epsilon());
            }
        }
};

DenseVectorReductionToMaxTest<tags::CPU, float> dense_vector_reduction_to_max_test_float("float");
DenseVectorReductionToMaxTest<tags::CPU, double> dense_vector_reduction_to_max_test_double("double");
DenseVectorReductionToMaxTest<tags::CPU::MultiCore, float> mc_dense_vector_reduction_to_max_test_float("MC float");
DenseVectorReductionToMaxTest<tags::CPU::MultiCore, double> mc_dense_vector_reduction_to_max_test_double("MC double");


template <typename Tag_, typename DT_>
class DenseVectorReductionToMaxQuickTest :
    public QuickTest
{
    public:
        DenseVectorReductionToMaxQuickTest(const std::string & type) :
            QuickTest("dense_vector_reduction_to_max_quick_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            unsigned long size(22);
            DenseVector<DT_> dv1(size);
            for (typename DenseVector<DT_>::ElementIterator i(dv1.begin_elements()), i_end(dv1.end_elements()) ;
                    i != i_end ; ++i)
            {
                *i = i.index();
            }

            DT_ v1(Reduction<rt_max,Tag_>::value(dv1));
            TEST_CHECK_EQUAL_WITHIN_EPS(v1, size-1, std::numeric_limits<DT_>::epsilon());
        }
};

DenseVectorReductionToMaxQuickTest<tags::CPU, float> dense_reduction_to_max_quick_test_float("float");
DenseVectorReductionToMaxQuickTest<tags::CPU, double> dense_reduction_to_max_quick_test_double("double");
DenseVectorReductionToMaxQuickTest<tags::CPU::MultiCore, float> mc_dense_reduction_to_max_quick_test_float("MC float");
DenseVectorReductionToMaxQuickTest<tags::CPU::MultiCore, double> mc_dense_reduction_to_max_quick_test_double("MC double");
#ifdef HONEI_CELL
DenseVectorReductionToMaxQuickTest<tags::Cell, float> dense_vector_reduction_to_max_quick_test_float_cell("Cell float");
#endif

template <typename Tag_, typename DT_>
class SparseVectorReductionToMaxTest :
    public BaseTest
{
    public:
        SparseVectorReductionToMaxTest(const std::string & type) :
            BaseTest("sparse_vector_reduction_to_max_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            for (unsigned long size(1) ; size < (1 << 10) ; size <<= 1)
            {
                SparseVector<DT_> sv1(size, size / 8 + 1);
                for (typename SparseVector<DT_>::ElementIterator i(sv1.begin_elements()), i_end(sv1.end_elements()) ;
                        i != i_end ; ++i)
                {
                    *i = i.index();
                }

                DT_ v1(Reduction<rt_max, Tag_>::value(sv1));
                TEST_CHECK_EQUAL_WITHIN_EPS(v1, size-1, std::numeric_limits<DT_>::epsilon());
            }
        }
};

SparseVectorReductionToMaxTest<tags::CPU, float> sparse_vector_reduction_to_max_test_float("float");
SparseVectorReductionToMaxTest<tags::CPU, double> sparse_vector_reduction_to_max_test_double("double");
SparseVectorReductionToMaxTest<tags::CPU::MultiCore, float> mc_sparse_vector_reduction_to_max_test_float("MC float");
SparseVectorReductionToMaxTest<tags::CPU::MultiCore, double> mc_sparse_vector_reduction_to_max_test_double("MC double");
#ifdef HONEI_CELL
SparseVectorReductionToMaxTest<tags::Cell, float> sparse_vector_reduction_to_max_test_float_cell("Cell float");
#endif

template <typename Tag_, typename DT_>
class SparseVectorReductionToMaxQuickTest :
    public QuickTest
{
    public:
        SparseVectorReductionToMaxQuickTest(const std::string & type) :
            QuickTest("sparse_vector_reduction_to_max_quick_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            unsigned long size(22);
            SparseVector<DT_> sv1(size, size / 8 + 1);
            for (typename SparseVector<DT_>::ElementIterator i(sv1.begin_elements()), i_end(sv1.end_elements()) ;
                    i != i_end ; ++i)
            {
                *i = i.index();
            }

            DT_ v1(Reduction<rt_max, Tag_>::value(sv1));
            TEST_CHECK_EQUAL_WITHIN_EPS(v1, size-1, std::numeric_limits<DT_>::epsilon());
        }
};

SparseVectorReductionToMaxQuickTest<tags::CPU, float> sparse_reduction_to_max_quick_test_float("float");
SparseVectorReductionToMaxQuickTest<tags::CPU, double> sparse_reduction_to_max_quick_test_double("double");
SparseVectorReductionToMaxQuickTest<tags::CPU::MultiCore, float> mc_sparse_reduction_to_max_quick_test_float("MC float");
SparseVectorReductionToMaxQuickTest<tags::CPU::MultiCore, double> mc_sparse_reduction_to_max_quick_test_double("MC double");
#ifdef HONEI_CELL
SparseVectorReductionToMaxQuickTest<tags::Cell, float> sparse_vector_reduction_to_max_quick_test_float_cell("Cell float");
#endif
