/* vim: set sw=4 sts=4 et nofoldenable : */

/*
 * Copyright (c) 2009 Dirk Ribbrock <dirk.ribbrock@uni-dortmund.de>
 *
 *
 * This file is part of the HONEI C++ library. HONEI is free software;
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

#pragma once
#ifndef LIBLA_GUARD_SPARSE_MATRIX_ELL_HH
#define LIBLA_GUARD_SPARSE_MATRIX_ELL_HH 1

#include <honei/la/dense_vector.hh>
#include <honei/la/dense_matrix.hh>
#include <honei/util/private_implementation_pattern.hh>
#ifdef HONEI_GMP
#include <gmpxx.h>
#endif


namespace honei
{
    // Forward declarations
    template <typename DataType_> class SparseMatrix;
    template <typename DataType_> class SparseMatrixCSR;

    /**
     * \brief SparseMatrixELL is a sparse matrix with its data kept in the ELLPACK format.
     *
     * \ingroup grpmatrix
     */
    template <typename DataType_> class SparseMatrixELL :
        PrivateImplementationPattern<SparseMatrixELL<DataType_>, Shared>
    {
        public:

            /// Our internal DataType
            typedef DataType_ iDT_;

            /// \name Basic operations
            /// \{

            /**
             * Constructor.
             *
             */
            SparseMatrixELL(unsigned long rows, unsigned columns, unsigned long stride,
                    unsigned long num_cols_per_row,
                    const DenseVector<unsigned long> & Aj,
                    const DenseVector<DataType_> & Ax,
                    unsigned long threads);


            /**
             * Constructor.
             *
             * \param src The SparseMatrix our matrix will be created from.
             */
            explicit SparseMatrixELL(const SparseMatrix<DataType_> & src);

            /**
             * Constructor.
             *
             * \param src The SparseMatrixCSR our matrix will be created from.
             */
            explicit SparseMatrixELL(const SparseMatrixCSR<DataType_> & src);

            /// Copy-constructor.
            SparseMatrixELL(const SparseMatrixELL<DataType_> & other);

            /// Destructor.
            ~SparseMatrixELL();

            /// \}

            /// Returns the number of our columns.
            unsigned long columns() const;

            /// Returns the number of our rows.
            unsigned long rows() const;

            /// Returns our size, equal to rows and columns.
            unsigned long size() const;

            /// Returns out non zero element count.
            unsigned long used_elements() const;

            /// Returns our Aj / Ax row count.
            unsigned long stride() const;

            /// Returns our Aj / Ax column count.
            unsigned long num_cols_per_row() const;

            /// Returns the thread count, e.g. the packed data count per Aj row
            unsigned long threads() const;

            /// Retrieves our Aj (indices) vector.
            DenseVector<unsigned long> & Aj() const;

            /// Retrieves our Ax (data) vector.
            DenseVector<DataType_> & Ax() const;

            /// Retrieves our Arl (row length) vector.
            DenseVector<unsigned long> & Arl() const;

            /// Retrieves element at (row, column), unassignable.
            const DataType_ operator() (unsigned long row, unsigned long column) const;

            /// Request a memory access lock for our data.
            void lock(LockMode mode) const;

            /// Release a memory access lock for our data.
            void unlock(LockMode mode) const;

            /// Returns a copy of the matrix.
            SparseMatrixELL copy() const;
    };

    /**
     * Equality operator for SparseMatrixELL.
     *
     * Compares if corresponding elements of two ell matrices are equal
     * within machine precision.
     */
    template <typename DataType_> bool operator== (const SparseMatrixELL<DataType_> & a, const SparseMatrixELL<DataType_> & b);

    /**
     * Output operator for SparseMatrixELL.
     *
     * Outputs a ell matrix to an output stream.
     */
    template <typename DataType_> std::ostream & operator<< (std::ostream & lhs, const SparseMatrixELL<DataType_> & matrix);

    extern template class SparseMatrixELL<float>;

    extern template bool operator== (const SparseMatrixELL<float> & a, const SparseMatrixELL<float> & b);

    extern template std::ostream & operator<< (std::ostream & lhs, const SparseMatrixELL<float> & matrix);

    extern template class SparseMatrixELL<double>;

    extern template bool operator== (const SparseMatrixELL<double> & a, const SparseMatrixELL<double> & b);

    extern template std::ostream & operator<< (std::ostream & lhs, const SparseMatrixELL<double> & matrix);

#ifdef HONEI_GMP
    extern template class SparseMatrixELL<mpf_class>;

    extern template bool operator== (const SparseMatrixELL<mpf_class> & a, const SparseMatrixELL<mpf_class> & b);

    extern template std::ostream & operator<< (std::ostream & lhs, const SparseMatrixELL<mpf_class> & matrix);
#endif
}
#endif
