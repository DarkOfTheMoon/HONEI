/* vim: set sw=4 sts=4 et nofoldenable : */

/*
 * Copyright (c) 2012 Dirk Ribbrock <dirk.ribbrock@uni-dortmund.de>
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
#ifndef LIBLA_GUARD_SPARSE_MATRIX_CSR_IMPL_HH
#define LIBLA_GUARD_SPARSE_MATRIX_CSR_IMPL_HH 1

#include <honei/la/sparse_matrix_csr.hh>
#include <honei/la/sparse_matrix.hh>
#include <honei/la/dense_vector.hh>
#include <honei/la/matrix_error.hh>
#include <honei/la/vector_error.hh>
#include <honei/util/assertion.hh>
#include <honei/util/log.hh>
#include <honei/util/private_implementation_pattern-impl.hh>
#include <honei/util/shared_array-impl.hh>
#include <honei/util/stringify.hh>
#include <honei/util/configuration.hh>

#include <cmath>
#include <vector>

namespace honei
{
    template <typename DataType_> struct Implementation<SparseMatrixCSR<DataType_> >
    {
        unsigned long blocksize;//our atomic 1d-blocksize

        DenseVector<unsigned long> Aj;//column indices corresponding to elements in Ax
        DenseVector<DataType_> Ax;//nonzero values
        DenseVector<unsigned long> Ar;//indices of beginning rows in Ax/Aj

        /// Our row count.
        unsigned long rows;

        /// Our column count.
        unsigned long columns;

        unsigned long used_elements;//number of non zero elements

        /// Our zero element.
        static const DataType_ zero_element;

        Implementation(const unsigned long blocksize, const unsigned long rows, const unsigned long columns,
            const DenseVector<unsigned long> & Aj, const DenseVector<DataType_> & Ax, const DenseVector<unsigned long> & Ar, const unsigned long used_elements) :
            blocksize(blocksize),
            Aj(Aj),
            Ax(Ax),
            Ar(Ar),
            rows(rows),
            columns(columns),
            used_elements(used_elements)
        {
        }

        Implementation(const SparseMatrix<DataType_> & src) :
            blocksize(Configuration::instance()->get_value("csr::blocksize", 1)),
            Aj(src.used_elements()),
            Ax(1),
            Ar(src.rows() + 1),
            rows(src.rows()),
            columns(src.columns()),
            used_elements(0)
        {
            _create(src);
        }

        Implementation(const SparseMatrixELL<DataType_> & src) :
            blocksize(Configuration::instance()->get_value("csr::blocksize", 1)),
            Aj(src.used_elements()),
            Ax(1),
            Ar(src.rows() + 1),
            rows(src.rows()),
            columns(src.columns()),
            used_elements(0)
        {
            SparseMatrix<DataType_> temp(src);
            _create(temp);
        }

        void _create(const SparseMatrix<DataType_> & src)
        {
            std::vector<DataType_> Axv;
            unsigned long gi(0);
            unsigned long ue(0);
            for (unsigned long row(0) ; row < src.rows() ; ++row)
            {
                Ar[row] = gi;
                for (unsigned long i(0) ; i < src[row].used_elements() ;)
                {
                    Aj[gi] = src[row].indices()[i] - (src[row].indices()[i] % blocksize);
                    unsigned long delta_i(0);
                    for (unsigned long blocki(0) ; blocki < blocksize ; ++blocki)
                    {
                        if (i+delta_i < src[row].used_elements() && Aj[gi] + blocki == src[row].indices()[i+delta_i])
                        {
                            Axv.push_back(src[row].elements()[i + delta_i]);
                            ++ue;
                            ++delta_i;
                        }
                        else
                        {
                            Axv.push_back(DataType_(0));
                        }
                    }
                    i += delta_i;
                    ++gi;
                }
            }
            Ar[src.rows()] = gi;
            used_elements = ue;

            DenseVector<DataType_> axt(Axv.size());
            Ax = axt;
            for (unsigned long i(0) ; i < Axv.size() ; ++i)
            {
                Ax[i] = Axv.at(i);
            }
        }
    };

    template <typename DataType_>
    SparseMatrixCSR<DataType_>::SparseMatrixCSR(const unsigned long blocksize, const unsigned long rows, const unsigned long columns,
            const DenseVector<unsigned long> & Aj, const DenseVector<DataType_> & Ax, const DenseVector<unsigned long> & Ar, const unsigned long ue) :
        PrivateImplementationPattern<SparseMatrixCSR<DataType_>, Shared>(new Implementation<SparseMatrixCSR<DataType_> >(blocksize, rows, columns, Aj, Ax, Ar, ue))
    {
        CONTEXT("When creating SparseMatrixCSR from CSR Vectors:");
    }

    template <typename DataType_>
    SparseMatrixCSR<DataType_>::SparseMatrixCSR(const SparseMatrix<DataType_> & src) :
        PrivateImplementationPattern<SparseMatrixCSR<DataType_>, Shared>(new Implementation<SparseMatrixCSR<DataType_> >(src))
    {
        CONTEXT("When creating SparseMatrixCSR from SparseMatrix:");
    }

    template <typename DataType_>
    SparseMatrixCSR<DataType_>::SparseMatrixCSR(const SparseMatrixELL<DataType_> & src) :
        PrivateImplementationPattern<SparseMatrixCSR<DataType_>, Shared>(new Implementation<SparseMatrixCSR<DataType_> >(src))
    {
        CONTEXT("When creating SparseMatrixCSR from SparseMatrixELL:");
    }

    template <typename DataType_>
    SparseMatrixCSR<DataType_>::SparseMatrixCSR(const SparseMatrixCSR<DataType_> & other) :
        PrivateImplementationPattern<SparseMatrixCSR<DataType_>, Shared>(other._imp)
    {
    }

    template <typename DataType_>
    SparseMatrixCSR<DataType_>::~SparseMatrixCSR()
    {
    }

    template <typename DataType_>
    unsigned long
    SparseMatrixCSR<DataType_>::blocksize() const
    {
        return this->_imp->blocksize;
    }

    template <typename DataType_>
    unsigned long
    SparseMatrixCSR<DataType_>::columns() const
    {
        return this->_imp->columns;
    }

    template <typename DataType_>
    unsigned long
    SparseMatrixCSR<DataType_>::rows() const
    {
        return this->_imp->rows;
    }

    template <typename DataType_>
    unsigned long
    SparseMatrixCSR<DataType_>::size() const
    {
        return this->_imp->columns * this->_imp->rows;
    }

    template <typename DataType_>
    unsigned long
    SparseMatrixCSR<DataType_>::used_elements() const
    {
        return this->_imp->used_elements;
    }

    template <typename DataType_>
    DenseVector<unsigned long> &
    SparseMatrixCSR<DataType_>::Aj() const
    {
        return this->_imp->Aj;
    }

    template <typename DataType_>
    DenseVector<DataType_> &
    SparseMatrixCSR<DataType_>::Ax() const
    {
        return this->_imp->Ax;
    }

    template <typename DataType_>
    DenseVector<unsigned long> &
    SparseMatrixCSR<DataType_>::Ar() const
    {
        return this->_imp->Ar;
    }

    template <typename DataType_>
    const DataType_ SparseMatrixCSR<DataType_>::operator() (unsigned long row, unsigned long column) const
    {
        unsigned long row_i(this->_imp->Ar[row]);
        while (row_i < this->_imp->Ar[row + 1])
        {
            if (column >= this->_imp->Aj[row_i] && (column < this->_imp->Aj[row_i] + this->blocksize()))
                return this->_imp->Ax[row_i * this->blocksize() +  (column - this->_imp->Aj[row_i])];

            ++row_i;
        }
        return this->_imp->zero_element;
    }

    template <typename DataType_>
    void SparseMatrixCSR<DataType_>::lock(LockMode mode) const
    {
        this->_imp->Aj.lock(mode);
        this->_imp->Ax.lock(mode);
        this->_imp->Ar.lock(mode);
    }

    template <typename DataType_>
            void SparseMatrixCSR<DataType_>::unlock(LockMode mode) const
    {
        this->_imp->Aj.unlock(mode);
        this->_imp->Ax.unlock(mode);
        this->_imp->Ar.unlock(mode);
    }

    template <typename DataType_>
    SparseMatrixCSR<DataType_>
    SparseMatrixCSR<DataType_>::copy() const
    {
        CONTEXT("When creating copy() of a SparseMatrixCSR:");
        SparseMatrixCSR result(this->_imp->blocksize, this->_imp->rows,
                this->_imp->columns,
                this->_imp->Aj.copy(),
                this->_imp->Ax.copy(),
                this->_imp->Ar.copy(),
                this->_imp->used_elements);

        return result;
    }

    template <typename DataType_>
    bool
    operator== (const SparseMatrixCSR<DataType_> & a, const SparseMatrixCSR<DataType_> & b)
    {
        if (a.columns() != b.columns())
        {
            throw MatrixColumnsDoNotMatch(b.columns(), a.columns());
        }

        if (a.rows() != b.rows())
        {
            throw MatrixRowsDoNotMatch(b.rows(), a.rows());
        }

        bool result(true);

        result &= (a.Aj() == b.Aj());
        result &= (a.Ax() == b.Ax());
        result &= (a.Ar() == b.Ar());

        return result;
    }

    template <typename DataType_>
    std::ostream &
    operator<< (std::ostream & lhs, const SparseMatrixCSR<DataType_> & b)
    {
        lhs << "SparseMatrixCSR" << std::endl << "[" << std::endl;
        for (unsigned long row(0) ; row < b.rows() ; ++row)
        {
            lhs << "[";
            for (unsigned long column(0) ; column < b.columns() ; ++column)
            {
                lhs << " " << b(row, column);
            }
            lhs << "]";
            lhs << std::endl;
        }
        lhs << "]" << std::endl;

        lhs << "Aj: " << b.Aj();
        lhs << "Ax: " << b.Ax();
        lhs << "Ar: " << b.Ar();
        lhs << "Blocksize: " << b.blocksize() << std::endl;
        return lhs;
    }
}
#endif
