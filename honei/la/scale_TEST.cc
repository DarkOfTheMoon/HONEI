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
 * Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <honei/la/banded_matrix.hh>
#include <honei/la/dense_matrix.hh>
#include <honei/la/norm.hh>
#include <honei/la/scale.hh>
#include <honei/util/unittest.hh>

#include <limits>

using namespace honei;
using namespace tests;

// Test cases for scaling matrices

template <typename Tag_, typename DataType_>
class BandedMatrixScaleTest :
    public BaseTest
{
    public:
        BandedMatrixScaleTest(const std::string & type) :
            BaseTest("banded_matrix_scale_test<" + type + ">")
        {
        }

        virtual void run() const
        {
            for (unsigned long size(10) ; size < (1 << 9) ; size <<= 1)
            {
                DenseVector<DataType_> dv1(size, DataType_(2));
                BandedMatrix<DataType_> bm1(size, dv1);
                Scale<Tag_>::value(bm1, DataType_(3));
                for (typename BandedMatrix<DataType_>::ConstBandIterator ce(bm1.begin_bands()),
                        ce_end(bm1.end_bands()) ; ce != ce_end ; ++ce)
                {
                    if (ce.index() != size - 1 )
                    {
                        for (typename DenseVector<DataType_>::ConstElementIterator i(ce->begin_elements()),
                            i_end(ce->end_elements()) ; i < i_end ; ++i)
                        {
                            TEST_CHECK_EQUAL_WITHIN_EPS(*i, 0, std::numeric_limits<DataType_>::epsilon());
                        }
                    }
                    else
                    {
                        for (typename DenseVector<DataType_>::ConstElementIterator i(ce->begin_elements()),
                            i_end(ce->end_elements()) ; i < i_end ; ++i)
                        {
                            TEST_CHECK_EQUAL_WITHIN_EPS(*i, 6, std::numeric_limits<DataType_>::epsilon());
                        }
                    }
                }
            }
        }
};
BandedMatrixScaleTest<tags::CPU, float> banded_matrix_scale_test_float("float");
BandedMatrixScaleTest<tags::CPU, double> banded_matrix_scale_test_double("double");
//BandedMatrixScaleTest<tags::CPU::MultiCore, float> mc_banded_matrix_scale_test_float("MC float");
//BandedMatrixScaleTest<tags::CPU::MultiCore, double> mc_banded_matrix_scale_test_double("MC double");
#ifdef HONEI_SSE
//BandedMatrixScaleTest<tags::CPU::MultiCore::SSE, float> mc_sse_banded_matrix_scale_test_float("MC SSE float");
//BandedMatrixScaleTest<tags::CPU::MultiCore::SSE, double> mc_sse_banded_matrix_scale_test_double("MC SSE double");
#endif
#ifdef HONEI_CELL
//BandedMatrixScaleTest<tags::Cell, float> cell_banded_matrix_scale_test_float("Cell float");
#endif

template <typename Tag_, typename DataType_>
class BandedMatrixScaleQuickTest :
    public QuickTest
{
    public:
        BandedMatrixScaleQuickTest(const std::string & type) :
            QuickTest("banded_matrix_scale_quick_test<" + type + ">")
        {
        }

        virtual void run() const
        {
            unsigned long size(20);
            DenseVector<DataType_> dv1(size, DataType_(2));
            BandedMatrix<DataType_> bm1(size, dv1);
            Scale<Tag_>::value(bm1, DataType_(3));
            for (typename BandedMatrix<DataType_>::ConstBandIterator ce(bm1.begin_bands()),
                    ce_end(bm1.end_bands()) ; ce != ce_end ; ++ce)
            {
                if (ce.index() != size - 1 )
                {
                    for (typename DenseVector<DataType_>::ConstElementIterator i(ce->begin_elements()),
                            i_end(ce->end_elements()) ; i < i_end ; ++i)
                    {
                        TEST_CHECK_EQUAL_WITHIN_EPS(*i, 0, std::numeric_limits<DataType_>::epsilon());
                    }
                }
                else
                {
                    for (typename DenseVector<DataType_>::ConstElementIterator i(ce->begin_elements()),
                            i_end(ce->end_elements()) ; i < i_end ; ++i)
                    {
                        TEST_CHECK_EQUAL_WITHIN_EPS(*i, 6, std::numeric_limits<DataType_>::epsilon());
                    }
                }
            }
        }
};
BandedMatrixScaleQuickTest<tags::CPU, float> banded_matrix_scale_quick_test_float("float");
BandedMatrixScaleQuickTest<tags::CPU, double> banded_matrix_scale_quick_test_double("double");
//BandedMatrixScaleQuickTest<tags::CPU::MultiCore, float> mc_banded_matrix_scale_quick_test_float("MC float");
//BandedMatrixScaleQuickTest<tags::CPU::MultiCore, double> mc_banded_matrix_scale_quick_test_double("MC double");
#ifdef HONEI_SSE
//BandedMatrixScaleQuickTest<tags::CPU::MultiCore::SSE, float> mc_sse_banded_matrix_scale_quick_test_float("MC SSE float");
//BandedMatrixScaleQuickTest<tags::CPU::MultiCore::SSE, double> mc_sse_banded_matrix_scale_quick_test_double("MC SSE double");
#endif
#ifdef HONEI_CELL
//BandedMatrixScaleQuickTest<tags::Cell, float> cell_banded_matrix_scale_quick_test_float("Cell float");
#endif


template <typename Tag_, typename DataType_>
class DenseMatrixScaleTest :
    public BaseTest
{
    public:
        DenseMatrixScaleTest(const std::string & type) :
            BaseTest("dense_matrix_scale_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            for (unsigned long size(10) ; size < (1 << 9) ; size <<= 1)
            {
                DenseMatrix<DataType_> dm1(size+1, size, DataType_(2)), dm2(size+1, size, DataType_(6));
                Scale<Tag_>::value(dm1, DataType_(3));

                TEST(dm1.lock(lm_read_only), TEST_CHECK_EQUAL(dm1, dm2), dm1.unlock(lm_read_only));
            }
        }
};
DenseMatrixScaleTest<tags::CPU, float> scalar_dense_matrix_scale_test_float("float");
DenseMatrixScaleTest<tags::CPU, double> scalar_dense_matrix_scale_test_double("double");
//DenseMatrixScaleTest<tags::CPU::MultiCore, float> mc_scalar_dense_matrix_scale_test_float("MC float");
//DenseMatrixScaleTest<tags::CPU::MultiCore, double> mc_scalar_dense_matrix_scale_test_double("MC double");
#ifdef HONEI_SSE
//DenseMatrixScaleTest<tags::CPU::SSE, float> sse_scalar_dense_matrix_scale_test_float("SSE float");
//DenseMatrixScaleTest<tags::CPU::SSE, double> sse_scalar_dense_matrix_scale_test_double("SSE double");
//DenseMatrixScaleTest<tags::CPU::MultiCore::SSE, float> mc_sse_scalar_dense_matrix_scale_test_float("MC SSE float");
//DenseMatrixScaleTest<tags::CPU::MultiCore::SSE, double> mc_sse_scalar_dense_matrix_scale_test_double("MC SSE double");
#endif
#ifdef HONEI_CUDA
DenseMatrixScaleTest<tags::GPU::CUDA, float> cuda_scalar_dense_matrix_scale_test_float("float");
#endif
#ifdef HONEI_CELL
DenseMatrixScaleTest<tags::Cell, float> cell_dense_matrix_scale_test_float("Cell float");
DenseMatrixScaleTest<tags::Cell, double> cell_dense_matrix_scale_test_double("Cell double");
#endif

template <typename Tag_, typename DataType_>
class DenseMatrixScaleQuickTest :
    public QuickTest
{
    public:
        DenseMatrixScaleQuickTest(const std::string & type) :
            QuickTest("dense_matrix_scale_quick_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            unsigned long size(100);
            DenseMatrix<DataType_> dm(size+1, size, DataType_(2));
            Scale<Tag_>::value(dm, DataType_(3));

            dm.lock(lm_read_only);
            DataType_ vsum(0);
            for (typename DenseMatrix<DataType_>::ElementIterator i(dm.begin_elements()),
                    i_end(dm.end_elements()) ; i != i_end ; ++i)
            {
                vsum += *i;
            }
            dm.unlock(lm_read_only);
            TEST_CHECK_EQUAL_WITHIN_EPS(vsum, static_cast<DataType_>(6 * size * (size +1)),
                std::numeric_limits<DataType_>::epsilon());
        }
};
DenseMatrixScaleQuickTest<tags::CPU, float> dense_matrix_scale_quick_test_float("float");
DenseMatrixScaleQuickTest<tags::CPU, double> dense_matrix_scale_quick_test_double("double");
//DenseMatrixScaleQuickTest<tags::CPU::MultiCore, float> mc_dense_matrix_scale_quick_test_float("MC float");
//DenseMatrixScaleQuickTest<tags::CPU::MultiCore, double> mc_dense_matrix_scale_quick_test_double("MC double");
#ifdef HONEI_SSE
//DenseMatrixScaleQuickTest<tags::CPU::SSE, float> sse_dense_matrix_scale_quick_test_float("SSE float");
//DenseMatrixScaleQuickTest<tags::CPU::SSE, double> sse_dense_matrix_scale_quick_test_double("SSE double");
//DenseMatrixScaleQuickTest<tags::CPU::MultiCore::SSE, float> mc_sse_dense_matrix_scale_quick_test_float("MC SSE float");
//DenseMatrixScaleQuickTest<tags::CPU::MultiCore::SSE, double> mc_sse_dense_matrix_scale_quick_test_double("MC SSE double");
#endif
#ifdef HONEI_CUDA
DenseMatrixScaleQuickTest<tags::GPU::CUDA, float> cuda_dense_matrix_scale_quick_test_float("float");
#endif
#ifdef HONEI_CELL
DenseMatrixScaleQuickTest<tags::Cell, float> cell_dense_matrix_scale_quick_test_float("Cell float");
#endif

template <typename Tag_, typename DataType_>
class SparseMatrixScalarTest :
    public BaseTest
{
    public:
        SparseMatrixScalarTest(const std::string & type) :
            BaseTest("sparse_matrix_scale_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            for (unsigned long size(10) ; size < (1 << 9) ; size <<= 1)
            {
                SparseMatrix<DataType_> sm1(size+1, size, size / 8 + 1),
                    sm2(size+1, size, size / 7 + 1);
                for (typename SparseMatrix<DataType_>::ElementIterator i(sm1.begin_elements()),
                    i_end(sm1.end_elements()), j(sm2.begin_elements()) ; i != i_end ; ++i, ++j)
                {
                    if (i.index() % 10 == 0)
                    {
                        *i = DataType_(2);
                        *j = DataType_(6);
                    }
                }
                Scale<Tag_>::value(sm1, DataType_(3));

                TEST_CHECK_EQUAL(sm1, sm2);
            }
        }
};

SparseMatrixScalarTest<tags::CPU, float> sparse_matrix_test_float("float");
SparseMatrixScalarTest<tags::CPU, double> sparse_matrix_test_double("double");
//SparseMatrixScalarTest<tags::CPU::MultiCore, float> mc_sparse_matrix_scale_test_float("MC float");
//SparseMatrixScalarTest<tags::CPU::MultiCore, double> mc_sparse_matrix_scale_test_double("MC double");
#ifdef HONEI_SSE
//SparseMatrixScalarTest<tags::CPU::MultiCore::SSE, float> mc_sse_sparse_matrix_scale_test_float("MC SSE float");
//SparseMatrixScalarTest<tags::CPU::MultiCore::SSE, double> mc_sse_sparse_matrix_scale_test_double("MC SSE double");
//SparseMatrixScalarTest<tags::CPU::SSE, float> sse_sparse_matrix_scale_test_float("SSE float");
//SparseMatrixScalarTest<tags::CPU::SSE, double> sse_sparse_matrix_scale_test_double("SSE double");
#endif
#ifdef HONEI_CELL
//SparseMatrixScalarTest<tags::Cell, float> cell_sparse_matrix_scalar_test_float("Cell float");
#endif

template <typename Tag_, typename DataType_>
class SparseMatrixScaleQuickTest :
    public QuickTest
{
    public:
        SparseMatrixScaleQuickTest(const std::string & type) :
            QuickTest("sparse_matrix_scale_quick_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            unsigned long size (22);
            SparseMatrix<DataType_> sm1(size+1, size, size / 8 + 1),
                sm2(size+1, size, size / 7 + 1);
            for (typename SparseMatrix<DataType_>::ElementIterator i(sm1.begin_elements()),
                i_end(sm1.end_elements()), j(sm2.begin_elements()) ; i != i_end ; ++i, ++j)
            {
                if (i.index() % 10 == 0)
                {
                    *i = DataType_(2);
                    *j = DataType_(6);
                }
            }
            Scale<Tag_>::value(sm1, DataType_(3));

            TEST_CHECK_EQUAL(sm1, sm2);
        }
};
SparseMatrixScaleQuickTest<tags::CPU, float> sparse_matrix_scale_quick_test_float("float");
SparseMatrixScaleQuickTest<tags::CPU, double> sparse_matrix_scale_quick_test_double("double");
//SparseMatrixScaleQuickTest<tags::CPU::MultiCore, float> mc_sparse_matrix_quick_test_float("MC float");
//SparseMatrixScaleQuickTest<tags::CPU::MultiCore, double> mc_sparse_matrix_quick_test_double("MC double");
#ifdef HONEI_SSE
//SparseMatrixScaleQuickTest<tags::CPU::MultiCore::SSE, float> mc_sse_sparse_matrix_scalar_quick_test_float("MC SSE float");
//SparseMatrixScaleQuickTest<tags::CPU::MultiCore::SSE, double> mc_sse_sparse_matrix_quick_test_double("MC SSE double");
//SparseMatrixScaleQuickTest<tags::CPU::SSE, float> sse_sparse_matrix_quick_test_float("SSE float");
//SparseMatrixScaleQuickTest<tags::CPU::SSE, double> sse_sparse_matrix_quick_test_double("SSE double");
#endif
#ifdef HONEI_CELL
//SparseMatrixScaleQuickTest<tags::Cell, float> cell_sparse_matrix_scalar_quick_test_float("Cell float");
#endif



// Test cases for scaling vectors

template <typename Tag_, typename DataType_>
class DenseVectorScaleTest :
    public BaseTest
{
    public:
        DenseVectorScaleTest(const std::string & type) :
            BaseTest("dense_vector_scale_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            for (unsigned long size(10) ; size < (1 << 8) ; size <<= 1)
            {
                DenseVector<DataType_> dv(size, DataType_(3));
                Scale<Tag_>::value(dv, DataType_(2));
                dv.lock(lm_read_only);
                DataType_ v1(Norm<vnt_l_one>::value(dv));
                dv.unlock(lm_read_only);
                TEST_CHECK_EQUAL(v1, 6 * size);
            }
        }
};

DenseVectorScaleTest<tags::CPU, float> dense_vector_scale_test_float("float");
DenseVectorScaleTest<tags::CPU, double> dense_vector_scale_test_double("double");
DenseVectorScaleTest<tags::CPU::MultiCore, float> mc_dense_vector_scale_test_float("MC float");
DenseVectorScaleTest<tags::CPU::MultiCore, double> mc_dense_vector_scale_test_double("MC double");
DenseVectorScaleTest<tags::CPU::Generic, float> generic_dense_vector_scale_test_float("float");
DenseVectorScaleTest<tags::CPU::Generic, double> generic_dense_vector_scale_test_double("double");
DenseVectorScaleTest<tags::CPU::MultiCore::Generic, float> generic_mc_dense_vector_scale_test_float("MC float");
DenseVectorScaleTest<tags::CPU::MultiCore::Generic, double> generic_mc_dense_vector_scale_test_double("MC double");
#ifdef HONEI_SSE
DenseVectorScaleTest<tags::CPU::SSE, float> sse_dense_vector_scale_test_float("sse float");
DenseVectorScaleTest<tags::CPU::SSE, double> sse_dense_vector_scale_test_double("sse double");
DenseVectorScaleTest<tags::CPU::MultiCore::SSE, float> mc_sse_dense_vector_scale_test_float("MC SSE float");
DenseVectorScaleTest<tags::CPU::MultiCore::SSE, double> mc_sse_dense_vector_scale_test_double("MC SSE double");
#endif
#ifdef HONEI_CUDA
DenseVectorScaleTest<tags::GPU::CUDA, float> cuda_dense_vector_scale_test_float("float");
DenseVectorScaleTest<tags::GPU::MultiCore::CUDA, float> mc_cuda_dense_vector_scale_test_float("float");
#ifdef HONEI_CUDA_DOUBLE
DenseVectorScaleTest<tags::GPU::CUDA, double> cuda_dense_vector_scale_test_double("double");
DenseVectorScaleTest<tags::GPU::MultiCore::CUDA, double> mc_cuda_dense_vector_scale_test_double("double");
#endif
#endif
#ifdef HONEI_CELL
DenseVectorScaleTest<tags::Cell, float> cell_dense_vector_scale_test_float("Cell float");
DenseVectorScaleTest<tags::Cell, double> cell_dense_vector_scale_test_double("Cell double");
#endif
#ifdef HONEI_OPENCL
DenseVectorScaleTest<tags::OpenCL::CPU, float> ocl_cpu_dense_vector_scale_test_float("float");
DenseVectorScaleTest<tags::OpenCL::CPU, double> ocl_cpu_dense_vector_scale_test_double("double");
DenseVectorScaleTest<tags::OpenCL::GPU, float> ocl_gpu_dense_vector_scale_test_float("float");
#ifdef HONEI_CUDA_DOUBLE
DenseVectorScaleTest<tags::OpenCL::GPU, double> ocl_gpu_dense_vector_scale_test_double("double");
#endif
#endif

template <typename Tag_, typename DataType_>
class DenseVectorScaleQuickTest :
    public QuickTest
{
    public:
       DenseVectorScaleQuickTest(const std::string & type) :
            QuickTest("dense_vector_scale_quick_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
#if defined HONEI_CELL
            unsigned long size(18225);
#else
            unsigned long size(4711);
#endif
            DenseVector<DataType_> dv1(size, DataType_(3));

            Scale<Tag_>::value(dv1, DataType_(2));
            dv1.lock(lm_read_only);
            DataType_ v1(Norm<vnt_l_one>::value(dv1));
            dv1.unlock(lm_read_only);
            TEST_CHECK_EQUAL(v1, 6 * size);
        }
};
DenseVectorScaleQuickTest<tags::CPU, float>  dense_vector_scale_quick_test_float("float");
DenseVectorScaleQuickTest<tags::CPU, double> dense_vector_scale_quick_test_double("double");
DenseVectorScaleQuickTest<tags::CPU::MultiCore, float> mc_dense_vector_scale_quick_test_float("MC float");
DenseVectorScaleQuickTest<tags::CPU::MultiCore, double> mc_dense_vector_scale_quick_test_double("MC double");
DenseVectorScaleQuickTest<tags::CPU::Generic, float>  generic_dense_vector_scale_quick_test_float("float");
DenseVectorScaleQuickTest<tags::CPU::Generic, double> generic_dense_vector_scale_quick_test_double("double");
DenseVectorScaleQuickTest<tags::CPU::MultiCore::Generic, float> generic_mc_dense_vector_scale_quick_test_float("MC float");
DenseVectorScaleQuickTest<tags::CPU::MultiCore::Generic, double> generic_mc_dense_vector_scale_quick_test_double("MC double");
#ifdef HONEI_SSE
DenseVectorScaleQuickTest<tags::CPU::SSE, float> sse_dense_vector_scale_quick_test_float("sse float");
DenseVectorScaleQuickTest<tags::CPU::SSE, double> sse_dense_vector_scale_quick_test_double("sse double");
DenseVectorScaleQuickTest<tags::CPU::MultiCore::SSE, float> mc_sse_dense_vector_scale_quick_test_float("MC SSE float");
DenseVectorScaleQuickTest<tags::CPU::MultiCore::SSE, double> mc_sse_dense_vector_scale_quick_test_double("MC SSE double");
#endif
#ifdef HONEI_CUDA
DenseVectorScaleQuickTest<tags::GPU::CUDA, float> cuda_dense_vector_scale_quick_test_float("float");
DenseVectorScaleQuickTest<tags::GPU::MultiCore::CUDA, float> mc_cuda_dense_vector_scale_quick_test_float("float");
#ifdef HONEI_CUDA_DOUBLE
DenseVectorScaleQuickTest<tags::GPU::CUDA, double> cuda_dense_vector_scale_quick_test_double("double");
DenseVectorScaleQuickTest<tags::GPU::MultiCore::CUDA, double> mc_cuda_dense_vector_scale_quick_test_double("double");
#endif
#endif
#ifdef HONEI_CELL
DenseVectorScaleQuickTest<tags::Cell, float> cell_dense_vector_scale_quick_test_float("Cell float");
DenseVectorScaleQuickTest<tags::Cell, double> cell_dense_vector_scale_quick_test_double("Cell double");
#endif
#ifdef HONEI_OPENCL
DenseVectorScaleQuickTest<tags::OpenCL::CPU, float>  ocl_cpu_dense_vector_scale_quick_test_float("float");
DenseVectorScaleQuickTest<tags::OpenCL::CPU, double> ocl_cpu_dense_vector_scale_quick_test_double("double");
DenseVectorScaleQuickTest<tags::OpenCL::GPU, float>  ocl_gpu_dense_vector_scale_quick_test_float("float");
#ifdef HONEI_CUDA_DOUBLE
DenseVectorScaleQuickTest<tags::OpenCL::GPU, double> ocl_gpu_dense_vector_scale_quick_test_double("double");
#endif
#endif

template <typename Tag_, typename DataType_>
class SparseVectorScaleTest :
    public BaseTest
{
    public:
        SparseVectorScaleTest(const std::string & type) :
            BaseTest("sparse_vector_scale_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            for (unsigned long size(11) ; size < (1 << 10) ; size <<= 1)
            {
                SparseVector<DataType_> sv1(size, size / 8 + 1);
                for (typename SparseVector<DataType_>::ElementIterator i(sv1.begin_elements()), i_end(sv1.end_elements()) ;
                        i != i_end ; ++i)
                {
                    if (i.index() % 10 == 0) *i = 3;
                }
                Scale<Tag_>::value(sv1, DataType_(2));
                DataType_ v1(Norm<vnt_l_one>::value(sv1));
                TEST_CHECK_EQUAL(v1, 6 * (size / 10 + 1));
            }
        }
};

SparseVectorScaleTest<tags::CPU, float> sparse_vector_scale_test_float("float");
SparseVectorScaleTest<tags::CPU, double> sparse_vector_scale_test_double("double");
//SparseVectorScaleTest<tags::CPU::MultiCore, float> mc_sparse_vector_scale_test_float("MC float");
//SparseVectorScaleTest<tags::CPU::MultiCore, double> mc_sparse_vector_scale_test_double("MC double");
#ifdef HONEI_SSE
//SparseVectorScaleTest<tags::CPU::MultiCore::SSE, float> mc_sse_sparse_vector_scale_test_float("MC SSE float");
//SparseVectorScaleTest<tags::CPU::MultiCore::SSE, double> mc_sse_sparse_vector_scale_test_double("MC SSE double");
SparseVectorScaleTest<tags::CPU::SSE, float> sse_sparse_vector_scale_test_float("SSE float");
SparseVectorScaleTest<tags::CPU::SSE, double> sse_sparse_vector_scale_test_double("SSE double");
#endif
#ifdef HONEI_CELL
SparseVectorScaleTest<tags::Cell, float> cell_scalar_sparse_vector_product_test_float("Cell float");
#endif

template <typename Tag_, typename DataType_>
class SparseVectorScaleQuickTest :
    public QuickTest
{
    public:
        SparseVectorScaleQuickTest(const std::string & type) :
            QuickTest("sparse_vector_scale_quick_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            unsigned long size(111);
            SparseVector<DataType_> sv1(size, size / 8 + 1);
            for (typename SparseVector<DataType_>::ElementIterator i(sv1.begin_elements()), i_end(sv1.end_elements()) ;
                    i != i_end ; ++i)
            {
                if (i.index() % 10 == 0) *i = 3;
            }
            Scale<Tag_>::value(sv1, DataType_(2));
            DataType_ v1(Norm<vnt_l_one>::value(sv1));
            TEST_CHECK_EQUAL(v1, 6 * (size / 10 + 1));
        }
};

SparseVectorScaleQuickTest<tags::CPU, float> sparse_vector_scale_quick_test_float("float");
SparseVectorScaleQuickTest<tags::CPU, double> sparse_vector_scale_quick_test_double("double");
//SparseVectorScaleQuickTest<tags::CPU::MultiCore, float> mc_sparse_vector_scale_quick_test_float("MC float");
//SparseVectorScaleQuickTest<tags::CPU::MultiCore, double> mc_sparse_vector_scale_quick_test_double("MC double");
#ifdef HONEI_SSE
//SparseVectorScaleQuickTest<tags::CPU::MultiCore::SSE, float> mc_sse_sparse_vector_scale_quick_test_float("MC SSE float");
//SparseVectorScaleQuickTest<tags::CPU::MultiCore::SSE, double> mc_sse_sparse_vector_scale_quick_test_double("MC SSE double");
SparseVectorScaleQuickTest<tags::CPU::SSE, float> sse_sparse_vector_scale_quick_test_float("SSE float");
SparseVectorScaleQuickTest<tags::CPU::SSE, double> sse_sparse_vector_scale_quick_test_double("SSE double");
#endif
#ifdef HONEI_CELL
SparseVectorScaleQuickTest<tags::Cell, float> cell_scalar_sparse_vector_product_quick_test_float("Cell float");
#endif

template <typename Tag_, typename DataType_>
class DenseVectorRangeScaleTest :
    public BaseTest
{
    public:
        DenseVectorRangeScaleTest(const std::string & type) :
            BaseTest("dense_vector_range_scale_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            for (unsigned long size(10) ; size < (1 << 8) ; size <<= 1)
            {
                for (int i(0) ; i < 4 ; ++i)
                {
                    DenseVector<DataType_> source(size + 3, DataType_(3));
                    DenseVectorRange<DataType_> dvr(source, size, i);
                    Scale<Tag_>::value(dvr, DataType_(2));
                    dvr.lock(lm_read_only);
                    DataType_ rslt(Norm<vnt_l_one>::value(dvr));
                    dvr.unlock(lm_read_only);
                    TEST_CHECK_EQUAL(rslt, 6 * size);
                }
            }
        }
};

DenseVectorRangeScaleTest<tags::CPU, float> dense_vector_range_scale_test_float("float");
DenseVectorRangeScaleTest<tags::CPU, double> dense_vector_range_scale_test_double("double");
DenseVectorRangeScaleTest<tags::CPU::MultiCore, float> mc_dense_vector_range_scale_test_float("MC float");
DenseVectorRangeScaleTest<tags::CPU::MultiCore, double> mc_dense_vector_range_scale_test_double("MC double");
#ifdef HONEI_SSE
DenseVectorRangeScaleTest<tags::CPU::SSE, float> sse_dense_vector_range_scale_test_float("sse float");
DenseVectorRangeScaleTest<tags::CPU::SSE, double> sse_dense_vector_range_scale_test_double("sse double");
DenseVectorRangeScaleTest<tags::CPU::MultiCore::SSE, float> mc_sse_dense_vector_range_scale_test_float("MC SSE float");
DenseVectorRangeScaleTest<tags::CPU::MultiCore::SSE, double> mc_sse_dense_vector_range_scale_test_double("MC SSE double");
#endif
#ifdef HONEI_CUDA
DenseVectorRangeScaleTest<tags::GPU::CUDA, float> cuda_dense_vector_range_scale_test_float("float");
DenseVectorRangeScaleTest<tags::GPU::MultiCore::CUDA, float> mc_cuda_dense_vector_range_scale_test_float("float");
#ifdef HONEI_CUDA_DOUBLE
DenseVectorRangeScaleTest<tags::GPU::CUDA, double> cuda_dense_vector_range_scale_test_double("double");
DenseVectorRangeScaleTest<tags::GPU::MultiCore::CUDA, double> mc_cuda_dense_vector_range_scale_test_double("double");
#endif
#endif
#ifdef HONEI_CELL
DenseVectorRangeScaleTest<tags::Cell, float> cell_dense_vector_range_scale_test_float("Cell float");
DenseVectorRangeScaleTest<tags::Cell, double> cell_dense_vector_range_scale_test_double("Cell double");
#endif
#ifdef HONEI_OPENCL
DenseVectorRangeScaleTest<tags::OpenCL::CPU, float> ocl_cpu_dense_vector_range_scale_test_float("float");
DenseVectorRangeScaleTest<tags::OpenCL::CPU, double> ocl_cpu_dense_vector_range_scale_test_double("double");
DenseVectorRangeScaleTest<tags::OpenCL::GPU, float> ocl_gpu_dense_vector_range_scale_test_float("float");
#ifdef HONEI_CUDA_DOUBLE
DenseVectorRangeScaleTest<tags::OpenCL::GPU, double> ocl_gpu_dense_vector_range_scale_test_double("double");
#endif
#endif

template <typename Tag_, typename DataType_>
class DenseVectorRangeScaleQuickTest :
    public QuickTest
{
    public:
        DenseVectorRangeScaleQuickTest(const std::string & type) :
            QuickTest("dense_vector_range_scale_quick_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            unsigned long size(4711);
            for (int i(0) ; i < 4 ; ++i)
            {
                DenseVector<DataType_> source(size + 3, DataType_(3));
                DenseVectorRange<DataType_> dvr(source, size, i);
                Scale<Tag_>::value(dvr, DataType_(2));
                dvr.lock(lm_read_only);
                DataType_ rslt(Norm<vnt_l_one>::value(dvr));
                dvr.unlock(lm_read_only);
                TEST_CHECK_EQUAL(rslt, 6 * size);
            }
        }
};

DenseVectorRangeScaleQuickTest<tags::CPU, float> dense_vector_range_scale_quick_test_float("float");
DenseVectorRangeScaleQuickTest<tags::CPU, double> dense_vector_range_scale_quick_test_double("double");
DenseVectorRangeScaleQuickTest<tags::CPU::MultiCore, float> mc_dense_vector_range_scale_quick_test_float("MC float");
DenseVectorRangeScaleQuickTest<tags::CPU::MultiCore, double> mc_dense_vector_range_scale_quick_test_double("MC double");
#ifdef HONEI_SSE
DenseVectorRangeScaleQuickTest<tags::CPU::SSE, float> sse_dense_vector_range_scale_quick_test_float("sse float");
DenseVectorRangeScaleQuickTest<tags::CPU::SSE, double> sse_dense_vector_range_scale_quick_test_double("sse double");
DenseVectorRangeScaleQuickTest<tags::CPU::MultiCore::SSE, float> mc_sse_dense_vector_range_scale_quick_test_float("MC SSE float");
DenseVectorRangeScaleQuickTest<tags::CPU::MultiCore::SSE, double> mc_sse_dense_vector_range_scale_quick_test_double("MC SSE double");
#endif
#ifdef HONEI_CUDA
DenseVectorRangeScaleQuickTest<tags::GPU::CUDA, float> cuda_dense_vector_range_scale_quick_test_float("float");
DenseVectorRangeScaleQuickTest<tags::GPU::MultiCore::CUDA, float> mc_cuda_dense_vector_range_scale_quick_test_float("float");
#ifdef HONEI_CUDA_DOUBLE
DenseVectorRangeScaleQuickTest<tags::GPU::CUDA, double> cuda_dense_vector_range_scale_quick_test_double("double");
DenseVectorRangeScaleQuickTest<tags::GPU::MultiCore::CUDA, double> mc_cuda_dense_vector_range_scale_quick_test_double("double");
#endif
#endif
#ifdef HONEI_CELL
DenseVectorRangeScaleQuickTest<tags::Cell, float> cell_dense_vector_range_scale_quick_test_float("Cell float");
DenseVectorRangeScaleQuickTest<tags::Cell, double> cell_dense_vector_range_scale_quick_test_double("Cell double");
#endif
#ifdef HONEI_OPENCL
DenseVectorRangeScaleQuickTest<tags::OpenCL::CPU, float> ocl_cpu_dense_vector_range_scale_quick_test_float("float");
DenseVectorRangeScaleQuickTest<tags::OpenCL::CPU, double> ocl_cpu_dense_vector_range_scale_quick_test_double("double");
DenseVectorRangeScaleQuickTest<tags::OpenCL::GPU, float> ocl_gpu_dense_vector_range_scale_quick_test_float("float");
#ifdef HONEI_CUDA_DOUBLE
DenseVectorRangeScaleQuickTest<tags::OpenCL::GPU, double> ocl_gpu_dense_vector_range_scale_quick_test_double("double");
#endif
#endif
