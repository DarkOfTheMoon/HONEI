/* vim: set sw=4 sts=4 et nofoldenable : */

/*
 * Copyright (c) 2008 Dirk Ribbrock <dirk.ribbrock@uni-dortmund.de>
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

#include <honei/backends/cuda/cuda_util.hh>

namespace honei
{
    namespace cuda
    {
        __global__ void scale_gpu(float * x, float a, unsigned long size)
        {
            unsigned long idx = (blockDim.y * blockIdx.y * gridDim.x * blockDim.x) + (blockDim.x * blockIdx.x) + threadIdx.x;
            if (idx < size)
            {
                x[idx] = x[idx] * a;
            }
        }

#ifdef HONEI_CUDA_DOUBLE
        __global__ void scale_gpu(double * x, double a, unsigned long size)
        {
            unsigned long idx = (blockDim.y * blockIdx.y * gridDim.x * blockDim.x) + (blockDim.x * blockIdx.x) + threadIdx.x;
            if (idx < size)
            {
                x[idx] = x[idx] * a;
            }
        }
#endif
    }
}

extern "C" void cuda_scale_one_float(void * x, float a, unsigned long size, unsigned long blocksize)
{
    dim3 grid;
    dim3 block;
    block.x = blocksize;
    grid.x = (unsigned)ceil(sqrt(size/(double)block.x));
    grid.y = grid.x;
    float * x_gpu((float *)x);

    honei::cuda::scale_gpu<<<grid, block>>>(x_gpu, a, size);

    CUDA_ERROR();
}

#ifdef HONEI_CUDA_DOUBLE
extern "C" void cuda_scale_one_double(void * x, double a, unsigned long size, unsigned long blocksize)
{
    dim3 grid;
    dim3 block;
    block.x = blocksize;
    grid.x = (unsigned)ceil(sqrt(size/(double)block.x));
    grid.y = grid.x;
    double * x_gpu((double *)x);

    honei::cuda::scale_gpu<<<grid, block>>>(x_gpu, a, size);

    CUDA_ERROR();
}
#endif
