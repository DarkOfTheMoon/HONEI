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

#include <honei/math/restriction.hh>
#include <honei/backends/cuda/operations.hh>
#include <honei/backends/cuda/gpu_pool.hh>
#include <honei/util/memory_arbiter.hh>
#include <honei/util/configuration.hh>


using namespace honei;

namespace
{
    class cudaRestrictionDVfloat
    {
        private:
            DenseVector<float> & coarse;
            const DenseVector<float> & fine;
            const DenseVector<unsigned long> & mask;
            unsigned long blocksize;
        public:
            cudaRestrictionDVfloat(DenseVector<float> & coarse, const DenseVector<float> & fine, const DenseVector<unsigned long> & mask, unsigned long blocksize) :
                coarse(coarse),
                fine(fine),
                mask(mask),
                blocksize(blocksize)
            {
            }

            void operator() ()
            {
                void * coarse_gpu (coarse.lock(lm_write_only, tags::GPU::CUDA::memory_value));
                void * fine_gpu (fine.lock(lm_read_only, tags::GPU::CUDA::memory_value));
                cuda_restriction_float(coarse_gpu, coarse.size(), fine_gpu, fine.size(), mask.elements(), blocksize);
                fine.unlock(lm_read_only);
                coarse.unlock(lm_write_only);
            }
    };

    class cudaRestrictionDVdouble
    {
        private:
            DenseVector<double> & coarse;
            const DenseVector<double> & fine;
            const DenseVector<unsigned long> & mask;
            unsigned long blocksize;
        public:
            cudaRestrictionDVdouble(DenseVector<double> & coarse, const DenseVector<double> & fine, const DenseVector<unsigned long> & mask, unsigned long blocksize) :
                coarse(coarse),
                fine(fine),
                mask(mask),
                blocksize(blocksize)
            {
            }

            void operator() ()
            {
                void * coarse_gpu (coarse.lock(lm_write_only, tags::GPU::CUDA::memory_value));
                void * fine_gpu (fine.lock(lm_read_only, tags::GPU::CUDA::memory_value));
                cuda_restriction_double(coarse_gpu, coarse.size(), fine_gpu, fine.size(), mask.elements(), blocksize);
                fine.unlock(lm_read_only);
                coarse.unlock(lm_write_only);
            }
    };
}

DenseVector<float> & Restriction<tags::GPU::CUDA, methods::NONE>::value(DenseVector<float> & coarse,
        const DenseVector<float> & fine, const DenseVector<unsigned long> & mask, HONEI_UNUSED BandedMatrixQx<Q1Type, float> & resmat)
{
    CONTEXT("When restricting from fine to coarse (CUDA):");

    unsigned long blocksize(Configuration::instance()->get_value("cuda::restriction_float", 64ul));

    if (! cuda::GPUPool::instance()->idle())
    {
        cudaRestrictionDVfloat task(coarse, fine, mask, blocksize);
        task();
    }
    else
    {
        cudaRestrictionDVfloat task(coarse, fine, mask, blocksize);
        cuda::GPUPool::instance()->enqueue(task, 0).wait();
    }

    return coarse;
}
DenseVector<float> & Restriction<tags::GPU::CUDA, methods::NONE>::value(DenseVector<float> & coarse,
        const DenseVector<float> & fine, const DenseVector<unsigned long> & mask, HONEI_UNUSED SparseMatrixELL<float> & resmat)
{
    CONTEXT("When restricting from fine to coarse (CUDA):");

    unsigned long blocksize(Configuration::instance()->get_value("cuda::restriction_float", 64ul));

    if (! cuda::GPUPool::instance()->idle())
    {
        cudaRestrictionDVfloat task(coarse, fine, mask, blocksize);
        task();
    }
    else
    {
        cudaRestrictionDVfloat task(coarse, fine, mask, blocksize);
        cuda::GPUPool::instance()->enqueue(task, 0).wait();
    }

    return coarse;
}

#ifdef HONEI_CUDA_DOUBLE
DenseVector<double> & Restriction<tags::GPU::CUDA, methods::NONE>::value(DenseVector<double> & coarse,
        const DenseVector<double> & fine, const DenseVector<unsigned long> & mask, HONEI_UNUSED BandedMatrixQx<Q1Type, double> & resmat)
{
    CONTEXT("When restricting from fine to coarse (CUDA):");

    unsigned long blocksize(Configuration::instance()->get_value("cuda::restriction_double", 64ul));

    if (! cuda::GPUPool::instance()->idle())
    {
        cudaRestrictionDVdouble task(coarse, fine, mask, blocksize);
        task();
    }
    else
    {
        cudaRestrictionDVdouble task(coarse, fine, mask, blocksize);
        cuda::GPUPool::instance()->enqueue(task, 0).wait();
    }

    return coarse;
}
DenseVector<double> & Restriction<tags::GPU::CUDA, methods::NONE>::value(DenseVector<double> & coarse,
        const DenseVector<double> & fine, const DenseVector<unsigned long> & mask, HONEI_UNUSED SparseMatrixELL<double> & resmat)
{
    CONTEXT("When restricting from fine to coarse (CUDA):");

    unsigned long blocksize(Configuration::instance()->get_value("cuda::restriction_double", 64ul));

    if (! cuda::GPUPool::instance()->idle())
    {
        cudaRestrictionDVdouble task(coarse, fine, mask, blocksize);
        task();
    }
    else
    {
        cudaRestrictionDVdouble task(coarse, fine, mask, blocksize);
        cuda::GPUPool::instance()->enqueue(task, 0).wait();
    }

    return coarse;
}
#endif
