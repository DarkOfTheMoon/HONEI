/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2011 Dirk Ribbrock <dirk.ribbrock@math.uni-dortmund.de>
 *
 * This file is part of the MATH C++ library. LibMath is free software;
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

#pragma once
#ifndef LIBMATH_GUARD_SPAI2_HH
#define LIBMATH_GUARD_SPAI2_HH 1

#include <honei/util/tags.hh>
#include <honei/la/sparse_matrix.hh>
#include <honei/la/dense_matrix.hh>
#include <honei/util/operation_wrapper.hh>
#include <honei/util/configuration.hh>
#include <honei/backends/multicore/operation.hh>
#include <honei/backends/multicore/thread_pool.hh>
#include <honei/la/product.hh>
#include <honei/math/ludecomposition.hh>
#include <vector>
#include <algorithm>
#include <iostream>
//#include <mkl_lapacke.h>
//#include <omp.h>

//based on "Parallel Preconditioning with Sparse Approximate Inverses" by Grote et al.
namespace honei
{
    template <typename Tag_>
    struct SPAI2
    {
    };

    template <>
    struct SPAI2<tags::CPU>
    {
        template <typename DT_>
        static DenseVector<DT_> & value_0(DenseVector<DT_> & result, const SparseMatrix<DT_> & A, unsigned long col_start = 0, unsigned long col_end = 0)
        {
            if (col_end == 0)
                col_end = A.columns();
            for (unsigned long idx(col_start) ; idx < col_end ; ++idx)
            {
                unsigned long n2(1);
                unsigned long J[n2];
                J[0] = idx;

                unsigned long n1(0);
                for (unsigned long i(0) ; i < n2 ; ++i)
                {
                    n1 += A.column(J[i]).used_elements();
                }
                if (n1 == 0)
                    continue;
                unsigned long I[n1];
                DenseVector<DT_> et(n1, DT_(0));
                unsigned long tmp(0);
                for (unsigned long i(0) ; i < n2 ; ++i)
                {
                    for (unsigned long j(0) ; j < A.column(J[i]).used_elements() ; ++j)
                    {
                        I[tmp] = A.column(J[i]).indices()[j];
                        et[tmp] = (I[tmp] == idx);
                        ++tmp;
                    }
                }

                DenseMatrix<DT_> At(n1, n2, DT_(0));
                for (unsigned long i(0) ; i < n1 ; ++i)
                {
                    SparseVector<DT_> row = A[I[i]];
                    unsigned long it(0);
                    const unsigned long * indices(row.indices());
                    const DT_ * elements(row.elements());
                    const unsigned long used_elements(row.used_elements());
                    for (unsigned long j(0) ; j < n2 ; ++j)
                    {
                        const unsigned long index(J[j]);
                        for ( ; (indices[it] < index) && (it < used_elements) ; ++it)
                            ;
                        if (indices[it] == index)
                            At(i, j) = elements[it];
                    }
                }

                //LAPACKE_dgels(LAPACK_ROW_MAJOR, 'N', At.rows(), At.columns(), 1, At.elements(), At.columns(), et.elements(), 1);
                //result[idx] = et[0];

                DenseMatrix<DT_> Atrans(n2, n1);
                for (unsigned long i(0) ; i < At.rows() ; ++i)
                {
                    for (unsigned long j(0) ; j < At.columns() ; ++j)
                    {
                        Atrans(j, i) = At(i, j);
                    }
                }
                DenseMatrix<DT_> product = Product<tags::CPU::SSE>::value(Atrans, At);
                DenseVector<DT_> pro_v = Product<tags::CPU::SSE>::value(Atrans, et);
                DenseVector<DT_> res(product.columns());
                LUDecomposition<tags::CPU::SSE>::value(product, pro_v, res);
                result[idx] = res[0];

            }

            return result;
        }
    };

    template <>
    struct SPAI2<tags::CPU::SSE>
    {
        template <typename DT_>
        static SparseMatrix<DT_> & value(SparseMatrix<DT_> & M, const SparseMatrix<DT_> & A, unsigned long col_start = 0, unsigned long col_end = 0)
        {
            if (col_end == 0)
                col_end = A.columns();
            for (unsigned long idx(col_start) ; idx < col_end ; ++idx)
            {
                unsigned long n2(A.column(idx).used_elements());
                if (n2 == 0)
                    continue;
                unsigned long J[n2];
                const unsigned long * A_col_idx_index(A.column(idx).indices());
                for (unsigned long i(0) ; i < n2 ; ++i)
                {
                    J[i] = A_col_idx_index[i];
                }

                unsigned long n1(0);
                for (unsigned long i(0) ; i < n2 ; ++i)
                {
                    n1 += A.column(J[i]).used_elements();
                }
                if (n1 == 0)
                    continue;
                unsigned long I[n1];
                DenseVector<DT_> et(n1, DT_(0));
                DT_ * __restrict__ ete(et.elements());
                unsigned long tmp(0);
                for (unsigned long i(0) ; i < n2 ; ++i)
                {
                    const unsigned long A_col_ji_ue(A.column(J[i]).used_elements());
                    const unsigned long * A_col_ji_index(A.column(J[i]).indices());
                    for (unsigned long j(0) ; j < A_col_ji_ue ; ++j)
                    {
                        I[tmp] = A_col_ji_index[j];
                        ete[tmp] = (I[tmp] == idx);
                        ++tmp;
                    }
                }

                DenseMatrix<DT_> At(n1, n2, DT_(0));
                for (unsigned long i(0) ; i < n1 ; ++i)
                {
                    const SparseVector<DT_> row = A[I[i]];
                    unsigned long it(0);
                    const unsigned long * indices(row.indices());
                    const DT_ * __restrict__ elements(row.elements());
                    const unsigned long used_elements(row.used_elements());
                    for (unsigned long j(0) ; j < n2 ; ++j)
                    {
                        const unsigned long index(J[j]);
                        for ( ; (indices[it] < index) && (it < used_elements) ; ++it)
                            ;
                        if (indices[it] == index)
                            At(i, j) = elements[it];
                    }
                }
                // GPU ONLY - without row vectors
                /*DenseMatrix<DT_> At(n1, n2, DT_(0));
                for (unsigned long j(0) ; j < n2 ; ++j)
                {
                    const SparseVector<DT_> column = A.column(J[j]);
                    for (unsigned long i(0) ; i < n1 ; ++i)
                    {
                        At(i, j) = column[I[i]];
                    }
                }*/

                /*LAPACKE_dgels(LAPACK_ROW_MAJOR, 'N', At.rows(), At.columns(), 1, At.elements(), At.columns(), et.elements(), 1);
                for (unsigned long i(0) ; i < n2 ; ++i)
                {
                    M(A.column(idx).indices()[i], idx, et[i]);
                }*/

                DenseMatrix<DT_> Atrans(n2, n1);
                DT_ * __restrict__ Ate(At.elements());
                DT_ * __restrict__ Atranse(Atrans.elements());
                // \TODO schneller transponieren
                for (unsigned long i(0) ; i < n1 ; ++i)
                {
                    for (unsigned long j(0) ; j < n2 ; ++j)
                    {
                        Atranse[n1 * j + i] = Ate[n2 * i + j];
                    }
                }
                //DenseMatrix<DT_> product = Product<tags::CPU::SSE>::value(Atrans, At);
                DenseMatrix<DT_> product(n2, n2);
                DT_ * __restrict__ producte(product.elements());
                for (unsigned long i(0) ; i < n2 ; ++i)
                {
                    for (unsigned long j(0) ; j < n2 ; ++j)
                    {
                        DT_ temp(0);
                        for (unsigned long k(0) ; k < n1 ; ++k)
                        {
                            temp += Atranse[n1 * i + k] * Ate[n2 * k + j];
                        }
                        producte[n2 * i + j] = temp;
                    }
                }
                //DenseVector<DT_> pro_v = Product<tags::CPU::SSE>::value(Atrans, et);
                DenseVector<DT_> pro_v(n2);
                DT_ * __restrict__ pro_ve(pro_v.elements());
                for (unsigned long i(0) ; i < n2 ; ++i)
                {
                    DT_ temp(0);
                    for (unsigned long j(0) ; j < n1 ; ++j)
                    {
                        temp += Atranse[n1 * i + j] * et[j];
                    }
                    pro_ve[i] = temp;
                }
                DenseVector<DT_> res(product.columns());
                LUDecomposition<tags::CPU::SSE>::value(product, pro_v, res);
                for (unsigned long i(0) ; i < n2 ; ++i)
                {
                    M(A.column(idx).indices()[i], idx, res[i]);
                    //M.column(idx).elements()[i] = res[i];
                    //M._synch_row_vectors();
                }
            }

            return M;
        }

    };

    namespace mc
    {
        template <typename Tag_> struct SPAI2
        {
            template <typename DT_>
            static SparseMatrix<DT_> & value(SparseMatrix<DT_> & M, const SparseMatrix<DT_> & A)
            {
                //omp_set_num_threads(1);
                unsigned long max_count(Configuration::instance()->get_value("mc::Product(DV,SMELL,DV)::max_count",
                            mc::ThreadPool::instance()->num_threads()));
                max_count = 10;

                TicketVector tickets;

                unsigned long limits[max_count + 1];
                limits[0] = 0;
                for (unsigned long i(1) ; i < max_count; ++i)
                {
                    limits[i] = limits[i-1] + A.columns() / max_count;
                }
                limits[max_count] = A.columns();

                for (unsigned long i(0) ; i < max_count ; ++i)
                {
                    OperationWrapper<honei::SPAI2<typename Tag_::DelegateTo>, SparseMatrix<DT_>,
                        SparseMatrix<DT_>, SparseMatrix<DT_>, unsigned long, unsigned long > wrapper(M);
                    tickets.push_back(mc::ThreadPool::instance()->enqueue(bind(wrapper, M, A, limits[i], limits[i+1])));
                }

                tickets.wait();

                return M;
            }
        };
    }

    template <> struct SPAI2<tags::CPU::MultiCore::SSE> :
        public mc::SPAI2<tags::CPU::MultiCore::SSE>
    {
    };

    template <>
    struct SPAI2<tags::GPU::CUDA>
    {
        static SparseMatrix<float> & value(SparseMatrix<float> & M, const SparseMatrix<float> & A);
        static SparseMatrix<double> & value(SparseMatrix<double> & M, const SparseMatrix<double> & A);
    };
}
#endif
