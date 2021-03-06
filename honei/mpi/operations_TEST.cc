/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2011 Dirk Ribbrock <dirk.ribbrock@uni-dortmund.de>
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

#include <honei/la/dense_vector.hh>
#include <honei/mpi/dense_vector_mpi.hh>
#include <honei/backends/mpi/operations.hh>
#include <honei/mpi/dense_vector_mpi.hh>
#include <honei/mpi/sparse_matrix_ell_mpi.hh>
#include <honei/util/unittest.hh>
#include <honei/la/scaled_sum.hh>
#include <honei/la/sum.hh>
#include <honei/la/difference.hh>
#include <honei/la/dot_product.hh>
#include <honei/la/norm.hh>
#include <honei/la/element_product.hh>
#include <honei/la/product.hh>
#include <honei/math/defect.hh>
#include <honei/math/transposition.hh>
#include <honei/math/matrix_io.hh>
#include <honei/math/vector_io.hh>
#include <honei/util/time_stamp.hh>

#include <string>
#include <limits>
#include <cmath>
#include <iostream>

#ifdef HONEI_CUDA
#include <honei/backends/cuda/gpu_pool.hh>
#endif

using namespace honei;
using namespace tests;


template <typename Tag_, typename DT_>
class ScaledSumMPITest :
    public BaseTest
{
    public:
        ScaledSumMPITest(const std::string & type) :
            BaseTest("scaled_sum_mpi_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
#ifdef HONEI_CUDA
            //cuda::GPUPool::instance()->single_start(mpi::mpi_comm_rank());
#endif

            DenseVector<DT_> rs(4711, DT_(42));
            DenseVector<DT_> xs(4711);
            DenseVector<DT_> ys(4711);
            for (unsigned long i(0) ; i < rs.size() ; ++i)
            {
                xs[i] = DT_(i) + 10;
                ys[i] = DT_(i) - 5;
            }

            DenseVectorMPI<DT_> r(rs);
            DenseVectorMPI<DT_> x(xs);
            DenseVectorMPI<DT_> y(ys);


            r.lock(lm_read_only);
            rs.lock(lm_read_only);
            rs.unlock(lm_read_only);
            r.unlock(lm_read_only);
            ScaledSum<Tag_>::value(r, x, y, DT_(5));
            ScaledSum<Tag_>::value(rs, xs, ys, DT_(5));

            for (unsigned long i(0) ; i < r.local_size() ; ++i)
                TEST_CHECK_EQUAL(r[i], rs[i + r.offset()]);
        }
};
ScaledSumMPITest<tags::CPU::Generic, double> generic_scaled_sum_mpi_test_double("double");
#ifdef HONEI_SSE
ScaledSumMPITest<tags::CPU::SSE, double> scaled_sum_mpi_test_double("double");
#else
ScaledSumMPITest<tags::CPU, double> scaled_sum_mpi_test_double("double");
#endif
#ifdef HONEI_CUDA
#ifdef HONEI_CUDA_DOUBLE
ScaledSumMPITest<tags::GPU::CUDA, double> cuda_scaled_sum_mpi_test_double("double");
#endif
#endif

template <typename Tag_, typename DT_>
class ScaleMPITest :
    public BaseTest
{
    public:
        ScaleMPITest(const std::string & type) :
            BaseTest("scale_mpi_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            DenseVector<DT_> xs(4711);
            for (unsigned long i(0) ; i < xs.size() ; ++i)
            {
                xs[i] = DT_(i) + 10;
            }

            DenseVectorMPI<DT_> x(xs);


            x.lock(lm_read_only);
            xs.lock(lm_read_only);
            xs.unlock(lm_read_only);
            x.unlock(lm_read_only);
            Scale<Tag_>::value(x, 3.12);
            Scale<Tag_>::value(xs, 3.12);

            for (unsigned long i(0) ; i < x.local_size() ; ++i)
                TEST_CHECK_EQUAL(x[i], xs[i + x.offset()]);
        }
};
ScaleMPITest<tags::CPU::Generic, double> generic_scale_mpi_test_double("double");
#ifdef HONEI_SSE
ScaleMPITest<tags::CPU::SSE, double> scale_mpi_test_double("double");
#else
ScaleMPITest<tags::CPU, double> scale_mpi_test_double("double");
#endif
#ifdef HONEI_CUDA
#ifdef HONEI_CUDA_DOUBLE
#endif
ScaleMPITest<tags::GPU::CUDA, double> cuda_scale_mpi_test_double("double");
#endif

template <typename Tag_, typename DT_>
class SumMPITest :
    public BaseTest
{
    public:
        SumMPITest(const std::string & type) :
            BaseTest("sum_mpi_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            DenseVector<DT_> xs(4711);
            DenseVector<DT_> ys(4711);
            for (unsigned long i(0) ; i < xs.size() ; ++i)
            {
                xs[i] = DT_(i) + 10;
                ys[i] = DT_(i) - 5;
            }

            DenseVectorMPI<DT_> x(xs);
            DenseVectorMPI<DT_> y(ys);


            Sum<Tag_>::value(x, y);
            Sum<Tag_>::value(xs, ys);

            x.lock(lm_read_only);
            xs.lock(lm_read_only);
            xs.unlock(lm_read_only);
            x.unlock(lm_read_only);
            for (unsigned long i(0) ; i < x.local_size() ; ++i)
                TEST_CHECK_EQUAL(x[i], xs[i + x.offset()]);
        }
};
SumMPITest<tags::CPU::Generic, double> generic_sum_mpi_test_double("double");
#ifdef HONEI_SSE
SumMPITest<tags::CPU::SSE, double> sum_mpi_test_double("double");
#else
SumMPITest<tags::CPU, double> sum_mpi_test_double("double");
#endif
#ifdef HONEI_CUDA
#ifdef HONEI_CUDA_DOUBLE
SumMPITest<tags::GPU::CUDA, double> cuda_sum_mpi_test_double("double");
#endif
#endif

template <typename Tag_, typename DT_>
class DifferenceMPITest :
    public BaseTest
{
    public:
        DifferenceMPITest(const std::string & type) :
            BaseTest("difference_mpi_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            DenseVector<DT_> rs(4711, DT_(42));
            DenseVector<DT_> xs(4711);
            DenseVector<DT_> ys(4711);
            for (unsigned long i(0) ; i < rs.size() ; ++i)
            {
                xs[i] = DT_(i) + 10;
                ys[i] = DT_(i) - 5;
            }

            DenseVectorMPI<DT_> r(rs);
            DenseVectorMPI<DT_> x(xs);
            DenseVectorMPI<DT_> y(ys);


            Difference<Tag_>::value(r, x, y);
            Difference<Tag_>::value(rs, xs, ys);

            r.lock(lm_read_only);
            rs.lock(lm_read_only);
            r.unlock(lm_read_only);
            rs.unlock(lm_read_only);
            for (unsigned long i(0) ; i < r.local_size() ; ++i)
                TEST_CHECK_EQUAL(r[i], rs[i + r.offset()]);
        }
};
DifferenceMPITest<tags::CPU::Generic, double> generic_difference_mpi_test_double("double");
#ifdef HONEI_SSE
DifferenceMPITest<tags::CPU::SSE, double> difference_mpi_test_double("double");
#else
DifferenceMPITest<tags::CPU, double> difference_mpi_test_double("double");
#endif
#ifdef HONEI_CUDA
#ifdef HONEI_CUDA_DOUBLE
DifferenceMPITest<tags::GPU::CUDA, double> cuda_difference_mpi_test_double("double");
#endif
#endif

template <typename Tag_, typename DT_>
class ElementProductMPITest :
    public BaseTest
{
    public:
        ElementProductMPITest(const std::string & type) :
            BaseTest("element_product_mpi_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            DenseVector<DT_> rs(4711);
            DenseVector<DT_> xs(4711);
            DenseVector<DT_> ys(4711);
            for (unsigned long i(0) ; i < xs.size() ; ++i)
            {
                xs[i] = DT_(i) + 10;
                ys[i] = DT_(i) - 5;
            }

            DenseVectorMPI<DT_> r(rs);
            DenseVectorMPI<DT_> x(xs);
            DenseVectorMPI<DT_> y(ys);


            r.lock(lm_read_only);
            rs.lock(lm_read_only);
            r.unlock(lm_read_only);
            rs.unlock(lm_read_only);
            ElementProduct<Tag_>::value(r, x, y);
            ElementProduct<Tag_>::value(rs, xs, ys);

            for (unsigned long i(0) ; i < x.local_size() ; ++i)
                TEST_CHECK_EQUAL(r[i], rs[i + x.offset()]);
        }
};
ElementProductMPITest<tags::CPU::Generic, double> generic_element_product_mpi_test_double("double");
#ifdef HONEI_SSE
ElementProductMPITest<tags::CPU::SSE, double> element_product_mpi_test_double("double");
#else
ElementProductMPITest<tags::CPU, double> element_product_mpi_test_double("double");
#endif
#ifdef HONEI_CUDA
#ifdef HONEI_CUDA_DOUBLE
ElementProductMPITest<tags::GPU::CUDA, double> cuda_element_product_mpi_test_double("double");
#endif
#endif

template <typename Tag_, typename DT_>
class DotProductMPITest :
    public BaseTest
{
    public:
        DotProductMPITest(const std::string & type) :
            BaseTest("dot_product_mpi_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            DenseVector<DT_> xs(4711);
            DenseVector<DT_> ys(4711);
            for (unsigned long i(0) ; i < xs.size() ; ++i)
            {
                xs[i] = DT_(i) + 10;
                ys[i] = DT_(i) - 5;
            }

            DenseVectorMPI<DT_> x(xs);
            DenseVectorMPI<DT_> y(ys);


            DT_ r = DotProduct<Tag_>::value(x, y);
            DT_ rs = DotProduct<Tag_>::value(xs, ys);

            TEST_CHECK_EQUAL(r, rs);
        }
};
DotProductMPITest<tags::CPU::Generic, double> generic_dot_product_mpi_test_double("double");
#ifdef HONEI_SSE
DotProductMPITest<tags::CPU::SSE, double> dot_product_mpi_test_double("double");
#else
DotProductMPITest<tags::CPU, double> dot_product_mpi_test_double("double");
#endif
#ifdef HONEI_CUDA
#ifdef HONEI_CUDA_DOUBLE
DotProductMPITest<tags::GPU::CUDA, double> cuda_dot_product_mpi_test_double("double");
#endif
#endif

template <typename Tag_, typename DT_>
class NormMPITest :
    public BaseTest
{
    public:
        NormMPITest(const std::string & type) :
            BaseTest("norm_mpi_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            DenseVector<DT_> xs(471);
            for (unsigned long i(0) ; i < xs.size() ; ++i)
            {
                xs[i] = DT_(i) / xs.size();
            }

            DenseVectorMPI<DT_> x(xs);


            DT_ r = Norm<vnt_l_two, false, Tag_>::value(x);
            DT_ rs = Norm<vnt_l_two, false, Tag_>::value(xs);
            TEST_CHECK_EQUAL_WITHIN_EPS(r, rs, 1e-11);

            r = Norm<vnt_l_two, true, Tag_>::value(x);
            rs = Norm<vnt_l_two, true, Tag_>::value(xs);
            TEST_CHECK_EQUAL_WITHIN_EPS(r, rs, 1e-11);
        }
};
NormMPITest<tags::CPU::Generic, double> generic_norm_mpi_test_double("double");
#ifdef HONEI_SSE
NormMPITest<tags::CPU::SSE, double> norm_mpi_test_double("double");
#else
NormMPITest<tags::CPU, double> norm_mpi_test_double("double");
#endif
#ifdef HONEI_CUDA
#ifdef HONEI_CUDA_DOUBLE
NormMPITest<tags::GPU::CUDA, double> cuda_norm_mpi_test_double("double");
#endif
#endif

template <typename Tag_, typename DT_>
class TranspositionMPITest :
    public BaseTest
{
    public:
        TranspositionMPITest(const std::string & type) :
            BaseTest("transposition_mpi_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            std::string dir(HONEI_SOURCEDIR);
            std::string file (dir + "/honei/math/testdata/poisson_advanced2/sort_0/");
            file += "prol_2";
            file += ".ell";
            SparseMatrixELL<DT_> aell(MatrixIO<io_formats::ELL>::read_matrix(file, DT_(0)));

            SparseMatrixELLMPI<DT_> a(aell);

            SparseMatrixELL<DT_> ref_result(Transposition<tags::CPU>::value(aell));
            SparseMatrixELLMPI<DT_> result(Transposition<tags::CPU>::value(a));

            for (unsigned long i(0) ; i < result.local_rows() ; ++i)
                for (unsigned long j(0) ; j < result.columns() ; ++j)
                {
                    TEST_CHECK_EQUAL(result(i, j), ref_result(i + result.offset(), j));
                }

        }
};
TranspositionMPITest<tags::CPU, double> generic_transposition_mpi_test_double("double");

template <typename Tag_, typename DT_>
class DefectMPITest :
    public BaseTest
{
    public:
        DefectMPITest(const std::string & type) :
            BaseTest("defect_mpi_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            std::string dir(HONEI_SOURCEDIR);
            std::string file (dir + "/honei/math/testdata/poisson_advanced2/sort_0/");
            file += "A_3";
            file += ".ell";
            SparseMatrixELL<DT_> aell(MatrixIO<io_formats::ELL>::read_matrix(file, DT_(0)));

            SparseMatrix<DT_> as(aell);
            SparseMatrixELLMPI<DT_> a(as);

            DenseVector<DT_> rs(aell.rows());
            DenseVector<DT_> xs(aell.rows());
            DenseVector<DT_> bs(aell.rows());
            for (unsigned long i(0) ; i < xs.size() ; ++i)
            {
                xs[i] = DT_(i) + 10;
                bs[i] = DT_(i) + i/2;
            }

            DenseVectorMPI<DT_> r(rs);
            DenseVectorMPI<DT_> x(xs);
            DenseVectorMPI<DT_> b(bs);

            Defect<Tag_>::value(rs, bs, aell, xs);
            Defect<Tag_>::value(rs, bs, aell, xs);
            Defect<Tag_>::value(r, b, a, x);
            Defect<Tag_>::value(r, b, a, x);

            rs.lock(lm_read_only);
            r.lock(lm_read_only);
            rs.lock(lm_read_only);
            rs.unlock(lm_read_only);
            for (unsigned long i(0) ; i < r.local_size() ; ++i)
                TEST_CHECK_EQUAL_WITHIN_EPS(r[i], rs[i + r.offset()], 1e-11);
        }
};
DefectMPITest<tags::CPU::Generic, double> generic_defect_mpi_test_double("double");
#ifdef HONEI_SSE
DefectMPITest<tags::CPU::SSE, double> defect_mpi_test_double("double");
#else
DefectMPITest<tags::CPU, double> defect_mpi_test_double("double");
#endif
#ifdef HONEI_CUDA
#ifdef HONEI_CUDA_DOUBLE
DefectMPITest<tags::GPU::CUDA, double> cuda_defect_mpi_test_double("double");
#endif
#endif

template <typename Tag_, typename DT_>
class SPMVMPITest :
    public BaseTest
{
    public:
        SPMVMPITest(const std::string & type) :
            BaseTest("spmv_mpi_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            MPI_Barrier(MPI_COMM_WORLD);
            std::string dir(HONEI_SOURCEDIR);
            std::string file (dir + "/honei/math/testdata/poisson_advanced4/sort_0/");
            file += "prol_3";
            file += ".ell";
            SparseMatrixELL<DT_> aell(MatrixIO<io_formats::ELL>::read_matrix(file, DT_(0)));

            SparseMatrix<DT_> as(aell);
            SparseMatrixELLMPI<DT_> a(as);

            DenseVector<DT_> rs(aell.rows());
            DenseVector<DT_> xs(aell.columns());
            for (unsigned long i(0) ; i < xs.size() ; ++i)
            {
                xs[i] = DT_(i) + 10;
            }

            DenseVectorMPI<DT_> r(rs);
            DenseVectorMPI<DT_> x(xs);

            TimeStamp at, bt, ct, dt;
            Product<Tag_>::value(rs, aell, xs);
            MPI_Barrier(MPI_COMM_WORLD);
            at.take();
            for (unsigned long i(0) ; i < 100 ; ++i)
            {
                Product<Tag_>::value(rs, aell, xs);
            }
#ifdef HONEI_CUDA
            cuda::GPUPool::instance()->flush();
#endif
            bt.take();
            Product<Tag_>::value(r, a, x);
            MPI_Barrier(MPI_COMM_WORLD);
            ct.take();
            for (unsigned long i(0) ; i < 100 ; ++i)
            {
                Product<Tag_>::value(r, a, x);
            }
#ifdef HONEI_CUDA
            cuda::GPUPool::instance()->flush();
#endif
            dt.take();
            MPI_Barrier(MPI_COMM_WORLD);
            std::cout<<"serial: "<<bt.total()-at.total()<<" parallel: "<<dt.total()-ct.total()<<std::endl;

            r.lock(lm_read_only);
            r.unlock(lm_read_only);
            rs.lock(lm_read_only);
            rs.unlock(lm_read_only);
            for (unsigned long i(0) ; i < r.local_size() ; ++i)
            {
                TEST_CHECK_EQUAL_WITHIN_EPS(r[i], rs[i + r.offset()], 1e-9);
            }
        }
};
SPMVMPITest<tags::CPU::Generic, double> generic_spmv_mpi_test_double("double");
SPMVMPITest<tags::CPU::MultiCore::Generic, double> mc_generic_spmv_mpi_test_double("double");
#ifdef HONEI_SSE
SPMVMPITest<tags::CPU::SSE, double> spmv_mpi_test_double("double");
SPMVMPITest<tags::CPU::MultiCore::SSE, double> mv_spmv_mpi_test_double("double");
#else
SPMVMPITest<tags::CPU, double> spmv_mpi_test_double("double");
SPMVMPITest<tags::CPU::MultiCore, double> mc_spmv_mpi_test_double("double");
#endif
#ifdef HONEI_CUDA
#ifdef HONEI_CUDA_DOUBLE
#endif
SPMVMPITest<tags::GPU::CUDA, double> cuda_spmv_mpi_test_double("double");
#endif
