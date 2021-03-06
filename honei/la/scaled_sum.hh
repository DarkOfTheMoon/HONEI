/* vim: set sw=4 sts=4 et nofoldenable : */

/*
 * Copyright (c) 2007, 2008, 2009 Sven Mallach <mallach@honei.org>
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
#ifndef LIBLA_GUARD_SCALED_SUM_HH
#define LIBLA_GUARD_SCALED_SUM_HH 1

#include <honei/backends/multicore/operation.hh>
#include <honei/backends/multicore/thread_pool.hh>
#include <honei/la/dense_matrix.hh>
#include <honei/la/dense_vector.hh>
#include <honei/la/sparse_vector.hh>
#include <honei/la/vector_error.hh>
#include <honei/util/benchmark_info.hh>
#include <honei/util/configuration.hh>
#include <honei/util/tags.hh>
#include <honei/mpi/operations.hh>
#include <honei/mpi/dense_vector_mpi-fwd.hh>

namespace honei
{
    template <typename Tag_ = tags::CPU> struct ScaledSum;

    /**
     * \brief Scaled sum of two given vectors and a given scalar.
     *
     * ScaledSum is the class template for the operation
     * \f[
     *     \texttt{ScaledSum}(x, a, y): \quad x \leftarrow a \cdot x + y,
     *     \texttt{ScaledSum}(x, y, b): \quad x \leftarrow x + b \cdot y,
     * \f]
     * which yields the scaled sum of x and y.
     *
     * \todo Implement variant using a.
     *
     * \ingroup grplaoperations
     * \ingroup grplavectoroperations
     */
    template <> struct ScaledSum<tags::CPU>
    {
        /**
         * \name Scaled sums
         * \{
         *
         * \brief Returns the vector x as the scaled sum of two given vectors.
         *
         * \param x The vector that shall not be scaled.
         * \param y The vector that shall be scaled.
         * \param b The scale factor.
         *
         * \retval x Will modify x and return it.
         *
         * \exception VectorSizeDoesNotMatch is thrown if the sizes of x and y do not match.
         */

        template <typename DT1_, typename DT2_>
        static DenseVectorBase<DT1_> & value(DenseVectorBase<DT1_> & x, const DenseVectorBase<DT2_> & y, DT2_ b)
        {
            CONTEXT("When calculating ScaledSum (DenseVectorBase, DenseVectorBase, scalar):");

            if (x.size() != y.size())
                throw VectorSizeDoesNotMatch(y.size(), x.size());

            typename DenseVectorBase<DT2_>::ConstElementIterator r(y.begin_elements());
            for (typename DenseVectorBase<DT1_>::ElementIterator l(x.begin_elements()),
                    l_end(x.end_elements()) ; l != l_end ; ++l, ++r)
            {
                *l += b * (*r);
            }

            return x;
        }

        template <typename DT1_, typename DT2_>
        static DenseVectorBase<DT1_> & value(DenseVectorBase<DT1_> & result, const DenseVectorBase<DT1_> & x, const DenseVectorBase<DT2_> & y, DT2_ b)
        {
            CONTEXT("When calculating ScaledSum (DenseVectorBase, DenseVectorBase, scalar):");

            if (x.size() != y.size())
                throw VectorSizeDoesNotMatch(y.size(), x.size());

            if (x.size() != result.size())
                throw VectorSizeDoesNotMatch(result.size(), x.size());

            typename DenseVectorBase<DT1_>::ConstElementIterator x_i(x.begin_elements());
            typename DenseVectorBase<DT2_>::ConstElementIterator x_i_end(x.end_elements());
            typename DenseVectorBase<DT2_>::ConstElementIterator y_i(y.begin_elements());
            for (typename DenseVectorBase<DT1_>::ElementIterator res_i(result.begin_elements())
                    ; x_i != x_i_end ; ++x_i, ++y_i, ++res_i)
            {
                *res_i = b * (*y_i) + *x_i;
            }

            return result;
        }
        template <typename DT1_, typename DT2_>
        static inline DenseVectorContinuousBase<DT1_> & value(DenseVectorContinuousBase<DT1_> & x, const DenseVectorBase<DT2_> & y, DT2_ b)
        {
            DenseVectorBase<DT1_> & temp = x;
            ScaledSum<>::value(temp, y, b);
            return x;
        }

        template <typename DT1_, typename DT2_, typename DT3_>
        static SparseVector<DT1_> & value(SparseVector<DT1_> & x, const SparseVector<DT2_> & y, DT3_ b)
        {
            CONTEXT("When calculating ScaledSum (SparseVector, SparseVector, scalar):");

            if (x.size() != y.size())
                throw VectorSizeDoesNotMatch(y.size(), x.size());

            typename SparseVector<DT2_>::NonZeroElementIterator l(x.begin_non_zero_elements());
            for (typename SparseVector<DT1_>::NonZeroConstElementIterator r(y.begin_non_zero_elements()),
                    r_end(y.end_non_zero_elements()) ; r != r_end ; )
            {
                if (r.index() < l.index())
                {
                    x[r.index()] = b * (*r);
                    ++r;
                }
                else if (l.index() < r.index())
                {
                    ++l;
                }
                else
                {
                    *l += b * (*r);
                    ++l; ++r;
                }
            }
            ///\todo: perhaps sparsify - written results may be zero.
            return x;
        }

        template <typename DT1_, typename DT2_, typename DT3_>
        static DenseVectorBase<DT1_> & value(DenseVectorBase<DT1_> & x, const SparseVector<DT2_> & y, DT3_ b)
        {
            CONTEXT("When calculating ScaledSum (DenseVectorBase, SparseVector, scalar):");

            if (x.size() != y.size())
                throw VectorSizeDoesNotMatch(y.size(), x.size());

            for (typename SparseVector<DT2_>::NonZeroConstElementIterator r(y.begin_non_zero_elements()),
                    r_end(y.end_non_zero_elements()) ; r != r_end ; ++r )
            {
                x[r.index()] += b * (*r);
            }
            ///\todo: perhaps sparsify - if senseless use with b == zero, zeros may be written.
            return x;
        }

        template <typename DT1_, typename DT2_, typename DT3_>
        static inline DenseVectorContinuousBase<DT1_> & value(DenseVectorContinuousBase<DT1_> & x, const SparseVector<DT2_> & y, DT3_ b)
        {
            DenseVectorBase<DT1_> & temp = x;
            ScaledSum<>::value(temp, y, b);
            return x;
        }

        template <typename DT1_, typename DT2_, typename DT3_>
        static DenseVectorBase<DT1_> & value(DenseVectorBase<DT1_> & a, const DenseVectorBase<DT2_> & b, const DenseVectorBase<DT3_> & c)
        {
            CONTEXT("When calculating ScaledSum (DenseVectorBase, DenseVectorBase, DenseVectorBase):");

            if (a.size() != b.size())
                throw VectorSizeDoesNotMatch(b.size(), a.size());
            if (a.size() != c.size())
                throw VectorSizeDoesNotMatch(c.size(), a.size());

            typename DenseVectorBase<DT2_>::ConstElementIterator l(b.begin_elements());
            typename DenseVectorBase<DT3_>::ConstElementIterator r(c.begin_elements());
            for (typename DenseVectorBase<DT1_>::ElementIterator s(a.begin_elements()),
                    s_end(a.end_elements()) ; s != s_end ; ++l)
            {
                *s += *l * *r;
                ++r, ++s;
            }

            return a;
        }

        template <typename DT1_, typename DT2_, typename DT3_>
        static inline DenseVectorContinuousBase<DT1_> & value(DenseVectorContinuousBase<DT1_> & x, const DenseVectorBase<DT2_> & y, const DenseVectorBase<DT3_> & z)
        {
            DenseVectorBase<DT1_> & temp = x;
            ScaledSum<>::value(temp, y, z);
            return x;
        }

        template <typename DT_>
        static inline DenseVectorMPI<DT_> & value(DenseVectorMPI<DT_> & x, const DenseVectorMPI<DT_> & y, DT_ a)
        {
            MPIOps<tags::CPU>::scaled_sum(x, y, a);
            return x;
        }

        template <typename DT_>
        static inline DenseVectorMPI<DT_> & value(DenseVectorMPI<DT_> & r, const DenseVectorMPI<DT_> & x, const DenseVectorMPI<DT_> & y, DT_ a)
        {
            MPIOps<tags::CPU>::scaled_sum(r, x, y, a);
            return r;
        }

        /// \}

        template <typename DT1_, typename DT2_, typename DT3_>
        static inline BenchmarkInfo get_benchmark_info(DenseVectorBase<DT1_> & a, const DenseVectorBase<DT2_> & b, HONEI_UNUSED DT3_ c)
        {
            BenchmarkInfo result;
            result.flops = a.size() * 2;
            result.load = a.size() * (sizeof(DT1_) + sizeof(DT2_)) + sizeof(DT3_);
            result.store = a.size() * sizeof(DT1_);
            result.size.push_back(a.size());
            result.size.push_back(b.size());
            return result;
        }

        template <typename DT1_, typename DT2_, typename DT3_>
        static inline BenchmarkInfo get_benchmark_info(DenseVectorContinuousBase<DT1_> & a, const DenseVectorContinuousBase<DT2_> & b, HONEI_UNUSED DT3_ c)
        {
            BenchmarkInfo result;
            result.flops = a.size() * 2;
            result.load = a.size() * (sizeof(DT1_) + sizeof(DT2_)) + sizeof(DT3_);
            result.store = a.size() * sizeof(DT1_);
            result.size.push_back(a.size());
            result.size.push_back(b.size());
            return result;
        }
    };

    template <> struct ScaledSum<tags::CPU::Generic>
    {
        template <typename DT_>
        static inline DenseVectorContinuousBase<DT_> & value(DenseVectorContinuousBase<DT_> & x, const DenseVectorContinuousBase<DT_> & y, DT_ a)
        {
            if (x.size() != y.size())
                throw VectorSizeDoesNotMatch(y.size(), x.size());

            BENCHADD(ScaledSum<tags::CPU>::get_benchmark_info(x, y, a));

            const DT_ * ye(y.elements());
            DT_ * xe(x.elements());
            const unsigned long size(x.size());
            for (unsigned long i(0) ; i < size ; ++i)
            {
                xe[i] += ye[i] * a;
            }
            return x;
        }

        template <typename DT_>
        static inline DenseVectorContinuousBase<DT_> & value(DenseVectorContinuousBase<DT_> & r, const DenseVectorContinuousBase<DT_> & x, const DenseVectorContinuousBase<DT_> & y, DT_ a)
        {
            if (x.size() != y.size())
                throw VectorSizeDoesNotMatch(y.size(), x.size());

            if (x.size() != r.size())
                throw VectorSizeDoesNotMatch(r.size(), x.size());

            const DT_ * ye(y.elements());
            const DT_ * xe(x.elements());
            DT_ * re(r.elements());
            const unsigned long size(x.size());
            for (unsigned long i(0) ; i < size ; ++i)
            {
                re[i] = xe[i] + ye[i] * a;
            }
            return r;
        }

        template <typename DT_>
        static inline DenseVectorMPI<DT_> & value(DenseVectorMPI<DT_> & x, const DenseVectorMPI<DT_> & y, DT_ a)
        {
            MPIOps<tags::CPU::Generic>::scaled_sum(x, y, a);
            return x;
        }

        template <typename DT_>
        static inline DenseVectorMPI<DT_> & value(DenseVectorMPI<DT_> & r, const DenseVectorMPI<DT_> & x, const DenseVectorMPI<DT_> & y, DT_ a)
        {
            MPIOps<tags::CPU::Generic>::scaled_sum(r, x, y, a);
            return r;
        }
    };

    /**
     * \brief Scaled sum of two given vectors and a given scalar.
     *
     * ScaledSum is the class template for the operation
     * \f[
     *     \texttt{ScaledSum}(x, a, y): \quad x \leftarrow a \cdot x + y,
     *     \texttt{ScaledSum}(x, y, b): \quad x \leftarrow x + b \cdot y,
     * \f]
     * which yields the scaled sum of x and y.
     *
     * \todo Implement variant using a.
     *
     * \ingroup grplaoperations
     * \ingroup grplavectoroperations
     */
    template <>
    struct ScaledSum<tags::CPU::SSE>
    {
        /**
         * \name Scaled sums
         * \{
         *
         * \brief Returns the vector x as the scaled sum of two given vectors.
         *
         * \param x The vector that shall not be scaled.
         * \param y The vector that shall be scaled.
         * \param b The scale factor.
         *
         * \retval x Will modify x and return it.
         *
         * \exception VectorSizeDoesNotMatch is thrown if the sizes of x and y do not match.
         */

        static DenseVectorContinuousBase<float> & value(DenseVectorContinuousBase<float> & x, const DenseVectorContinuousBase<float> & y, float b);

        static DenseVectorContinuousBase<double> & value(DenseVectorContinuousBase<double> & x, const DenseVectorContinuousBase<double> & y, double b);

        static DenseVectorContinuousBase<float> & value(DenseVectorContinuousBase<float> & a, const DenseVectorContinuousBase<float> & b, const DenseVectorContinuousBase<float> & c );

        static DenseVectorContinuousBase<double> & value(DenseVectorContinuousBase<double> & a, const DenseVectorContinuousBase<double> & b, const DenseVectorContinuousBase<double> & c);

        static DenseVectorContinuousBase<float> & value(DenseVectorContinuousBase<float> & x, const DenseVectorContinuousBase<float> & y, const DenseVectorContinuousBase<float> & z, float b);

        static DenseVectorContinuousBase<double> & value(DenseVectorContinuousBase<double> & x, const DenseVectorContinuousBase<double> & y, const DenseVectorContinuousBase<double> & z, double b);

        template <typename DT_>
        static inline DenseVectorMPI<DT_> & value(DenseVectorMPI<DT_> & x, const DenseVectorMPI<DT_> & y, DT_ a)
        {
            MPIOps<tags::CPU::SSE>::scaled_sum(x, y, a);
            return x;
        }

        template <typename DT_>
        static inline DenseVectorMPI<DT_> & value(DenseVectorMPI<DT_> & r, const DenseVectorMPI<DT_> & x, const DenseVectorMPI<DT_> & y, DT_ a)
        {
            MPIOps<tags::CPU::SSE>::scaled_sum(r, x, y, a);
            return r;
        }

        /// \}
    };

    template <>
    struct ScaledSum<tags::OpenCL::CPU>
    {
        template <typename DT_>
        static DenseVectorContinuousBase<DT_> & value(DenseVectorContinuousBase<DT_> & x, const DenseVectorContinuousBase<DT_> & y, DT_ b);

        template <typename DT_>
        static DenseVectorContinuousBase<DT_> & value(DenseVectorContinuousBase<DT_> & a, const DenseVectorContinuousBase<DT_> & b, const DenseVectorContinuousBase<DT_> & c );

        template <typename DT_>
        static DenseVectorContinuousBase<DT_> & value(DenseVectorContinuousBase<DT_> & x, const DenseVectorContinuousBase<DT_> & y, const DenseVectorContinuousBase<DT_> & z, DT_ b);
    };

    template <>
    struct ScaledSum<tags::OpenCL::GPU>
    {
        template <typename DT_>
        static DenseVectorContinuousBase<DT_> & value(DenseVectorContinuousBase<DT_> & x, const DenseVectorContinuousBase<DT_> & y, DT_ b);

        template <typename DT_>
        static DenseVectorContinuousBase<DT_> & value(DenseVectorContinuousBase<DT_> & a, const DenseVectorContinuousBase<DT_> & b, const DenseVectorContinuousBase<DT_> & c );

        template <typename DT_>
        static DenseVectorContinuousBase<DT_> & value(DenseVectorContinuousBase<DT_> & x, const DenseVectorContinuousBase<DT_> & y, const DenseVectorContinuousBase<DT_> & z, DT_ b);
    };

    /**
     * \brief Scaled sum of two given vectors and a given scalar.
     *
     * ScaledSum is the class template for the operation
     * \f[
     *     \texttt{ScaledSum}(x, a, y): \quad x \leftarrow a \cdot x + y,
     *     \texttt{ScaledSum}(x, y, b): \quad x \leftarrow x + b \cdot y,
     * \f]
     * which yields the scaled sum of x and y.
     *
     * \todo Implement variant using a.
     *
     * \ingroup grplaoperations
     * \ingroup grplavectoroperations
     */
    template <>
    struct ScaledSum<tags::CPU::Itanium>
    {
        /**
         * \name Scaled sums
         * \{
         *
         * \brief Returns the vector x as the scaled sum of two given vectors.
         *
         * \param x The vector that shall not be scaled.
         * \param y The vector that shall be scaled.
         * \param b The scale factor.
         *
         * \retval x Will modify x and return it.
         *
         * \exception VectorSizeDoesNotMatch is thrown if the sizes of x and y do not match.
         */

        template <typename DT1_, typename DT2_>
        static DenseVectorContinuousBase<DT1_> & value(DenseVectorContinuousBase<DT1_> & x, const DenseVectorContinuousBase<DT2_> & y, DT2_ b);
        /// \}
    };

    /**
     * \brief Scaled sum of two given vectors and a given scalar.
     *
     * ScaledSum is the class template for the operation
     * \f[
     *     \texttt{ScaledSum}(x, a, y): \quad x \leftarrow a \cdot x + y,
     *     \texttt{ScaledSum}(x, y, b): \quad x \leftarrow x + b \cdot y,
     * \f]
     * which yields the scaled sum of x and y.
     *
     * \todo Implement variant using a.
     *
     * \ingroup grplaoperations
     * \ingroup grplavectoroperations
     */
    template <>
    struct ScaledSum<tags::GPU::CUDA>
    {
        /**
         * \name Scaled sums
         * \{
         *
         * \brief Returns the vector x as the scaled sum of two given vectors.
         *
         * \param x The vector that shall not be scaled.
         * \param y The vector that shall be scaled.
         * \param b The scale factor.
         *
         * \retval x Will modify x and return it.
         *
         * \exception VectorSizeDoesNotMatch is thrown if the sizes of x and y do not match.
         */

        static DenseVectorContinuousBase<float> & value(DenseVectorContinuousBase<float> & x, const DenseVectorContinuousBase<float> & y, float b);

        static DenseVectorContinuousBase<double> & value(DenseVectorContinuousBase<double> & x, const DenseVectorContinuousBase<double> & y, double b);

        static DenseVectorContinuousBase<float> & value(DenseVectorContinuousBase<float> & result, const DenseVectorContinuousBase<float> & x, const DenseVectorContinuousBase<float> & y, float b);

        static DenseVectorContinuousBase<double> & value(DenseVectorContinuousBase<double> & result, const DenseVectorContinuousBase<double> & x, const DenseVectorContinuousBase<double> & y, double b);

        static DenseVectorContinuousBase<float> & value(DenseVectorContinuousBase<float> & a, const DenseVectorContinuousBase<float> & b, const DenseVectorContinuousBase<float> & c );

        static DenseVectorContinuousBase<double> & value(DenseVectorContinuousBase<double> & a, const DenseVectorContinuousBase<double> & b, const DenseVectorContinuousBase<double> & c );

        template <typename DT_>
        static inline DenseVectorMPI<DT_> & value(DenseVectorMPI<DT_> & x, const DenseVectorMPI<DT_> & y, DT_ a)
        {
            MPIOps<tags::GPU::CUDA>::scaled_sum(x, y, a);
            return x;
        }

        template <typename DT_>
        static inline DenseVectorMPI<DT_> & value(DenseVectorMPI<DT_> & r, const DenseVectorMPI<DT_> & x, const DenseVectorMPI<DT_> & y, DT_ a)
        {
            MPIOps<tags::GPU::CUDA>::scaled_sum(r, x, y, a);
            return r;
        }
        /// \}
    };

    template <>
    struct ScaledSum<tags::GPU::MultiCore::CUDA>
    {
        /**
         * \name Scaled sums
         * \{
         *
         * \brief Returns the vector x as the scaled sum of two given vectors.
         *
         * \param x The vector that shall not be scaled.
         * \param y The vector that shall be scaled.
         * \param b The scale factor.
         *
         * \retval x Will modify x and return it.
         *
         * \exception VectorSizeDoesNotMatch is thrown if the sizes of x and y do not match.
         */

        static DenseVectorContinuousBase<float> & value(DenseVectorContinuousBase<float> & x, const DenseVectorContinuousBase<float> & y, float b);

        static DenseVectorContinuousBase<double> & value(DenseVectorContinuousBase<double> & x, const DenseVectorContinuousBase<double> & y, double b);

        static DenseVectorContinuousBase<float> & value(DenseVectorContinuousBase<float> & result, const DenseVectorContinuousBase<float> & x, const DenseVectorContinuousBase<float> & y, float b);

        static DenseVectorContinuousBase<double> & value(DenseVectorContinuousBase<double> & result, const DenseVectorContinuousBase<double> & x, const DenseVectorContinuousBase<double> & y, double b);

        static DenseVectorContinuousBase<float> & value(DenseVectorContinuousBase<float> & a, const DenseVectorContinuousBase<float> & b, const DenseVectorContinuousBase<float> & c );

        static DenseVectorContinuousBase<double> & value(DenseVectorContinuousBase<double> & a, const DenseVectorContinuousBase<double> & b, const DenseVectorContinuousBase<double> & c );

        /// \}
    };

    /**
     * \brief Scaled sum of two given vectors and a given scalar.
     *
     * ScaledSum is the class template for the operation
     * \f[
     *     \texttt{ScaledSum}(x, a, y): \quad x \leftarrow a \cdot x + y,
     *     \texttt{ScaledSum}(x, y, b): \quad x \leftarrow x + b \cdot y,
     * \f]
     * which yields the scaled sum of x and y.
     *
     * \todo Implement variant using a.
     *
     * \ingroup grplaoperations
     * \ingroup grplavectoroperations
     */
    template <>
    struct ScaledSum<tags::Cell>
    {
        /**
         * \name Scaled sums
         * \{
         *
         * \brief Returns the vector x as the scaled sum of two given vectors.
         *
         * \param x The vector that shall not be scaled.
         * \param y The vector that shall be scaled.
         * \param b The scale factor.
         *
         * \retval x Will modify x and return it.
         *
         * \exception VectorSizeDoesNotMatch is thrown if the sizes of x and y do not match.
         */

        static DenseVectorContinuousBase<float> & value(DenseVectorContinuousBase<float> & x, const DenseVectorContinuousBase<float> & y, const float & b);

        static DenseVectorContinuousBase<double> & value(DenseVectorContinuousBase<double> & x, const DenseVectorContinuousBase<double> & y, const double & b);

        /// \todo Implement real cell version
        static DenseVectorContinuousBase<float> & value(DenseVectorContinuousBase<float> & a, const DenseVectorContinuousBase<float> & b, const DenseVectorContinuousBase<float> & c )
        {
            ScaledSum<tags::CPU>::value(a, b, c);
            return a;
        }

        /// \todo Implement real cell version
        static DenseVectorContinuousBase<double> & value(DenseVectorContinuousBase<double> & a, const DenseVectorContinuousBase<double> & b, const DenseVectorContinuousBase<double> & c)
        {
            ScaledSum<tags::CPU>::value(a, b, c);
            return a;
        }

        static DenseMatrix<float> & value(DenseMatrix<float> & x, const DenseMatrix<float> & y, const float & b);

        /// \}
    };

    namespace mc
    {
        template <typename Tag_> struct ScaledSum
        {
            template <typename DT1_, typename DT2_>
            static DenseVectorBase<DT1_> & value(DenseVectorBase<DT1_> & x, const DenseVectorBase<DT2_> & y, DT2_ b)
            {
                CONTEXT("When calculating ScaledSum (DenseVectorBase, DenseVectorBase, scalar) using backend : " + Tag_::name);

                unsigned long min_part_size(Configuration::instance()->get_value("mc::ScaledSum(DVB,DVB,DT)::min_part_size", 128));
                unsigned long max_count(Configuration::instance()->get_value("mc::ScaledSum(DVB,DVB,DT)::max_count",
                            mc::ThreadPool::instance()->num_threads()));

                Operation<honei::ScaledSum<typename Tag_::DelegateTo> >::op(x, y, b, min_part_size, max_count);

                return x;
            }

            template <typename DT1_, typename DT2_>
            static DenseVectorContinuousBase<DT1_> & value(DenseVectorContinuousBase<DT1_> & x, const DenseVectorContinuousBase<DT2_> & y, DT2_ b)
            {
                CONTEXT("When calculating ScaledSum (DenseVectorContinuousBase, DenseVectorContinuousBase, scalar) using backend : " + Tag_::name);

                unsigned long min_part_size(Configuration::instance()->get_value("mc::ScaledSum(DVCB,DVCB,DT)::min_part_size", 128));
                unsigned long max_count(Configuration::instance()->get_value("mc::ScaledSum(DVCB,DVCB,DT)::max_count",
                            mc::ThreadPool::instance()->num_threads()));

                Operation<honei::ScaledSum<typename Tag_::DelegateTo> >::op(x, y, b, min_part_size, max_count);

                return x;
            }

            template <typename DT1_, typename DT2_>
            static DenseVectorContinuousBase<DT1_> & value(DenseVectorContinuousBase<DT1_> & result, const DenseVectorContinuousBase<DT1_> & x, const DenseVectorContinuousBase<DT2_> & y, DT2_ b)
            {
                CONTEXT("When calculating ScaledSum (DenseVectorContinuousBase, DenseVectorContinuousBase, scalar) using backend : " + Tag_::name);

                unsigned long min_part_size(Configuration::instance()->get_value("mc::ScaledSum(DVCB,DVCB,DT)::min_part_size", 128));
                unsigned long max_count(Configuration::instance()->get_value("mc::ScaledSum(DVCB,DVCB,DT)::max_count",
                            mc::ThreadPool::instance()->num_threads()));

                Operation<honei::ScaledSum<typename Tag_::DelegateTo> >::op(result, x, y, b, min_part_size, max_count);

                return result;
            }

            template <typename DT1_, typename DT2_>
            static DenseVectorBase<DT1_> & value(DenseVectorBase<DT1_> & x, const DenseVectorBase<DT2_> & y, const DenseVectorBase<DT2_> & z)
            {
                CONTEXT("When calculating ScaledSum (DenseVectorBase, DenseVectorBase, DenseVectorBase) using backend : " + Tag_::name);
                unsigned long min_part_size(Configuration::instance()->get_value("mc::ScaledSum(DVB,DVB,DVB)::min_part_size", 128));
                unsigned long max_count(Configuration::instance()->get_value("mc::ScaledSum(DVB,DVB,DVB)::max_count",
                            mc::ThreadPool::instance()->num_threads()));

                Operation<honei::ScaledSum<typename Tag_::DelegateTo> >::op(x, y, z, min_part_size, max_count);

                return x;
            }

            template <typename DT1_, typename DT2_>
            static DenseVectorContinuousBase<DT1_> & value(DenseVectorContinuousBase<DT1_> & x,
                    const DenseVectorContinuousBase<DT2_> & y, const DenseVectorContinuousBase<DT2_> & z)
            {
                CONTEXT("When calculating ScaledSum (DenseVectorContinuousBase, DenseVectorContinuousBase, DenseVectorContinuousBase) using backend : " + Tag_::name);

                unsigned long min_part_size(Configuration::instance()->get_value("mc::ScaledSum(DVCB,DVCB,DVCB)::min_part_size", 128));
                unsigned long max_count(Configuration::instance()->get_value("mc::ScaledSum(DVCB,DVCB,DVCB)::max_count",
                            mc::ThreadPool::instance()->num_threads()));

                Operation<honei::ScaledSum<typename Tag_::DelegateTo> >::op(x, y, z, min_part_size, max_count);

                return x;
            }

            template <typename DT_>
            static inline DenseVectorMPI<DT_> & value(DenseVectorMPI<DT_> & x, const DenseVectorMPI<DT_> & y, DT_ a)
            {
                MPIOps<Tag_>::scaled_sum(x, y, a);
                return x;
            }

            template <typename DT_>
            static inline DenseVectorMPI<DT_> & value(DenseVectorMPI<DT_> & r, const DenseVectorMPI<DT_> & x, const DenseVectorMPI<DT_> & y, DT_ a)
            {
                MPIOps<Tag_>::scaled_sum(r, x, y, a);
                return r;
            }
        };
    }

    template <> struct ScaledSum<tags::CPU::MultiCore> :
        public mc::ScaledSum<tags::CPU::MultiCore>
    {
    };

    template <> struct ScaledSum<tags::CPU::MultiCore::Generic> :
        public mc::ScaledSum<tags::CPU::MultiCore::Generic>
    {
    };

    template <> struct ScaledSum<tags::CPU::MultiCore::SSE> :
        public mc::ScaledSum<tags::CPU::MultiCore::SSE>
    {
    };
}
#endif
