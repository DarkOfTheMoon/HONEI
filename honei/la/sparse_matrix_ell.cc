/* vim: set sw=4 sts=4 et nofoldenable : */

/*
 * Copyright (c) 2009 Dirk Ribbrock <dirk.ribbrock@uni-dortmund.de>
 *
 * This file is part of the HONEI C++ library. HOENI is free software;
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

#include <honei/la/sparse_matrix_ell.hh>
#include <honei/la/sparse_matrix_ell-impl.hh>

namespace honei
{
    template <> const float Implementation<SparseMatrixELL<float> >::zero_element(float(0.0));

    template class SparseMatrixELL<float>;

    template bool operator== (const SparseMatrixELL<float> & a, const SparseMatrixELL<float> & b);

    template std::ostream & operator<< (std::ostream & lhs, const SparseMatrixELL<float> & matrix);

    template <> const double Implementation<SparseMatrixELL<double> >::zero_element(double(0.0));

    template class SparseMatrixELL<double>;

    template bool operator== (const SparseMatrixELL<double> & a, const SparseMatrixELL<double> & b);

    template std::ostream & operator<< (std::ostream & lhs, const SparseMatrixELL<double> & matrix);

#ifdef HONEI_GMP
    template <> const mpf_class Implementation<SparseMatrixELL<mpf_class> >::zero_element(mpf_class(0.0));

    template class SparseMatrixELL<mpf_class>;

    template bool operator== (const SparseMatrixELL<mpf_class> & a, const SparseMatrixELL<mpf_class> & b);

    template std::ostream & operator<< (std::ostream & lhs, const SparseMatrixELL<mpf_class> & matrix);
#endif
}

