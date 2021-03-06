/* vim: set sw=4 sts=4 et foldmethod=syntax : */

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
#include <iostream>

namespace honei
{
    namespace cuda
    {
#ifdef HONEI_CUDA_DOUBLE
        __global__ void convert_float_double_gpu(float * src, double * dest, unsigned long size)
        {
            unsigned long idx = (blockDim.y * blockIdx.y * gridDim.x * blockDim.x) + (blockDim.x * blockIdx.x) + threadIdx.x;
            if (idx < size)
            {
                dest[idx] = src[idx];
            }
        }

        __global__ void convert_double_float_gpu(double * src, float * dest, unsigned long size)
        {
            unsigned long idx = (blockDim.y * blockIdx.y * gridDim.x * blockDim.x) + (blockDim.x * blockIdx.x) + threadIdx.x;
            if (idx < size)
            {
                dest[idx] = src[idx];
            }
        }
#endif
    }
}

extern "C" void * cuda_malloc_host(unsigned long bytes)
{
    void * cpu(NULL);
    if (cudaErrorMemoryAllocation == cudaMallocHost((void**)&cpu, bytes))
        return 0;
    CUDA_ERROR();
    return cpu;
}

extern "C" void * cuda_malloc(unsigned long bytes)
{
    void * gpu(NULL);
    if (cudaErrorMemoryAllocation == cudaMalloc((void**)&gpu, bytes))
        return 0;
    CUDA_ERROR();
    return gpu;
}

extern "C" void cuda_upload(void * cpu, void * gpu, unsigned long bytes)
{
    cudaMemcpy(gpu, cpu, bytes, cudaMemcpyHostToDevice);
    CUDA_ERROR();
}

extern "C" void cuda_download(void * gpu, void * cpu, unsigned long bytes)
{
    cudaMemcpy(cpu, gpu, bytes, cudaMemcpyDeviceToHost);
    CUDA_ERROR();
}

extern "C" void cuda_upload_async(void * cpu, void * gpu, unsigned long bytes, cudaStream_t stream)
{
    cudaMemcpyAsync(gpu, cpu, bytes, cudaMemcpyHostToDevice, stream);
    CUDA_ERROR();
}

extern "C" void cuda_download_async(void * gpu, void * cpu, unsigned long bytes, cudaStream_t stream)
{
    cudaMemcpyAsync(cpu, gpu, bytes, cudaMemcpyDeviceToHost, stream);
    CUDA_ERROR();
}

extern "C" void cuda_free(void * gpu)
{
    cudaFree(gpu);
    CUDA_ERROR();
}

extern "C" void cuda_free_host(void * cpu)
{
    cudaFreeHost(cpu);
    CUDA_ERROR();
}

extern "C" void cuda_copy(void * src, void * dest, unsigned long bytes)
{
    cudaMemcpy(dest, src, bytes, cudaMemcpyDeviceToDevice);
    CUDA_ERROR();
}

#ifdef HONEI_CUDA_DOUBLE
extern "C" void cuda_convert_float_double(void * src, void * dest, unsigned long bytes)
{
    unsigned long size = bytes / sizeof(float);
    dim3 grid;
    dim3 block;
    block.x = 128;
    grid.x = (unsigned)ceil(sqrt(size/(double)block.x));
    grid.y = grid.x;
    float * src_gpu((float *)src);
    double * dest_gpu((double *)dest);

    honei::cuda::convert_float_double_gpu<<<grid, block>>>(src_gpu, dest_gpu, size);
    CUDA_ERROR();
}

extern "C" void cuda_convert_double_float(void * src, void * dest, unsigned long bytes)
{
    unsigned long size = bytes / sizeof(double);
    dim3 grid;
    dim3 block;
    block.x = 128;
    grid.x = (unsigned)ceil(sqrt(size/(double)block.x));
    grid.y = grid.x;
    double * src_gpu((double *)src);
    float * dest_gpu((float *)dest);

    honei::cuda::convert_double_float_gpu<<<grid, block>>>(src_gpu, dest_gpu, size);
    CUDA_ERROR();
}
#else
extern "C" void cuda_convert_float_double(void * src, void * dest, unsigned long bytes)
{
    std::cout<<"cuda convert not available without --enable-cuda_double!";
    exit(1);
}

extern "C" void cuda_convert_double_float(void * src, void * dest, unsigned long bytes)
{
    std::cout<<"cuda convert not available without --enable-cuda_double!";
    exit(1);
}
#endif

extern "C" void cuda_fill_zero(void * dest, unsigned long bytes)
{
    cudaMemset(dest, 0, bytes);
    CUDA_ERROR();
}


