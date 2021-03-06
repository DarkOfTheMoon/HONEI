/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2007 Markus Geveler <apryde@gmx.de>
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

#define SOLVER_VERBOSE_L2

#include <honei/math/jacobi.hh>
#include <honei/math/matrix_io.hh>
#include <honei/util/unittest.hh>
#include <honei/util/stringify.hh>
#include <iostream>
#include <honei/math/matrix_io.hh>
#include <honei/math/vector_io.hh>

using namespace honei;
using namespace tests;
using namespace std;

template <typename Tag_, typename DT1_>
class JacobiTestDense:
    public BaseTest
{
    public:
        JacobiTestDense(const std::string & tag) :
            BaseTest("Jacobi solver test (Dense system)<" + tag + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            DenseMatrix<DT1_> A(3, 3, DT1_(1));
            DenseVector<DT1_> b(3, DT1_(1));
            A[0][0] = DT1_(7);
            A[0][1] = DT1_(-2);
            A[0][2] = DT1_(0);
            A[1][0] = DT1_(-2);
            A[1][1] = DT1_(6);
            A[1][2] = DT1_(2);
            A[2][0] = DT1_(0);
            A[2][1] = DT1_(2);
            A[2][2] = DT1_(5);

            b[0] = DT1_(3);
            b[1] = DT1_(3);
            b[2] = DT1_(0);

            std::cout<<"A:"<<A<<std::endl;
            std::cout<<"b:"<<b<<std::endl;
            DenseVector<DT1_> result(3, DT1_(0));
            Jacobi<Tag_>::value(A, b, result, 12ul);
            DT1_ x_n = Norm< vnt_l_two, false, Tag_>::value(result);
            DenseVector<DT1_> x_analytical(3, DT1_(0));
            std::cout << "RESULT(v1):" << result << std::endl;

            x_analytical[0] = DT1_(2./3.);
            x_analytical[1] = DT1_(5./6.);
            x_analytical[2] = DT1_(-1./3.);
            DT1_ x_analytical_n = Norm< vnt_l_two, false, Tag_>::value(x_analytical);
            TEST_CHECK_EQUAL_WITHIN_EPS(x_analytical_n, x_n , double(0.001));

            DenseVector<DT1_> result_2(3, DT1_(0));
            Jacobi<Tag_>::value(A, b, result_2, double(std::numeric_limits<double>::epsilon()));
            std::cout << "RESULT(v2):" << result_2 << std::endl;

            DT1_ x_n_2 = Norm< vnt_l_two, false, Tag_>::value(result_2);
            TEST_CHECK_EQUAL_WITHIN_EPS(x_analytical_n, x_n_2 , double(0.001));

        }
};

template <typename Tag_, typename DT1_>
class JacobiTestBanded:
    public BaseTest
{
    public:
        JacobiTestBanded(const std::string & tag) :
            BaseTest("Jacobi solver test (Banded system)<" + tag + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            DenseVector<DT1_> diag(3, DT1_(1));
            DenseVector<DT1_> bp1(3, DT1_(1));
            DenseVector<DT1_> bp2(3, DT1_(1));
            DenseVector<DT1_> bm1(3, DT1_(1));
            DenseVector<DT1_> bm2(3, DT1_(1));

            diag[0] = DT1_(7);
            diag[1] = DT1_(6);
            diag[2] = DT1_(5);
            bp1[0] = DT1_(-2);
            bp1[1] = DT1_(2);
            bp1[2] = DT1_(0);
            bp2[0] = DT1_(0);
            bp2[1] = DT1_(0);
            bp2[2] = DT1_(0);
            bm1[0] = DT1_(0);
            bm1[1] = DT1_(-2);
            bm1[2] = DT1_(2);
            bm2[0] = DT1_(0);
            bm2[1] = DT1_(0);
            bm2[2] = DT1_(0);

            BandedMatrix<DT1_> A(3);
            A.insert_band(0, diag);
            A.insert_band(1, bp1);
            A.insert_band(2, bp2);
            A.insert_band(-1, bm1);
            A.insert_band(-2, bm2);

            DenseVector<DT1_> b(3, DT1_(1));
            b[0] = DT1_(3);
            b[1] = DT1_(3);
            b[2] = DT1_(0);

            std::cout << "A:" << A << std::endl;
            std::cout << "b:" << b << std::endl;

            DenseVector<DT1_> result(3, DT1_(0));
            Jacobi<Tag_>::value(A, b, result, 10ul);

            DT1_ x_n = Norm< vnt_l_two, false, Tag_>::value(result);
            DenseVector<DT1_> x_analytical(3, DT1_(1));
            x_analytical[0] = DT1_(2./3.);
            x_analytical[1] = DT1_(5./6.);
            x_analytical[2] = DT1_(-1./3.);
            DT1_ x_analytical_n = Norm< vnt_l_two, false, Tag_>::value(x_analytical);

            std::cout << "RESULT(v1):" << result << std::endl;
            TEST_CHECK_EQUAL_WITHIN_EPS(x_analytical_n, x_n , double(0.1));

            DenseVector<DT1_> result_2(3, DT1_(0));
            Jacobi<Tag_>::value(A, b, result_2, double(std::numeric_limits<double>::epsilon()));
            DT1_ x_n_2 = Norm< vnt_l_two, false, Tag_>::value(result_2);
            TEST_CHECK_EQUAL_WITHIN_EPS(x_analytical_n, x_n_2 , double(0.1));

            std::cout << "RESULT(v2):" << result_2 << std::endl;

        }
};
template <typename Tag_, typename DT1_>
class JacobiTestSparse:
    public BaseTest
{
    public:
        JacobiTestSparse(const std::string & tag) :
            BaseTest("Jacobi solver test (sparse system)<" + tag + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            SparseMatrix<DT1_> A(3, 3);
            DenseVector<DT1_> b(3, DT1_(1));
            A[0][0] = DT1_(7);
            A[0][1] = DT1_(-2);
            A[0][2] = DT1_(0);
            A[1][0] = DT1_(-2);
            A[1][1] = DT1_(6);
            A[1][2] = DT1_(2);
            A[2][0] = DT1_(0);
            A[2][1] = DT1_(2);
            A[2][2] = DT1_(5);

            b[0] = DT1_(3);
            b[1] = DT1_(3);
            b[2] = DT1_(0);

            std::cout << "A:" << A << std::endl;
            std::cout << "b:" << b << std::endl;

            DenseVector<DT1_> result(3, DT1_(0));
            Jacobi<Tag_>::value(A,b, result, 12ul);

            DT1_ x_n = Norm< vnt_l_two, false, Tag_>::value(result);
            DenseVector<DT1_> x_analytical(3, DT1_(0));
            std::cout << "RESULT(v1):" << result << std::endl;

            x_analytical[0] = DT1_(2./3.);
            x_analytical[1] = DT1_(5./6.);
            x_analytical[2] = DT1_(-1./3.);
            DT1_ x_analytical_n = Norm< vnt_l_two, false, Tag_>::value(x_analytical);
            TEST_CHECK_EQUAL_WITHIN_EPS(x_analytical_n, x_n , double(0.001));

            DenseVector<DT1_> result_2(3, DT1_(0));
            Jacobi<Tag_>::value(A, b, result_2, double(std::numeric_limits<double>::epsilon()));
            std::cout << "RESULT(v2):" << result_2 << std::endl;

            DT1_ x_n_2 = Norm< vnt_l_two, false, Tag_>::value(result_2);
            TEST_CHECK_EQUAL_WITHIN_EPS(x_analytical_n, x_n_2 , double(0.001));

        }
};
JacobiTestSparse<tags::CPU, float> jacobi_test_float_sparse("float");
JacobiTestSparse<tags::CPU, double> jacobi_test_double_sparse("double");

JacobiTestDense<tags::CPU, float> jacobi_test_float_dense("float");
JacobiTestDense<tags::CPU, double> jacobi_test_double_dense("double");
JacobiTestBanded<tags::CPU, float> jacobi_test_float_banded("float");
JacobiTestBanded<tags::CPU, double> jacobi_test_double_banded("double");


#ifdef HONEI_SSE
JacobiTestDense<tags::CPU::SSE, float> sse_jacobi_test_float_dense("SSE float");
JacobiTestDense<tags::CPU::SSE, double> sse_jacobi_test_double_dense("SSE double");
JacobiTestBanded<tags::CPU::SSE, float> sse_jacobi_test_float_banded("SSE float");
JacobiTestBanded<tags::CPU::SSE, double> sse_jacobi_test_double_banded("SSE double");

#endif

template <typename Tag_, typename DT1_>
class JacobiTestSparseELL:
    public BaseTest
{
    public:
        JacobiTestSparseELL(const std::string & tag) :
            BaseTest("Jacobi solver test (sparse ELL system)<" + tag + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {

            std::string filename(HONEI_SOURCEDIR);
            filename += "/honei/math/testdata/5pt_10x10.mtx";
            SparseMatrix<DT1_> tsmatrix2(MatrixIO<io_formats::MTX>::read_matrix(filename, DT1_(0)));
            SparseMatrixELL<DT1_> smatrix2(tsmatrix2);

            DenseVector<DT1_> x(tsmatrix2.rows(), DT1_(1.2345));

            DenseVector<DT1_> rhs(x.size(), DT1_(0));

            Product<Tag_>::value(rhs, smatrix2, x);

            DenseVector<DT1_> initial_guess(x.size(), DT1_(1));
            DenseVector<DT1_> diag_inverted(x.size(), DT1_(0));
            for(unsigned long i(0) ; i < diag_inverted.size() ; ++i)
            {
                    diag_inverted[i] = DT1_(1)/tsmatrix2(i, i);
            }

            //Initial defect (for convergence purposes later):
            DenseVector<DT1_> initial_defect(Defect<Tag_>::value(rhs, smatrix2, x));

            DenseVector<DT1_> result(rhs.size(), DT1_(0));
            Jacobi<Tag_>::value(initial_guess, smatrix2, rhs, result, 1000ul, DT1_(0.7), diag_inverted);

            result.lock(lm_read_only);
            x.lock(lm_read_only);
            for(unsigned long i(0) ; i < rhs.size() ; ++i)
            {
                TEST_CHECK_EQUAL_WITHIN_EPS(result[i], x[i], std::numeric_limits<DT1_>::epsilon());
            }
            result.unlock(lm_read_only);
            x.unlock(lm_read_only);
        }
};
JacobiTestSparseELL<tags::CPU, float> jacobi_test_float_sparse_ell("float");
JacobiTestSparseELL<tags::CPU, double> jacobi_test_double_sparse_ell("double");
#ifdef HONEI_SSE
JacobiTestSparseELL<tags::CPU::SSE, float> sse_jacobi_test_float_sparse_ell("float");
JacobiTestSparseELL<tags::CPU::SSE, double> sse_jacobi_test_double_sparse_ell("double");
JacobiTestSparseELL<tags::CPU::MultiCore::SSE, float> mc_sse_jacobi_test_float_sparse_ell("float");
JacobiTestSparseELL<tags::CPU::MultiCore::SSE, double> mc_sse_jacobi_test_double_sparse_ell("double");
#endif
#ifdef HONEI_CUDA
JacobiTestSparseELL<tags::GPU::CUDA, float> cuda_jacobi_test_float_sparse_ell("float");
#ifdef HONEI_CUDA_DOUBLE
JacobiTestSparseELL<tags::GPU::CUDA, double> cuda_jacobi_test_double_sparse_ell("double");
#endif
#endif

template <typename Tag_, typename DT1_>
class JacobiSparseELLComparisonTest:
    public BaseTest
{
    private:
        std::string _m_f, _v_f, _r_f;
    public:
        JacobiSparseELLComparisonTest(const std::string & tag, std::string m_file, std::string v_file, std::string res_file) :
            BaseTest("Jacobi solver test (sparse ELL system)<" + tag + ">")
        {
            register_tag(Tag_::name);
            _m_f = m_file;
            _v_f = v_file;
            _r_f = res_file;
        }

        virtual void run() const
        {
            std::string filename(HONEI_SOURCEDIR);
            filename += "/honei/math/testdata/";
            filename += _m_f;

            SparseMatrix<DT1_> tsmatrix2(MatrixIO<io_formats::M>::read_matrix(filename, DT1_(0)));
            SparseMatrixELL<DT1_> smatrix2(tsmatrix2);

            std::string filename_2(HONEI_SOURCEDIR);
            filename_2 += "/honei/math/testdata/";
            filename_2 += _v_f;
            DenseVector<DT1_> rhs(VectorIO<io_formats::EXP>::read_vector(filename_2, DT1_(0)));

            DenseVector<DT1_> diag_inverted(tsmatrix2.rows(), DT1_(0));
            for(unsigned long i(0) ; i < diag_inverted.size() ; ++i)
            {
                    diag_inverted[i] = DT1_(1)/tsmatrix2(i, i);
            }
            SparseMatrix<DT1_> tdifference(tsmatrix2.copy());
            for(unsigned long i(0) ; i < tdifference.rows() ; ++i)
            {
                     tdifference(i, i) = DT1_(0);
            }
            SparseMatrixELL<DT1_> difference(tdifference);

            DenseVector<DT1_> result(rhs.size(), DT1_(0));
            DenseVector<DT1_> result_c(result.copy());
            Defect<Tag_>::value(result, rhs, smatrix2, result_c);

            unsigned long used_iters;
            Jacobi<Tag_>::value(smatrix2, difference, rhs, result, diag_inverted, 10000ul, used_iters, DT1_(1e-8));

            std::string filename_3(HONEI_SOURCEDIR);
            filename_3 += "/honei/math/testdata/";
            filename_3 += _r_f;
            DenseVector<DT1_> ref_result(VectorIO<io_formats::EXP>::read_vector(filename_3, DT1_(0)));

            result.lock(lm_read_only);
            //std::cout << result << std::endl;
            result.unlock(lm_read_only);

            //TEST_CHECK_EQUAL(result, ref_result);
            for(unsigned long i(0) ; i < result.size() ; ++i)
                TEST_CHECK_EQUAL_WITHIN_EPS(result[i], ref_result[i], 1e-3);
        }
};

JacobiSparseELLComparisonTest<tags::CPU, float> jac_test_float_sparse_ell("float", "l2/area51_full_0.m", "l2/area51_rhs_0", "l2/area51_sol_0");
JacobiSparseELLComparisonTest<tags::CPU, double> jac_test_double_sparse_ell("double", "l2/area51_full_0.m", "l2/area51_rhs_0", "l2/area51_sol_0");
#ifdef HONEI_SSE
JacobiSparseELLComparisonTest<tags::CPU::SSE, float> jac_test_float_sparse_ell_sse("float", "l2/area51_full_0.m", "l2/area51_rhs_0", "l2/area51_sol_0");
JacobiSparseELLComparisonTest<tags::CPU::SSE, double> jac_test_double_sparse_ell_sse("double", "l2/area51_full_0.m", "l2/area51_rhs_0", "l2/area51_sol_0");
JacobiSparseELLComparisonTest<tags::CPU::MultiCore::SSE, float> jac_test_float_sparse_ell_mcsse("float", "l2/area51_full_0.m", "l2/area51_rhs_0", "l2/area51_sol_0");
JacobiSparseELLComparisonTest<tags::CPU::MultiCore::SSE, double> jac_test_double_sparse_ell_mcsse("double", "l2/area51_full_0.m", "l2/area51_rhs_0", "l2/area51_sol_0");
#endif
#ifdef HONEI_CUDA
JacobiSparseELLComparisonTest<tags::GPU::CUDA, float> jac_test_float_sparse_ell_cuda("float", "l2/area51_full_0.m", "l2/area51_rhs_0", "l2/area51_sol_0");
#ifdef HONEI_CUDA_DOUBLE
JacobiSparseELLComparisonTest<tags::GPU::CUDA, double> jac_test_double_sparse_ell_cuda("double", "l2/area51_full_0.m", "l2/area51_rhs_0", "l2/area51_sol_0");
#endif
#endif
