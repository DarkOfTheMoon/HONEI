/* vim: set sw=4 sts=4 et nofoldenable : */

/*
 * Copyright (c)  2008 Dirk Ribbrock <dirk.ribbrock@uni-dortmund.de>
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
#include <tr1/functional>


using namespace honei;

    class SumTask
    {
        private:
            void * a;
            void * b;
            unsigned long size;
            unsigned long blocksize;
        public:
            SumTask(void * a, void * b, unsigned long size, unsigned long blocksize) :
                a(a),
                b(b),
                size(size),
                blocksize(blocksize)
            {
            }

            void operator() ()
            {
                cuda_sum_two_float(a, b, size, blocksize);
            }
    };

    class SumTask2
    {
        private:
            void * a;
            void * b;
            unsigned long size;
            unsigned long blocksize;
        public:
            SumTask2(void * a, void * b, unsigned long size, unsigned long blocksize) :
                a(a),
                b(b),
                size(size),
                blocksize(blocksize)
            {
            }

            void operator() ()
            {
                cuda_sum_two_double(a, b, size, blocksize);
            }
    };
DenseVectorContinuousBase<float> & Sum<tags::GPU::CUDA>::value(DenseVectorContinuousBase<float> & a,
        const DenseVectorContinuousBase<float> & b)
{
    CONTEXT("When adding DenseVectorContinuousBase<float> to DenseVectorContinuousBase<float> (CUDA):");

    if (a.size() != b.size())
        throw VectorSizeDoesNotMatch(b.size(), a.size());

    unsigned long blocksize(Configuration::instance()->get_value("cuda::sum_two_float", 128ul));

    void * a_gpu (a.lock(lm_read_and_write, tags::GPU::CUDA::memory_value));
    void * b_gpu (b.lock(lm_read_only, tags::GPU::CUDA::memory_value));

    //cuda_sum_two_float(a_gpu, b_gpu, a.size(), blocksize);
    SumTask st(a_gpu, b_gpu, a.size(), blocksize);
    cuda::GPUPool::instance()->enqueue(st,0)->wait();

    b.unlock(lm_read_only);
    a.unlock(lm_read_and_write);

    return a;
}

#ifdef HONEI_CUDA_DOUBLE
DenseVectorContinuousBase<double> & Sum<tags::GPU::CUDA>::value(DenseVectorContinuousBase<double> & a,
        const DenseVectorContinuousBase<double> & b)
{
    CONTEXT("When adding DenseVectorContinuousBase<double> to DenseVectorContinuousBase<double> (CUDA):");

    if (a.size() != b.size())
        throw VectorSizeDoesNotMatch(b.size(), a.size());

    unsigned long blocksize(Configuration::instance()->get_value("cuda::sum_two_double", 128ul));

    void * a_gpu (a.lock(lm_read_and_write, tags::GPU::CUDA::memory_value));
    void * b_gpu (b.lock(lm_read_only, tags::GPU::CUDA::memory_value));
    //cuda_sum_two_double(a_gpu, b_gpu, a.size(), blocksize);
    SumTask2 st(a_gpu, b_gpu, a.size(), blocksize);
    cuda::GPUPool::instance()->enqueue(st,0)->wait();
    b.unlock(lm_read_only);
    a.unlock(lm_read_and_write);

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

    void * a_gpu (a.lock(lm_read_and_write, tags::GPU::CUDA::memory_value));
    void * b_gpu (b.lock(lm_read_only, tags::GPU::CUDA::memory_value));
    //cuda_sum_two_float(a_gpu, b_gpu, a.size(), blocksize);
    SumTask st(a_gpu, b_gpu, a.size(), blocksize);
    cuda::GPUPool::instance()->enqueue(st,0)->wait();
    b.unlock(lm_read_only);
    a.unlock(lm_read_and_write);

    return a;
}
