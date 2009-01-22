/* vim: set number sw=4 sts=4 et nofoldenable : */

/*
 * Copyright (c) 2009 Dirk Ribbrock <dirk.ribbrock@uni-dortmund.de>
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

#ifndef ALLBENCH
#include <benchmark/benchmark.cc>
#include <tr1/memory>
#include <string>
#endif

#include <honei/lbm/equilibrium_distribution_grid.hh>
#include <honei/lbm/grid_packer.hh>
#include <honei/lbm/solver_labswe_grid.hh>
#include <honei/swe/volume.hh>
#include <honei/math/quadrature.hh>
#include <honei/backends/cuda/operations.hh>
#include <honei/lbm/partial_derivative.hh>

using namespace std;
using namespace honei;
using namespace lbm;
using namespace lbm_lattice_types;

template <typename Tag_, typename DataType_, typename Dir_>
class EquilibriumDistributionGridBench :
    public Benchmark
{
    private:
        unsigned long _size;
        int _count;
    public:
        EquilibriumDistributionGridBench(const std::string & id, unsigned long size, int count) :
            Benchmark(id)
        {
            register_tag(Tag_::name);
            _size = size;
            _count = count;
        }

        virtual void run()
        {
            unsigned long g_h(_size);
            unsigned long g_w(_size);

            DenseMatrix<DataType_> h(g_h, g_w, DataType_(0.05));
            Cylinder<DataType_> c1(h, DataType_(0.02), 25, 25);
            c1.value();

            DenseMatrix<DataType_> u(g_h, g_w, DataType_(0.));
            DenseMatrix<DataType_> v(g_h, g_w, DataType_(0.));

            DenseMatrix<DataType_> b(g_h, g_w, DataType_(0.));

            Cylinder<DataType_> b1(b, DataType_(0.04), 15, 15);
            b1.value();

            DenseMatrix<DataType_> b_x(PartialDerivative<Tag_, X, CENTRALDIFF>::value(b , DataType_(1)));
            DenseMatrix<DataType_> b_y(PartialDerivative<Tag_, Y, CENTRALDIFF>::value(b , DataType_(1)));

            Grid<D2Q9, DataType_> grid;
            DenseMatrix<bool> obstacles(g_h, g_w, false);
            grid.obstacles = &obstacles;
            grid.h = &h;
            grid.u = &u;
            grid.v = &v;
            grid.b_x = &b_x;
            grid.b_y = &b_y;

            PackedGridData<D2Q9, DataType_>  data;
            PackedGridInfo<D2Q9> info;

            GridPacker<D2Q9, NOSLIP, DataType_>::pack(grid, info, data);

            SolverLABSWEGrid<Tag_, DataType_,lbm_source_types::CENTRED, lbm_source_schemes::CENTRALDIFF, lbm_grid_types::RECTANGULAR, lbm_lattice_types::D2Q9, lbm_boundary_types::NOSLIP> solver(&data, &info, 1., 1., 1.);

            solver.do_preprocessing();

            DataType_ g(9.81);
            DataType_ e(1.);

            for(int i = 0; i < _count; ++i)
            {
                BENCHMARK((EquilibriumDistributionGrid<Tag_, lbm_applications::LABSWE>::value(g, e, info, data)));
#ifdef HONEI_CUDA
                        cuda_thread_synchronize();
#endif
            }
            evaluate();
        }
};

EquilibriumDistributionGridBench<tags::CPU, float, D2Q9::DIR_0> eq_dist_grid_bench_float("EquilibriumDistributionGridBenchmark - size: 1000, float", 1000, 10);
EquilibriumDistributionGridBench<tags::CPU, double, D2Q9::DIR_0> eq_dist_grid_bench_double("EquilibriumDistributionGrid Benchmark - size: 1000, double", 1000, 10);
#ifdef HONEI_SSE
EquilibriumDistributionGridBench<tags::CPU::SSE, float, D2Q9::DIR_0> sse_eq_dist_grid_bench_float("SSE EquilibriumDistributionGridBenchmark - size: 1000, float", 1000, 10);
EquilibriumDistributionGridBench<tags::CPU::SSE, double, D2Q9::DIR_0> sse_eq_dist_grid_bench_double("SSE EquilibriumDistributionGridBenchmark - size: 1000, double", 1000, 10);
#endif
#ifdef HONEI_CUDA
EquilibriumDistributionGridBench<tags::GPU::CUDA, float, D2Q9::DIR_0> cuda_eq_dist_grid_bench_float("CUDA EquilibriumDistributionGridBenchmark - size: 1000, float", 1000, 10);
#endif