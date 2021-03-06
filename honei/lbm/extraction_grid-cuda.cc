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

#include <honei/lbm/extraction_grid.hh>
#include <honei/backends/cuda/operations.hh>
#include <honei/backends/cuda/gpu_pool.hh>
#include <honei/util/memory_arbiter.hh>
#include <honei/util/configuration.hh>


using namespace honei;

namespace
{
    class cudaExtractionGridDryfloat
    {
        private:
            PackedGridInfo<D2Q9> & info;
            PackedGridData<D2Q9, float> & data;
            float epsilon;
            unsigned long blocksize;
        public:
            cudaExtractionGridDryfloat(PackedGridInfo<D2Q9> & info, PackedGridData<D2Q9, float> & data, float epsilon, unsigned long blocksize) :
                info(info),
                data(data),
                epsilon(epsilon),
                blocksize(blocksize)
            {
            }

            void operator() ()
            {
                info.limits->lock(lm_read_only);

                void * f_0_gpu(data.f_0->lock(lm_read_and_write, tags::GPU::CUDA::memory_value));
                void * f_1_gpu(data.f_1->lock(lm_read_and_write, tags::GPU::CUDA::memory_value));
                void * f_2_gpu(data.f_2->lock(lm_read_and_write, tags::GPU::CUDA::memory_value));
                void * f_3_gpu(data.f_3->lock(lm_read_and_write, tags::GPU::CUDA::memory_value));
                void * f_4_gpu(data.f_4->lock(lm_read_and_write, tags::GPU::CUDA::memory_value));
                void * f_5_gpu(data.f_5->lock(lm_read_and_write, tags::GPU::CUDA::memory_value));
                void * f_6_gpu(data.f_6->lock(lm_read_and_write, tags::GPU::CUDA::memory_value));
                void * f_7_gpu(data.f_7->lock(lm_read_and_write, tags::GPU::CUDA::memory_value));
                void * f_8_gpu(data.f_8->lock(lm_read_and_write, tags::GPU::CUDA::memory_value));

                void * h_gpu(data.h->lock(lm_write_only, tags::GPU::CUDA::memory_value));
                void * u_gpu(data.u->lock(lm_write_only, tags::GPU::CUDA::memory_value));
                void * v_gpu(data.v->lock(lm_write_only, tags::GPU::CUDA::memory_value));

                void * distribution_x_gpu(data.distribution_x->lock(lm_read_only, tags::GPU::CUDA::memory_value));
                void * distribution_y_gpu(data.distribution_y->lock(lm_read_only, tags::GPU::CUDA::memory_value));

                unsigned long start((*info.limits)[0]);
                unsigned long end((*info.limits)[info.limits->size() - 1]);

                cuda_extraction_grid_dry_float(start, end,
                        f_0_gpu, f_1_gpu, f_2_gpu,
                        f_3_gpu, f_4_gpu, f_5_gpu,
                        f_6_gpu, f_7_gpu, f_8_gpu,
                        h_gpu, u_gpu, v_gpu,
                        distribution_x_gpu, distribution_y_gpu, epsilon,
                        blocksize);

                info.limits->unlock(lm_read_only);

                data.f_0->unlock(lm_read_and_write);
                data.f_1->unlock(lm_read_and_write);
                data.f_2->unlock(lm_read_and_write);
                data.f_3->unlock(lm_read_and_write);
                data.f_4->unlock(lm_read_and_write);
                data.f_5->unlock(lm_read_and_write);
                data.f_6->unlock(lm_read_and_write);
                data.f_7->unlock(lm_read_and_write);
                data.f_8->unlock(lm_read_and_write);

                data.h->unlock(lm_write_only);

                data.distribution_x->unlock(lm_read_only);
                data.distribution_y->unlock(lm_read_only);

                data.u->unlock(lm_write_only);
                data.v->unlock(lm_write_only);
            }
    };

#ifdef HONEI_CUDA_DOUBLE
    class cudaExtractionGridDrydouble
    {
        private:
            PackedGridInfo<D2Q9> & info;
            PackedGridData<D2Q9, double> & data;
            double epsilon;
            unsigned long blocksize;
        public:
            cudaExtractionGridDrydouble(PackedGridInfo<D2Q9> & info, PackedGridData<D2Q9, double> & data, double epsilon, unsigned long blocksize) :
                info(info),
                data(data),
                epsilon(epsilon),
                blocksize(blocksize)
            {
            }

            void operator() ()
            {
                info.limits->lock(lm_read_only);

                void * f_0_gpu(data.f_0->lock(lm_read_and_write, tags::GPU::CUDA::memory_value));
                void * f_1_gpu(data.f_1->lock(lm_read_and_write, tags::GPU::CUDA::memory_value));
                void * f_2_gpu(data.f_2->lock(lm_read_and_write, tags::GPU::CUDA::memory_value));
                void * f_3_gpu(data.f_3->lock(lm_read_and_write, tags::GPU::CUDA::memory_value));
                void * f_4_gpu(data.f_4->lock(lm_read_and_write, tags::GPU::CUDA::memory_value));
                void * f_5_gpu(data.f_5->lock(lm_read_and_write, tags::GPU::CUDA::memory_value));
                void * f_6_gpu(data.f_6->lock(lm_read_and_write, tags::GPU::CUDA::memory_value));
                void * f_7_gpu(data.f_7->lock(lm_read_and_write, tags::GPU::CUDA::memory_value));
                void * f_8_gpu(data.f_8->lock(lm_read_and_write, tags::GPU::CUDA::memory_value));

                void * h_gpu(data.h->lock(lm_write_only, tags::GPU::CUDA::memory_value));
                void * u_gpu(data.u->lock(lm_write_only, tags::GPU::CUDA::memory_value));
                void * v_gpu(data.v->lock(lm_write_only, tags::GPU::CUDA::memory_value));

                void * distribution_x_gpu(data.distribution_x->lock(lm_read_only, tags::GPU::CUDA::memory_value));
                void * distribution_y_gpu(data.distribution_y->lock(lm_read_only, tags::GPU::CUDA::memory_value));

                unsigned long start((*info.limits)[0]);
                unsigned long end((*info.limits)[info.limits->size() - 1]);

                cuda_extraction_grid_dry_double(start, end,
                        f_0_gpu, f_1_gpu, f_2_gpu,
                        f_3_gpu, f_4_gpu, f_5_gpu,
                        f_6_gpu, f_7_gpu, f_8_gpu,
                        h_gpu, u_gpu, v_gpu,
                        distribution_x_gpu, distribution_y_gpu, epsilon,
                        blocksize);

                info.limits->unlock(lm_read_only);

                data.f_0->unlock(lm_read_and_write);
                data.f_1->unlock(lm_read_and_write);
                data.f_2->unlock(lm_read_and_write);
                data.f_3->unlock(lm_read_and_write);
                data.f_4->unlock(lm_read_and_write);
                data.f_5->unlock(lm_read_and_write);
                data.f_6->unlock(lm_read_and_write);
                data.f_7->unlock(lm_read_and_write);
                data.f_8->unlock(lm_read_and_write);

                data.h->unlock(lm_write_only);

                data.distribution_x->unlock(lm_read_only);
                data.distribution_y->unlock(lm_read_only);

                data.u->unlock(lm_write_only);
                data.v->unlock(lm_write_only);
            }
    };
#endif

    class cudaExtractionGridWetfloat
    {
        private:
            PackedGridInfo<D2Q9> & info;
            PackedGridData<D2Q9, float> & data;
            float epsilon;
            unsigned long blocksize;
        public:
            cudaExtractionGridWetfloat(PackedGridInfo<D2Q9> & info, PackedGridData<D2Q9, float> & data, float epsilon, unsigned long blocksize) :
                info(info),
                data(data),
                epsilon(epsilon),
                blocksize(blocksize)
            {
            }

            void operator() ()
            {
                info.limits->lock(lm_read_only);

                void * f_0_gpu(data.f_0->lock(lm_read_and_write, tags::GPU::CUDA::memory_value));
                void * f_1_gpu(data.f_1->lock(lm_read_and_write, tags::GPU::CUDA::memory_value));
                void * f_2_gpu(data.f_2->lock(lm_read_and_write, tags::GPU::CUDA::memory_value));
                void * f_3_gpu(data.f_3->lock(lm_read_and_write, tags::GPU::CUDA::memory_value));
                void * f_4_gpu(data.f_4->lock(lm_read_and_write, tags::GPU::CUDA::memory_value));
                void * f_5_gpu(data.f_5->lock(lm_read_and_write, tags::GPU::CUDA::memory_value));
                void * f_6_gpu(data.f_6->lock(lm_read_and_write, tags::GPU::CUDA::memory_value));
                void * f_7_gpu(data.f_7->lock(lm_read_and_write, tags::GPU::CUDA::memory_value));
                void * f_8_gpu(data.f_8->lock(lm_read_and_write, tags::GPU::CUDA::memory_value));

                void * h_gpu(data.h->lock(lm_write_only, tags::GPU::CUDA::memory_value));
                void * u_gpu(data.u->lock(lm_write_only, tags::GPU::CUDA::memory_value));
                void * v_gpu(data.v->lock(lm_write_only, tags::GPU::CUDA::memory_value));

                void * distribution_x_gpu(data.distribution_x->lock(lm_read_only, tags::GPU::CUDA::memory_value));
                void * distribution_y_gpu(data.distribution_y->lock(lm_read_only, tags::GPU::CUDA::memory_value));

                unsigned long start((*info.limits)[0]);
                unsigned long end((*info.limits)[info.limits->size() - 1]);

                cuda_extraction_grid_wet_float(start, end,
                        f_0_gpu, f_1_gpu, f_2_gpu,
                        f_3_gpu, f_4_gpu, f_5_gpu,
                        f_6_gpu, f_7_gpu, f_8_gpu,
                        h_gpu, u_gpu, v_gpu,
                        distribution_x_gpu, distribution_y_gpu, epsilon,
                        blocksize);

                info.limits->unlock(lm_read_only);

                data.f_0->unlock(lm_read_and_write);
                data.f_1->unlock(lm_read_and_write);
                data.f_2->unlock(lm_read_and_write);
                data.f_3->unlock(lm_read_and_write);
                data.f_4->unlock(lm_read_and_write);
                data.f_5->unlock(lm_read_and_write);
                data.f_6->unlock(lm_read_and_write);
                data.f_7->unlock(lm_read_and_write);
                data.f_8->unlock(lm_read_and_write);

                data.h->unlock(lm_write_only);

                data.distribution_x->unlock(lm_read_only);
                data.distribution_y->unlock(lm_read_only);

                data.u->unlock(lm_write_only);
                data.v->unlock(lm_write_only);
            }
    };

#ifdef HONEI_CUDA_DOUBLE
    class cudaExtractionGridWetdouble
    {
        private:
            PackedGridInfo<D2Q9> & info;
            PackedGridData<D2Q9, double> & data;
            double epsilon;
            unsigned long blocksize;
        public:
            cudaExtractionGridWetdouble(PackedGridInfo<D2Q9> & info, PackedGridData<D2Q9, double> & data, double epsilon, unsigned long blocksize) :
                info(info),
                data(data),
                epsilon(epsilon),
                blocksize(blocksize)
            {
            }

            void operator() ()
            {
                info.limits->lock(lm_read_only);

                void * f_0_gpu(data.f_0->lock(lm_read_and_write, tags::GPU::CUDA::memory_value));
                void * f_1_gpu(data.f_1->lock(lm_read_and_write, tags::GPU::CUDA::memory_value));
                void * f_2_gpu(data.f_2->lock(lm_read_and_write, tags::GPU::CUDA::memory_value));
                void * f_3_gpu(data.f_3->lock(lm_read_and_write, tags::GPU::CUDA::memory_value));
                void * f_4_gpu(data.f_4->lock(lm_read_and_write, tags::GPU::CUDA::memory_value));
                void * f_5_gpu(data.f_5->lock(lm_read_and_write, tags::GPU::CUDA::memory_value));
                void * f_6_gpu(data.f_6->lock(lm_read_and_write, tags::GPU::CUDA::memory_value));
                void * f_7_gpu(data.f_7->lock(lm_read_and_write, tags::GPU::CUDA::memory_value));
                void * f_8_gpu(data.f_8->lock(lm_read_and_write, tags::GPU::CUDA::memory_value));

                void * h_gpu(data.h->lock(lm_write_only, tags::GPU::CUDA::memory_value));
                void * u_gpu(data.u->lock(lm_write_only, tags::GPU::CUDA::memory_value));
                void * v_gpu(data.v->lock(lm_write_only, tags::GPU::CUDA::memory_value));

                void * distribution_x_gpu(data.distribution_x->lock(lm_read_only, tags::GPU::CUDA::memory_value));
                void * distribution_y_gpu(data.distribution_y->lock(lm_read_only, tags::GPU::CUDA::memory_value));

                unsigned long start((*info.limits)[0]);
                unsigned long end((*info.limits)[info.limits->size() - 1]);

                cuda_extraction_grid_wet_double(start, end,
                        f_0_gpu, f_1_gpu, f_2_gpu,
                        f_3_gpu, f_4_gpu, f_5_gpu,
                        f_6_gpu, f_7_gpu, f_8_gpu,
                        h_gpu, u_gpu, v_gpu,
                        distribution_x_gpu, distribution_y_gpu, epsilon,
                        blocksize);

                info.limits->unlock(lm_read_only);

                data.f_0->unlock(lm_read_and_write);
                data.f_1->unlock(lm_read_and_write);
                data.f_2->unlock(lm_read_and_write);
                data.f_3->unlock(lm_read_and_write);
                data.f_4->unlock(lm_read_and_write);
                data.f_5->unlock(lm_read_and_write);
                data.f_6->unlock(lm_read_and_write);
                data.f_7->unlock(lm_read_and_write);
                data.f_8->unlock(lm_read_and_write);

                data.h->unlock(lm_write_only);

                data.distribution_x->unlock(lm_read_only);
                data.distribution_y->unlock(lm_read_only);

                data.u->unlock(lm_write_only);
                data.v->unlock(lm_write_only);
            }
    };
#endif
}

void ExtractionGrid<tags::GPU::CUDA, lbm_modes::DRY>::value(
        PackedGridInfo<lbm_lattice_types::D2Q9> & info,
        PackedGridData<lbm_lattice_types::D2Q9, float> & data, float epsilon)
{
    CONTEXT("When extracting h, u and v (CUDA):");

    unsigned long blocksize(Configuration::instance()->get_value("cuda::extraction_grid_float", 128ul));

    //set f to t_temp
    DenseVector<float> * swap;
    swap = data.f_0;
    data.f_0 = data.f_temp_0;
    data.f_temp_0 = swap;
    swap = data.f_1;
    data.f_1 = data.f_temp_1;
    data.f_temp_1 = swap;
    swap = data.f_2;
    data.f_2 = data.f_temp_2;
    data.f_temp_2 = swap;
    swap = data.f_3;
    data.f_3 = data.f_temp_3;
    data.f_temp_3 = swap;
    swap = data.f_4;
    data.f_4 = data.f_temp_4;
    data.f_temp_4 = swap;
    swap = data.f_5;
    data.f_5 = data.f_temp_5;
    data.f_temp_5 = swap;
    swap = data.f_6;
    data.f_6 = data.f_temp_6;
    data.f_temp_6 = swap;
    swap = data.f_7;
    data.f_7 = data.f_temp_7;
    data.f_temp_7 = swap;
    swap = data.f_8;
    data.f_8 = data.f_temp_8;
    data.f_temp_8 = swap;

    if (! cuda::GPUPool::instance()->idle())
    {
        cudaExtractionGridDryfloat task(info, data, epsilon, blocksize);
        task();
    }
    else
    {
        cudaExtractionGridDryfloat task(info, data, epsilon, blocksize);
        cuda::GPUPool::instance()->enqueue(task, 0).wait();
    }
}

#ifdef HONEI_CUDA_DOUBLE
void ExtractionGrid<tags::GPU::CUDA, lbm_modes::DRY>::value(
        PackedGridInfo<lbm_lattice_types::D2Q9> & info,
        PackedGridData<lbm_lattice_types::D2Q9, double> & data, double epsilon)
{
    CONTEXT("When extracting h, u and v (CUDA):");

    unsigned long blocksize(Configuration::instance()->get_value("cuda::extraction_grid_double", 128ul));

    //set f to t_temp
    DenseVector<double> * swap;
    swap = data.f_0;
    data.f_0 = data.f_temp_0;
    data.f_temp_0 = swap;
    swap = data.f_1;
    data.f_1 = data.f_temp_1;
    data.f_temp_1 = swap;
    swap = data.f_2;
    data.f_2 = data.f_temp_2;
    data.f_temp_2 = swap;
    swap = data.f_3;
    data.f_3 = data.f_temp_3;
    data.f_temp_3 = swap;
    swap = data.f_4;
    data.f_4 = data.f_temp_4;
    data.f_temp_4 = swap;
    swap = data.f_5;
    data.f_5 = data.f_temp_5;
    data.f_temp_5 = swap;
    swap = data.f_6;
    data.f_6 = data.f_temp_6;
    data.f_temp_6 = swap;
    swap = data.f_7;
    data.f_7 = data.f_temp_7;
    data.f_temp_7 = swap;
    swap = data.f_8;
    data.f_8 = data.f_temp_8;
    data.f_temp_8 = swap;

    if (! cuda::GPUPool::instance()->idle())
    {
        cudaExtractionGridDrydouble task(info, data, epsilon, blocksize);
        task();
    }
    else
    {
        cudaExtractionGridDrydouble task(info, data, epsilon, blocksize);
        cuda::GPUPool::instance()->enqueue(task, 0).wait();
    }
}
#endif

void ExtractionGrid<tags::GPU::CUDA, lbm_modes::WET>::value(
        PackedGridInfo<lbm_lattice_types::D2Q9> & info,
        PackedGridData<lbm_lattice_types::D2Q9, float> & data, float epsilon)
{
    CONTEXT("When extracting h, u and v (CUDA):");

    unsigned long blocksize(Configuration::instance()->get_value("cuda::extraction_grid_float", 128ul));

    //set f to t_temp
    DenseVector<float> * swap;
    swap = data.f_0;
    data.f_0 = data.f_temp_0;
    data.f_temp_0 = swap;
    swap = data.f_1;
    data.f_1 = data.f_temp_1;
    data.f_temp_1 = swap;
    swap = data.f_2;
    data.f_2 = data.f_temp_2;
    data.f_temp_2 = swap;
    swap = data.f_3;
    data.f_3 = data.f_temp_3;
    data.f_temp_3 = swap;
    swap = data.f_4;
    data.f_4 = data.f_temp_4;
    data.f_temp_4 = swap;
    swap = data.f_5;
    data.f_5 = data.f_temp_5;
    data.f_temp_5 = swap;
    swap = data.f_6;
    data.f_6 = data.f_temp_6;
    data.f_temp_6 = swap;
    swap = data.f_7;
    data.f_7 = data.f_temp_7;
    data.f_temp_7 = swap;
    swap = data.f_8;
    data.f_8 = data.f_temp_8;
    data.f_temp_8 = swap;

    if (! cuda::GPUPool::instance()->idle())
    {
        cudaExtractionGridWetfloat task(info, data, epsilon, blocksize);
        task();
    }
    else
    {
        cudaExtractionGridWetfloat task(info, data, epsilon, blocksize);
        cuda::GPUPool::instance()->enqueue(task, 0).wait();
    }
}

#ifdef HONEI_CUDA_DOUBLE
void ExtractionGrid<tags::GPU::CUDA, lbm_modes::WET>::value(
        PackedGridInfo<lbm_lattice_types::D2Q9> & info,
        PackedGridData<lbm_lattice_types::D2Q9, double> & data, double epsilon)
{
    CONTEXT("When extracting h, u and v (CUDA):");

    unsigned long blocksize(Configuration::instance()->get_value("cuda::extraction_grid_double", 128ul));

    //set f to t_temp
    DenseVector<double> * swap;
    swap = data.f_0;
    data.f_0 = data.f_temp_0;
    data.f_temp_0 = swap;
    swap = data.f_1;
    data.f_1 = data.f_temp_1;
    data.f_temp_1 = swap;
    swap = data.f_2;
    data.f_2 = data.f_temp_2;
    data.f_temp_2 = swap;
    swap = data.f_3;
    data.f_3 = data.f_temp_3;
    data.f_temp_3 = swap;
    swap = data.f_4;
    data.f_4 = data.f_temp_4;
    data.f_temp_4 = swap;
    swap = data.f_5;
    data.f_5 = data.f_temp_5;
    data.f_temp_5 = swap;
    swap = data.f_6;
    data.f_6 = data.f_temp_6;
    data.f_temp_6 = swap;
    swap = data.f_7;
    data.f_7 = data.f_temp_7;
    data.f_temp_7 = swap;
    swap = data.f_8;
    data.f_8 = data.f_temp_8;
    data.f_temp_8 = swap;

    if (! cuda::GPUPool::instance()->idle())
    {
        cudaExtractionGridWetdouble task(info, data, epsilon, blocksize);
        task();
    }
    else
    {
        cudaExtractionGridWetdouble task(info, data, epsilon, blocksize);
        cuda::GPUPool::instance()->enqueue(task, 0).wait();
    }
}
#endif
