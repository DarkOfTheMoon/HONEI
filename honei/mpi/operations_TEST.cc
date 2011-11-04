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
#include <honei/util/unittest.hh>
#include <honei/la/scaled_sum.hh>
#include <honei/la/dot_product.hh>
#include <honei/la/product.hh>
#include <honei/math/matrix_io.hh>
#include <honei/math/vector_io.hh>
#include <honei/util/time_stamp.hh>

#include <string>
#include <limits>
#include <cmath>
#include <iostream>


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
            mpi::mpi_init();
            int rank;
            mpi::mpi_comm_rank(&rank);
            int comm_size;
            mpi::mpi_comm_size(&comm_size);

            DenseVector<DT_> rs(4711, DT_(42));
            DenseVector<DT_> xs(4711);
            DenseVector<DT_> ys(4711);
            for (unsigned long i(0) ; i < rs.size() ; ++i)
            {
                xs[i] = DT_(i) + 10;
                ys[i] = DT_(i) - 5;
            }

            DenseVectorMPI<DT_> r(rs, rank, comm_size);
            DenseVectorMPI<DT_> x(xs, rank, comm_size);
            DenseVectorMPI<DT_> y(ys, rank, comm_size);


            ScaledSum<tags::CPU>::value(r, x, y, DT_(5));
            ScaledSum<tags::CPU>::value(rs, xs, ys, DT_(5));

            for (unsigned long i(0) ; i < r.size() ; ++i)
                TEST_CHECK_EQUAL(r[i], rs[i + r.offset()]);
        }
};
ScaledSumMPITest<tags::CPU, double> scaled_sum_mpi_test_double("double");

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
            int rank;
            mpi::mpi_comm_rank(&rank);
            int comm_size;
            mpi::mpi_comm_size(&comm_size);

            DenseVector<DT_> xs(4711);
            DenseVector<DT_> ys(4711);
            for (unsigned long i(0) ; i < xs.size() ; ++i)
            {
                xs[i] = DT_(i) + 10;
                ys[i] = DT_(i) - 5;
            }

            DenseVectorMPI<DT_> x(xs, rank, comm_size);
            DenseVectorMPI<DT_> y(ys, rank, comm_size);


            DT_ r = DotProduct<tags::CPU>::value(x, y);
            DT_ rs = DotProduct<tags::CPU>::value(xs, ys);

            TEST_CHECK_EQUAL(r, rs);
        }
};
DotProductMPITest<tags::CPU, double> dot_product_mpi_test_double("double");

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
            int rank;
            mpi::mpi_comm_rank(&rank);
            int comm_size;
            mpi::mpi_comm_size(&comm_size);


            std::string dir(HONEI_SOURCEDIR);
            std::string file (dir + "/honei/math/testdata/poisson_advanced2/q2_sort_2/");
            file += "A_4";
            file += ".ell";
            SparseMatrixELL<DT_> aell(MatrixIO<io_formats::ELL>::read_matrix(file, DT_(0)));

            SparseMatrix<DT_> as(aell);
            SparseMatrixELLMPI<DT_> a(as, rank, comm_size);

            DenseVector<DT_> rs(aell.rows());
            DenseVector<DT_> xs(aell.rows());
            for (unsigned long i(0) ; i < xs.size() ; ++i)
            {
                xs[i] = DT_(i) + 10;
            }

            DenseVectorMPI<DT_> r(rs, rank, comm_size);
            DenseVectorMPI<DT_> x(xs, rank, comm_size);

            TimeStamp at, bt, ct, dt;
            at.take();
            Product<Tag_>::value(rs, aell, xs);
            bt.take();
            //Product<Tag_>::value(rs, aell, xs);
            MPI_Barrier(MPI_COMM_WORLD);
            ct.take();
            Product<Tag_>::value(r, a, x);
            dt.take();
            std::cout<<bt.total()-at.total()<<" "<<dt.total()-ct.total()<<std::endl;
            //Product<Tag_>::value(r, a, x);


            for (unsigned long i(0) ; i < r.size() ; ++i)
                TEST_CHECK_EQUAL(r[i], rs[i + r.offset()]);

            mpi::mpi_finalize();
        }
};
SPMVMPITest<tags::CPU::SSE, double> spmv_mpi_test_double("double");