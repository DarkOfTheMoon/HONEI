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

#include <honei/math/matrix_io.hh>
#include <unittest/unittest.hh>
#include <honei/util/stringify.hh>
#include <honei/la/product.hh>
#include <honei/la/sparse_matrix_ell.hh>
#include <iostream>
#include <cstdio>

using namespace honei;
using namespace tests;
using namespace std;

template<typename DT_, typename Tag_>
class MMIOTest:
    public BaseTest
{
    public:
        MMIOTest(const std::string & tag) :
            BaseTest("Matrix Market Format I/O test <" + tag + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            std::string filename(HONEI_SOURCEDIR);
            filename += "/honei/math/testdata/5pt_10x10.mtx";
            unsigned long non_zeros(0);
            DenseMatrix<DT_> matrix = MatrixIO<io_formats::MTX>::read_matrix(filename, DT_(0), non_zeros);
            //std::cout << matrix;
            //std::cout << "Non zero elements: " << non_zeros << std::endl;

            DenseVector<DT_> x(matrix.rows());
            DenseVector<DT_> y(matrix.rows());
            for (unsigned long i(0) ; i < x.size() ; ++i)
            {
                x[i] = DT_(i) / 1.234;
            }
            SparseMatrix<DT_> ssmatrix(matrix);
            SparseMatrixELL<DT_> smatrix(ssmatrix);
            DenseVector<DT_> y1(Product<Tag_>::value(y, smatrix, x));
            DenseVector<DT_> y2(Product<tags::CPU>::value(matrix, x));

            y1.lock(lm_read_only);
            TEST_CHECK_EQUAL(y1, y2);
            y1.unlock(lm_read_only);

            ///Test second read-in method:
            std::cout << "Testing sparse data read-in:" << std::endl;
            unsigned long non_zeros_2(MatrixIO<io_formats::MTX>::get_non_zeros(filename));
            TEST_CHECK_EQUAL(non_zeros, non_zeros_2);
            DenseVector<unsigned long> r(non_zeros_2);
            DenseVector<unsigned long> c(non_zeros_2);
            DenseVector<DT_> data(non_zeros_2);

            MatrixIO<io_formats::MTX>::read_matrix(filename, r, c, data);
            /*std::cout << "Row Indices:" << std::endl;
            std::cout << r;
            std::cout << "Column Indices:" << std::endl;
            std::cout << c;
            std::cout << "Data:" << std::endl;
            std::cout << data;*/

            for(unsigned long i(0) ; i < non_zeros_2 ; ++i)
            {
                TEST_CHECK_EQUAL(matrix[r[i]][c[i]] , data[i]);
            }

            unsigned long rows, columns, ax, bx;
            MatrixIO<io_formats::MTX>::get_sizes(filename, rows, columns, ax, bx);
            SparseMatrix<DT_> tsmatrix2(rows, columns, r, c, data);
            SparseMatrixELL<DT_> smatrix2(tsmatrix2);

            TEST_CHECK_EQUAL(smatrix2, smatrix);

            std::string filename_2(HONEI_SOURCEDIR);
            filename_2 += "/honei/math/testdata/area51_full_0.m";

            unsigned long r_2, c_2, n_z_2, non_data_2;

            MatrixIO<io_formats::M>::get_sizes(filename_2, r_2, c_2, n_z_2, non_data_2);

            //std::cout << r_2 << " " << c_2 << " " << n_z_2 << " " << non_data_2 << std::endl;

            //------------------------------------------------------------------------------
            unsigned long length_matlab(MatrixIO<io_formats::M>::get_non_zeros(filename_2));
            //std::cout << "LENGTH = " << length_matlab << std::endl;
            DenseVector<unsigned long> r_matlab(length_matlab);
            DenseVector<unsigned long> c_matlab(length_matlab);
            DenseVector<DT_> data_matlab(length_matlab);
            MatrixIO<io_formats::M>::read_matrix(filename_2, r_matlab, c_matlab, data_matlab);

            /*std::cout << r_matlab << std::endl;
            std::cout << c_matlab << std::endl;
            std::cout << data_matlab << std::endl;*/

            SparseMatrix<DT_> tsmatrix3(r_2, c_2, r_matlab, c_matlab, data_matlab);
            SparseMatrixELL<DT_> smatrix3(tsmatrix3);

            //std::cout << smatrix3 << std::endl;

            std::string filename_3(HONEI_SOURCEDIR);
            filename_3 +="/honei/math/testdata/area51_full_0.ell";
            SparseMatrixELL<DT_> smatrix4 = MatrixIO<io_formats::ELL>::read_matrix(filename_3, DT_(1));
            TEST_CHECK_EQUAL(smatrix4, smatrix3);

            std::string filename_4(HONEI_SOURCEDIR);
            filename_4 += "/honei/math/testdata/5pt_10x10.ell";
            SparseMatrixELL<DT_> smatrix5 = MatrixIO<io_formats::ELL>::read_matrix(filename_4, DT_(1));
            TEST_CHECK_EQUAL(smatrix5, smatrix2);

            //--------------------- ell write_matrix test
            if (sizeof(DT_) == 8 && sizeof(unsigned long) ==  8)
            {
                std::string filename_6(HONEI_SOURCEDIR);
                filename_6 += "/honei/math/testdata/5pt_10x10-out.ell";
                MatrixIO<io_formats::ELL>::write_matrix(filename_6, smatrix5);
                SparseMatrixELL<DT_> smatrix6 = MatrixIO<io_formats::ELL>::read_matrix(filename_6, DT_(1));
                TEST_CHECK_EQUAL(smatrix6, smatrix5);
                remove(filename_6.c_str());
            }

            //-------------------------- MTX write matrix test
            std::string filename_5(HONEI_SOURCEDIR);
            filename_5 += "/honei/math/testdata/5pt_10x10-out.mtx";
            MatrixIO<io_formats::MTX>::write_matrix(filename_5, tsmatrix2);

            non_zeros = (MatrixIO<io_formats::MTX>::get_non_zeros(filename_5));
            DenseVector<unsigned long> rw(non_zeros);
            DenseVector<unsigned long> cw(non_zeros);
            DenseVector<DT_> dataw(non_zeros);
            MatrixIO<io_formats::MTX>::read_matrix(filename_5, rw, cw, dataw);
            MatrixIO<io_formats::MTX>::get_sizes(filename_5, rows, columns, ax, bx);
            SparseMatrix<DT_> tsmatrixw(rows, columns, rw, cw, dataw);

            TEST_CHECK_EQUAL(tsmatrixw, tsmatrix2);

            remove(filename_5.c_str());

        }
};
MMIOTest<float, tags::CPU> mmio_test_float_dense("float");
MMIOTest<double, tags::CPU> mmio_test_double_dense("double");
#ifdef HONEI_CUDA
MMIOTest<float, tags::GPU::CUDA> cuda_mmio_test_float_dense("float");
#ifdef HONEI_CUDA_DOUBLE
MMIOTest<double, tags::GPU::CUDA> cuda_mmio_test_double_dense("double");
#endif
#endif

