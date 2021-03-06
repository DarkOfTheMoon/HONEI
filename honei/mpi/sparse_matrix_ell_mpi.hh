/* vim: set sw=4 sts=4 et nofoldenable : */

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

#pragma once
#ifndef MPI_GUARD_SPARSE_MATRIX_ELL_MPI_HH
#define MPI_GUARD_SPARSE_MATRIX_ELL_MPI_HH 1

#include <honei/util/tags.hh>
#include <honei/util/configuration.hh>
#include <honei/la/sparse_matrix_ell.hh>
#include <honei/la/sparse_matrix.hh>
#include <honei/la/dense_vector.hh>
#include <honei/mpi/dense_vector_mpi.hh>
#include <honei/mpi/sparse_matrix_csr_mpi-fwd.hh>
#include <honei/backends/mpi/operations.hh>

#include <vector>
#include <set>

namespace honei
{
    template <typename DT_> class SparseMatrixELLMPI
    {
        private:
            shared_ptr<SparseMatrixELL<DT_> > _inner;
            shared_ptr<SparseMatrixELL<DT_> > _outer;
            std::set<unsigned long> _missing_indices;
            std::vector<unsigned long> _recv_ranks;
            std::vector<unsigned long> _recv_sizes;
            std::vector<unsigned long> _send_ranks;
            std::vector<unsigned long> _send_sizes;
            std::vector<unsigned long> _send_index;
            unsigned long _send_size;
            unsigned long _rows;
            unsigned long _columns;
            unsigned long _offset;
            unsigned long _x_offset;
            unsigned long _col_part_size;
            unsigned long _rank;
            unsigned long _com_size;
            unsigned long _orig_rows;
            unsigned long _orig_columns;
            bool _active;

            void _init(const SparseMatrix<DT_> & src, unsigned long global_rows, MPI_Comm com = MPI_COMM_WORLD)
            {
                _orig_rows = global_rows;
                _orig_columns = src.columns();
                int irank;
                mpi::mpi_comm_rank(&irank, com);
                _rank = irank;
                int icom_size;
                mpi::mpi_comm_size(&icom_size, com);
                _com_size = icom_size;

                _rows = DenseVectorMPI<DT_>::calc_size(global_rows, com);
                _columns = src.columns();
                _offset = DenseVectorMPI<DT_>::calc_offset(global_rows, com);

                _col_part_size = DenseVectorMPI<DT_>::calc_size(src.columns(), com);
                _x_offset = DenseVectorMPI<DT_>::calc_offset(src.columns(), com);

                if (_rows != 0)
                {
                    _active = true;
                    SparseMatrix<DT_> src_part(1,1);
                    if (global_rows == src.rows())
                    {
                        // matrix fenster ausschneiden und in src_part speichern
                        SparseMatrix<DT_> src_part_new(_rows, _columns);
                        src_part = src_part_new;

                        for (unsigned long row(0) ; row < _rows ; ++row)
                        {
                            for (unsigned long i(0) ; i < src[row + _offset].used_elements() ; ++i)
                            {
                                src_part(row, (src[row + _offset].indices())[i], (src[row + _offset].elements())[i]);
                            }
                        }
                    }
                    else
                    {
                        if (_rows != src.rows())
                            throw InternalError("SMELLMPI init: src matrix has wrong row count!");

                        // src matrix übernehmen
                        src_part = src;
                    }

                    // inneren teil ohne abhaengigkeiten in inner speichern
                    SparseMatrix<DT_> inner(_rows, _col_part_size);
                    for (unsigned long col(0) ; col < _col_part_size ; ++col)
                    {
                        for (unsigned long i(0) ; i < src_part.column(col + _x_offset).used_elements() ; ++i)
                            inner((src_part.column(col + _x_offset).indices())[i], col, (src_part.column(col + _x_offset).elements())[i]);
                    }
                    _inner.reset(new SparseMatrixELL<DT_>(inner));


                    // extern abhaengige spalten berechnen
                    for (unsigned long col(0) ; col < _x_offset  && col < _columns; ++col)
                    {
                        if (src_part.column(col).used_elements() != 0)
                            _missing_indices.insert(col);
                    }
                    for (unsigned long col(_x_offset + _col_part_size) ; col < _columns  ; ++col)
                    {
                        if (src_part.column(col).used_elements() != 0)
                            _missing_indices.insert(col);
                    }

                    // outer matrix mit allen abhaengigkeiten nach draussen erstellen
                    if(_missing_indices.size() != 0)
                    {
                        SparseMatrix<DT_> outer_comp(_rows, _missing_indices.size());
                        {
                            unsigned long cix(0);
                            for (std::set<unsigned long>::iterator ci(_missing_indices.begin()) ; ci != _missing_indices.end() ; ++ci, ++cix)
                            {
                                for (unsigned long i(0) ; i < src_part.column(*ci).used_elements() ; ++i)
                                {
                                    outer_comp((src_part.column(*ci).indices())[i], cix, (src_part.column(*ci).elements())[i]);
                                }
                            }
                        }
                        _outer.reset(new SparseMatrixELL<DT_>(outer_comp));
                    }
                    else
                    {
                        SparseMatrix<DT_> outer_comp(_rows, 1);
                        _outer.reset(new SparseMatrixELL<DT_>(outer_comp));
                    }
                }
                else
                {
                    _active = false;
                    SparseMatrix<DT_> inner(1, 1);
                    _inner.reset(new SparseMatrixELL<DT_>(inner));
                    SparseMatrix<DT_> outer_comp(1, 1);
                    _outer.reset(new SparseMatrixELL<DT_>(outer_comp));
                }


                // liste aufbauen, wer was hat und braucht
                unsigned long send_size(0);
                for (unsigned long rank(0) ; rank < _com_size ; ++rank)
                {
                    send_size = 0;
                    if (rank == _rank)
                    {
                        unsigned long count(_missing_indices.size());
                        mpi::mpi_bcast(&count, 1, rank);

                        unsigned long last_owner(-1);
                        unsigned long cix(0);
                        unsigned long last_cix(0);
                        for (std::set<unsigned long>::iterator i(_missing_indices.begin()) ; i != _missing_indices.end() ; ++i, ++cix)
                        {
                            unsigned long index(*i);
                            mpi::mpi_bcast(&index, 1, rank);
                            unsigned long owner(0);
                            mpi::mpi_recv(&owner, 1, MPI_ANY_SOURCE, rank);

                            if (last_owner == (unsigned long)(-1))
                            {
                                last_owner = owner;
                                _recv_ranks.push_back(owner);
                            }
                            else if (last_owner != owner)
                            {
                                _recv_sizes.push_back(cix - last_cix);
                                last_cix = cix;
                                last_owner = owner;
                                _recv_ranks.push_back(owner);
                            }

                        }
                        if (_missing_indices.size() != 0)
                            _recv_sizes.push_back(cix - last_cix);
                    }
                    else
                    {
                        unsigned long count;
                        mpi::mpi_bcast(&count, 1, rank);
                        for (unsigned long i(0) ; i < count ; ++i)
                        {
                            unsigned long index;
                            mpi::mpi_bcast(&index, 1, rank);
                            if (index >= _x_offset && index < _x_offset + _col_part_size)
                            {
                                mpi::mpi_send(&_rank, 1, rank, rank);

                                if (_send_ranks.size() == 0 || _send_ranks.at(_send_ranks.size() - 1) != rank)
                                {
                                    _send_ranks.push_back(rank);
                                }

                                _send_index.push_back(index - _x_offset);
                                ++send_size;
                            }
                        }
                    }
                    if (_send_ranks.size() != 0 && _send_ranks.at(_send_ranks.size() - 1) == rank)
                        _send_sizes.push_back(send_size);
                }

                send_size = 0;
                for (unsigned long i(0) ; i < _send_sizes.size() ; ++i)
                    send_size+= _send_sizes.at(i);
                _send_size = send_size;

            }

        public:
            friend class SparseMatrixCSRMPI<DT_>;

            /// Our internal DataType
            typedef DT_ DataType;

            /// Constructors
            /// \{

            /**
             * Constructor.
             */
            explicit SparseMatrixELLMPI(const SparseMatrix<DT_> & src, MPI_Comm com = MPI_COMM_WORLD)
            {
                _init(src, src.rows(), com);
            }

            explicit SparseMatrixELLMPI(const SparseMatrix<DT_> & src, unsigned long global_rows, MPI_Comm com = MPI_COMM_WORLD)
            {
                _init(src, global_rows, com);
            }

            explicit SparseMatrixELLMPI(const SparseMatrixELL<DT_> & src, MPI_Comm com = MPI_COMM_WORLD)
            {
                SparseMatrix<DT_> t(src);
                _init(t, t.rows(), com);
            }


            /// Copy-constructor.
            SparseMatrixELLMPI(const SparseMatrixELLMPI<DT_> & other) :
                _missing_indices(other._missing_indices),
                _recv_ranks(other._recv_ranks),
                _recv_sizes(other._recv_sizes),
                _send_ranks(other._send_ranks),
                _send_sizes(other._send_sizes),
                _send_index(other._send_index),
                _send_size(other._send_size),
                _rows(other._rows),
                _columns(other._columns),
                _offset(other._offset),
                _x_offset(other._x_offset),
                _col_part_size(other._col_part_size),
                _rank(other._rank),
                _com_size(other._com_size),
                _orig_rows(other._orig_rows),
                _orig_columns(other._orig_columns),
                _active(other._active)
            {
                _inner.reset(new SparseMatrixELL<DT_> (*other._inner));
                _outer.reset(new SparseMatrixELL<DT_> (*other._outer));
            }

            SparseMatrixELLMPI(const SparseMatrixCSRMPI<DT_> & other) :
                _missing_indices(other._missing_indices),
                _recv_ranks(other._recv_ranks),
                _recv_sizes(other._recv_sizes),
                _send_ranks(other._send_ranks),
                _send_sizes(other._send_sizes),
                _send_index(other._send_index),
                _send_size(other._send_size),
                _rows(other._rows),
                _columns(other._columns),
                _offset(other._offset),
                _x_offset(other._x_offset),
                _col_part_size(other._col_part_size),
                _rank(other._rank),
                _com_size(other._com_size),
                _orig_rows(other._orig_rows),
                _orig_columns(other._orig_columns),
                _active(other._active)
            {
                _inner.reset(new SparseMatrixELL<DT_> (*other._inner));
                _outer.reset(new SparseMatrixELL<DT_> (*other._outer));
            }

            /// Destructor.
            virtual ~SparseMatrixELLMPI()
            {
            }

            /// \}

            typedef SparseMatrixELL<DT_> LocalType_;

            /// Returns our size.
            virtual unsigned long size() const
            {
                return _orig_rows * _orig_columns;
            }

            /// Returns our row count.
            virtual unsigned long rows() const
            {
                return _orig_rows;
            }

            /// Returns our column count.
            virtual unsigned long columns() const
            {
                return _orig_columns;
            }

            /// Returns our local row count.
            virtual unsigned long local_rows() const
            {
                return _rows;
            }

            /// Returns our local column count.
            virtual unsigned long local_columns() const
            {
                return _columns;
            }

            /// Returns our offset into the origin vector.
            virtual unsigned long offset() const
            {
                return _offset;
            }

            /// Returns our offset into the origin vector.
            virtual unsigned long x_offset() const
            {
                return _x_offset;
            }

            virtual bool active() const
            {
                return _active;
            }

            const DT_ operator()(unsigned long i, unsigned long j) const
            {
                if (j >= _x_offset && j < _x_offset + _col_part_size)
                {
                    return (*_inner)(i, j - _x_offset);
                }
                else
                {
                    unsigned long cix(0);
                    for (std::set<unsigned long>::iterator ci(_missing_indices.begin()) ; ci != _missing_indices.end() ; ++ci, ++cix)
                    {
                        if (*ci == j)
                            return (*_outer)(i, cix);
                    }
                    return DT_(0);
                }
            }

            const SparseMatrixELL<DT_> & inner_matrix() const
            {
                return *_inner;
            }

            /*SparseMatrixELL<DT_> & inner_matrix()
            {
                return *_inner;
            }*/

            const SparseMatrixELL<DT_> & outer_matrix() const
            {
                return *_outer;
            }

            /*SparseMatrixELL<DT_> & outer_matrix()
            {
                return *_outer;
            }*/

            const std::set<unsigned long> & missing_indices() const
            {
                return _missing_indices;
            }

            const std::vector<unsigned long> & recv_sizes() const
            {
                return _recv_sizes;
            }

            const std::vector<unsigned long> & recv_ranks() const
            {
                return _recv_ranks;
            }

            const std::vector<unsigned long> & send_sizes() const
            {
                return _send_sizes;
            }

            const std::vector<unsigned long> & send_ranks() const
            {
                return _send_ranks;
            }

            const std::vector<unsigned long> & send_index() const
            {
                return _send_index;
            }

            unsigned long send_size() const
            {
                return _send_size;
            }

            /// \{


            /*/// Return our memory id
            virtual void * memid() const;

            /// Return the address of our data
            virtual void * address() const;

            /// Request a memory access lock for our data.
            virtual void * lock(LockMode mode, tags::TagValue memory = tags::CPU::memory_value) const;

            /// Release a memory access lock for our data.
            virtual void unlock(LockMode mode) const;*/

            /// \}

            /// Return a copy of the Vector.
            SparseMatrixELLMPI<DT_> copy() const
            {
                SparseMatrixELLMPI<DT_> result(*this);
                result._inner.reset(new SparseMatrixELL<DT_>(this->_inner->copy()));
                result._outer.reset(new SparseMatrixELL<DT_>(this->_outer->copy()));
                return result;
            }
    };

    /**
     * Equality operator for SparseMatrixELLMPI.
     *
     * Compares if corresponding elements of two dense vectors are equal
     * within machine precision.
     */
    template <typename DT_> bool operator== (const SparseMatrixELLMPI<DT_> & a, const SparseMatrixELLMPI<DT_> & b)
    {
        return (
                a.inner_matrix() == b.inner_matrix() &&
                //a.outer_matrix() == b.outer_matrix() &&
                a.rows() == b.rows() &&
                a.columns() == b.columns() &&
                a.offset() == b.offset() &&
                a.x_offset() == b.x_offset());
    }

    /**
     * Output operator for SparseMatrixELLMPI.
     *
     * Outputs a dense vector to an output stream.
     */
    template <typename DT_> std::ostream & operator<< (std::ostream & lhs, const SparseMatrixELLMPI<DT_> & matrix)
    {
        lhs << "SparseMatrixELLMPI" << std::endl;
        lhs << "[" << std::endl;
        for (unsigned long row(0) ; row < matrix.local_rows() ; ++row)
        {
            lhs << "[";
            for (unsigned long col(0) ; col < matrix.columns() ; ++col)
            {
                lhs << " " << matrix(row, col);
            }
            lhs << "]" << std::endl;
        }
        lhs << "]" << std::endl;
        return lhs;
    }

    /*extern template class SparseMatrixELLMPI<float>;

    extern template bool operator== (const SparseMatrixELLMPI<float> & a, const SparseMatrixELLMPI<float> & b);

    extern template std::ostream & operator<< (std::ostream & lhs, const SparseMatrixELLMPI<float> & vector);

    extern template class SparseMatrixELLMPI<double>;

    extern template bool operator== (const SparseMatrixELLMPI<double> & a, const SparseMatrixELLMPI<double> & b);

    extern template std::ostream & operator<< (std::ostream & lhs, const SparseMatrixELLMPI<double> & vector);

    extern template class SparseMatrixELLMPI<long>;

    extern template bool operator== (const SparseMatrixELLMPI<long> & a, const SparseMatrixELLMPI<long> & b);

    extern template std::ostream & operator<< (std::ostream & lhs, const SparseMatrixELLMPI<long> & vector);

    extern template class SparseMatrixELLMPI<unsigned long>;

    extern template bool operator== (const SparseMatrixELLMPI<unsigned long> & a, const SparseMatrixELLMPI<unsigned long> & b);

    extern template std::ostream & operator<< (std::ostream & lhs, const SparseMatrixELLMPI<unsigned long> & vector);*/
}

#endif
