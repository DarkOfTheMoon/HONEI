/* vim: set sw=4 sts=4 et nofoldenable : */

/*
 * Copyright (c)  2008, 2010 Dirk Ribbrock <dirk.ribbrock@uni-dortmund.de>
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

#include <honei/la/sum.hh>
#include <honei/backends/cuda/operations.hh>
#include <honei/backends/cuda/gpu_pool.hh>
#include <honei/util/memory_arbiter.hh>
#include <honei/util/configuration.hh>
#include <honei/util/profiler.hh>

using namespace honei;

namespace
{
    class cudaSumDVfloat
    {
        private:
            DenseVectorContinuousBase<float> & a;
            const DenseVectorContinuousBase<float> & b;
            unsigned long blocksize;
        public:
            cudaSumDVfloat(DenseVectorContinuousBase<float> & a, const DenseVectorContinuousBase<float> & b, unsigned long blocksize) :
                a(a),
                b(b),
                blocksize(blocksize)
            {
            }

            void operator() ()
            {
                void * a_gpu (a.lock(lm_read_and_write, tags::GPU::CUDA::memory_value));
                void * b_gpu (b.lock(lm_read_only, tags::GPU::CUDA::memory_value));

                cuda_sum_two_float(a_gpu, b_gpu, a.size(), blocksize);

                b.unlock(lm_read_only);
                a.unlock(lm_read_and_write);
            }
    };

    class cudaSumDMfloat
    {
        private:
            DenseMatrix<float> & a;
            const DenseMatrix<float> & b;
            unsigned long blocksize;
        public:
            cudaSumDMfloat(DenseMatrix<float> & a, const DenseMatrix<float> & b, unsigned long blocksize) :
                a(a),
                b(b),
                blocksize(blocksize)
            {
            }

            void operator() ()
            {
                void * a_gpu (a.lock(lm_read_and_write, tags::GPU::CUDA::memory_value));
                void * b_gpu (b.lock(lm_read_only, tags::GPU::CUDA::memory_value));

                cuda_sum_two_float(a_gpu, b_gpu, a.size(), blocksize);

                b.unlock(lm_read_only);
                a.unlock(lm_read_and_write);
            }
    };

    class cudaSumDVdouble
    {
        private:
            DenseVectorContinuousBase<double> & a;
            const DenseVectorContinuousBase<double> & b;
            unsigned long blocksize;
        public:
            cudaSumDVdouble(DenseVectorContinuousBase<double> & a, const DenseVectorContinuousBase<double> & b, unsigned long blocksize) :
                a(a),
                b(b),
                blocksize(blocksize)
            {
            }

            void operator() ()
            {
                void * a_gpu (a.lock(lm_read_and_write, tags::GPU::CUDA::memory_value));
                void * b_gpu (b.lock(lm_read_only, tags::GPU::CUDA::memory_value));

                cuda_sum_two_double(a_gpu, b_gpu, a.size(), blocksize);

                b.unlock(lm_read_only);
                a.unlock(lm_read_and_write);
            }
    };
}

DenseVectorContinuousBase<float> & Sum<tags::GPU::CUDA>::value(DenseVectorContinuousBase<float> & a,
        const DenseVectorContinuousBase<float> & b)
{
    CONTEXT("When adding DenseVectorContinuousBase<float> to DenseVectorContinuousBase<float> (CUDA):");

    if (a.size() != b.size())
        throw VectorSizeDoesNotMatch(b.size(), a.size());

    unsigned long blocksize(Configuration::instance()->get_value("cuda::sum_two_float", 128ul));

    if (! cuda::GPUPool::instance()->idle())
    {
        cudaSumDVfloat task(a, b, blocksize);
        task();
    }
    else
    {
        cudaSumDVfloat task(a, b, blocksize);
        cuda::GPUPool::instance()->enqueue(task, 0).wait();
    }

    return a;
}

#ifdef HONEI_CUDA_DOUBLE
DenseVectorContinuousBase<double> & Sum<tags::GPU::CUDA>::value(DenseVectorContinuousBase<double> & a,
        const DenseVectorContinuousBase<double> & b)
{
    CONTEXT("When adding DenseVectorContinuousBase<double> to DenseVectorContinuousBase<double> (CUDA):");
    PROFILER_START("Sum DV double tags::GPU::CUDA");

    if (a.size() != b.size())
        throw VectorSizeDoesNotMatch(b.size(), a.size());

    unsigned long blocksize(Configuration::instance()->get_value("cuda::sum_two_double", 128ul));

    if (! cuda::GPUPool::instance()->idle())
    {
        cudaSumDVdouble task(a, b, blocksize);
        task();
    }
    else
    {
        cudaSumDVdouble task(a, b, blocksize);
        cuda::GPUPool::instance()->enqueue(task, 0).wait();
    }

    PROFILER_STOP("Sum DV double tags::GPU::CUDA");
    return a;
}
#endif

DenseMatrix<float> & Sum<tags::GPU::CUDA>::value(DenseMatrix<float> & a, const DenseMatrix<float> & b)
{
    CONTEXT("When adding DenseMatrix<float> to DenseMatrix<float> (CUDA):");

    if (a.columns() != b.columns())
    {
        throw MatrixColumnsDoNotMatch(b.columns(), a.columns());
    }

    if (a.rows() != b.rows())
    {
        throw MatrixRowsDoNotMatch(b.rows(), a.rows());
    }

    unsigned long blocksize(Configuration::instance()->get_value("cuda::sum_two_float", 128ul));

    if (! cuda::GPUPool::instance()->idle())
    {
        cudaSumDMfloat task(a, b, blocksize);
        task();
    }
    else
    {
        cudaSumDMfloat task(a, b, blocksize);
        cuda::GPUPool::instance()->enqueue(task, 0).wait();
    }

    return a;
}

DenseVectorContinuousBase<float> & Sum<tags::GPU::MultiCore::CUDA>::value(DenseVectorContinuousBase<float> & a,
        const DenseVectorContinuousBase<float> & b)
{
    CONTEXT("When adding DenseVectorContinuousBase<float> to DenseVectorContinuousBase<float> (MC CUDA):");

    if (a.size() != b.size())
        throw VectorSizeDoesNotMatch(b.size(), a.size());

    unsigned long blocksize(Configuration::instance()->get_value("cuda::sum_two_float", 128ul));

    if (! cuda::GPUPool::instance()->idle())
    {
        throw InternalError("You should not run this operation within any MC CUDA op!");
        //cudaSumDVfloat task(a, b, blocksize);
        //task();
    }
    else
    {
        DenseVectorRange<float> a1(a.range(a.size()/2, 0));
        DenseVectorRange<float> b1(b.range(b.size()/2, 0));
        cudaSumDVfloat task1(a1, b1, blocksize);
        DenseVectorRange<float> a2(a.range(a.size()/2 + a.size()%2, a.size()/2));
        DenseVectorRange<float> b2(b.range(b.size()/2 + b.size()%2, b.size()/2));
        cudaSumDVfloat task2(a2, b2, blocksize);
        cuda::GPUPool::instance()->enqueue(task1, 0).wait();
        cuda::GPUPool::instance()->enqueue(task2, 1).wait();
    }

    return a;
}

#ifdef HONEI_CUDA_DOUBLE
DenseVectorContinuousBase<double> & Sum<tags::GPU::MultiCore::CUDA>::value(DenseVectorContinuousBase<double> & a,
        const DenseVectorContinuousBase<double> & b)
{
    CONTEXT("When adding DenseVectorContinuousBase<double> to DenseVectorContinuousBase<double> (MC CUDA):");

    if (a.size() != b.size())
        throw VectorSizeDoesNotMatch(b.size(), a.size());

    unsigned long blocksize(Configuration::instance()->get_value("cuda::sum_two_double", 128ul));

    if (! cuda::GPUPool::instance()->idle())
    {
        throw InternalError("You should not run this operation within any MC CUDA op!");
        //cudaSumDVdouble task(a, b, blocksize);
        //task();
    }
    else
    {
        DenseVectorRange<double> a1(a.range(a.size()/2, 0));
        DenseVectorRange<double> b1(b.range(b.size()/2, 0));
        cudaSumDVdouble task1(a1, b1, blocksize);
        DenseVectorRange<double> a2(a.range(a.size()/2 + a.size()%2, a.size()/2));
        DenseVectorRange<double> b2(b.range(b.size()/2 + b.size()%2, b.size()/2));
        cudaSumDVdouble task2(a2, b2, blocksize);
        cuda::GPUPool::instance()->enqueue(task1, 0).wait();
        cuda::GPUPool::instance()->enqueue(task2, 1).wait();
    }

    return a;
}
#endif
