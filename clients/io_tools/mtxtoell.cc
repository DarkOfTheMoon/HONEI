/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2010 Dirk Ribbrock <dirk.ribbrock@math.uni-dortmund.de>
 *
 * This file is part of HONEI. HONEI is free software;
 * you can redistribute it and/or modify it under the terms of the GNU General
 * Public License version 2, as published by the Free Software Foundation.
 *
 * HONEI is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA  02111-1307  USA
 */


#include <iostream>
#include <honei/math/matrix_io.hh>
#include <honei/la/sparse_matrix.hh>
#include <honei/la/sparse_matrix_ell.hh>

using namespace honei;

int main(int argc, char ** argv)
{
    if (argc != 3)
    {
        std::cout<<"Usage 'mtx2ell mtx-file ell-file'"<<std::endl;
        exit(EXIT_FAILURE);
    }
    if (sizeof(unsigned long) != sizeof(uint64_t))
    {
        std::cout<<"Dont run this on 32bit machines"<<std::endl;
        exit(EXIT_FAILURE);
    }
    /*for (int i(0) ; i < argc ; ++i)
        std::cout<<argv[i]<<std::endl;*/

    // Read in m file matrix
    std::string input(argv[1]);
    std::string output(argv[2]);
    SparseMatrix<double> tsmatrix(MatrixIO<io_formats::MTX, SparseMatrix<double> >::read_matrix(input));
    SparseMatrixELL<double> smatrix(tsmatrix);

    // Write out ell file matrix
    MatrixIO<io_formats::ELL, SparseMatrixELL<double> >::write_matrix(output, smatrix);

    return EXIT_SUCCESS;
}
