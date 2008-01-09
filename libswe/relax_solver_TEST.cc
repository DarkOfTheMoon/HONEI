/* vim: set number sw=4 sts=4 et nofoldenable : */

/*
 * Copyright (c) 2007 Markus Geveler <apryde@gmx.de>
 *
 * This file is part of the SWE C++ library. LiSWE is free software;
 * you can redistribute it and/or modify it under the terms of the GNU General
 * Public License version 2, as published by the Free Software Foundation.
 *
 * LibSWE is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA  02111-1307  USA
 */
#define SOLVER_BENCHMARK 1
#include <libswe/relax_solver.hh>
#include <libla/dense_vector.hh>
#include <libla/dense_matrix.hh>
#include <unittest/unittest.hh>
#include <libutil/stringify.hh>
#include <string>
#include <libswe/scenario.hh>
#include <sys/time.h>

using namespace honei;
using namespace tests;
using namespace std;
using namespace swe_solvers;

template <typename Tag_, typename DataType_>
class RelaxSolverTest :
    public BaseTest
{
    public:
        RelaxSolverTest(const std::string & type) :
            BaseTest("relax_solver_quick_test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            ulint dwidth = 40;
            ulint dheight = 40;
            ulint timesteps = 50;

            DenseMatrix<DataType_> height(dheight, dwidth, DataType_(5));
            //SCENARIO setup
            for(ulint i = 0; i< height.rows(); ++i)
            {
                for(ulint j=height.columns()-10; j<height.columns(); ++j)
                {
                    height[i][j] = DataType_(10);
                }
                 //(height)[0][i] = DataType_(10);
            }

            //END SCENARIO setup
            DenseMatrix<DataType_> bottom(dheight, dwidth, DataType_(1));
            /*for(ulint i = 0; i< bottom.rows(); ++i)
            {
                for(ulint j=0; j<bottom.columns()-10; ++j)
                {
                    bottom[i][j] = DataType_(1);
                    if(j>4 && j< bottom.columns()-9)
                    {
                        if(i < 6 || i > 11)
                            bottom[i][j] = DataType_(3);
                        else
                            bottom[i][j] = DataType_(1);
                    }
                }
            }*/
            std::cout<<bottom<<std::endl;

            DenseMatrix<DataType_> u1(dheight, dwidth, DataType_(0));
            DenseMatrix<DataType_> u2(dheight, dwidth, DataType_(0));
            unsigned long entries = 3*((dwidth*dheight)+4*(dwidth+dheight+4));
            DenseVector<DataType_> u(entries, DataType_(1));
            DenseVector<DataType_> v(entries, DataType_(1));
            DenseVector<DataType_> w(entries, DataType_(1)); 
            DenseVector<DataType_> bx (entries/3, DataType_(0));
            DenseVector<DataType_> by (entries/3, DataType_(0));
            DenseVector<DataType_> c (3,DataType_(5));
            DenseVector<DataType_> d (3,DataType_(5));
            //SCENARIO setup:
            c[0] = 10;
            c[1] = 6;
            c[2] = 11;
            d[0] = 10;
            d[1] = 5;
            d[2] = 11;

            DataType_ deltax = 5;
            DataType_ deltay = 5;
            DataType_ deltat = 5./24.;

            double eps = 10e-6;
            DataType_ manning = DataType_(0);
            Scenario<DataType_, swe_solvers::RELAX, boundaries::REFLECT> scenario(dwidth, dheight);
            scenario.height = &height;
            scenario.bottom = &bottom;
            scenario.x_veloc = &u1;
            scenario.y_veloc = &u2;
            scenario.u = &u;
            scenario.v = &v;
            scenario.w = &w;
            scenario.bottom_slopes_x = &bx;
            scenario.bottom_slopes_y = &by;
            scenario.c = &c;
            scenario.d = &d;
            scenario.delta_x = deltax;
            scenario.delta_y = deltay;
            scenario.delta_t = deltat;
            scenario.eps = eps;
            scenario.manning_n = manning;

            RelaxSolver<Tag_, DataType_, DataType_, DataType_, DataType_, DataType_, source_types::SIMPLE, boundaries::REFLECT> relax_solver
                (scenario);
            relax_solver.do_preprocessing();
            cout << "Height -field after preprocessing:\n";
            string outHeight = stringify(height);
            cout <<  outHeight;

            timeval start, end;
            for (ulint i = 1; i <= timesteps; ++i)
            {
                gettimeofday(&start, 0);
                relax_solver.solve();
                gettimeofday(&end, 0);
                cout << "Timestep "<< i <<" / " << timesteps << " finished." <<endl;
                cout << "Solvetime: "<< end.tv_sec - start.tv_sec << " " << end.tv_usec - start.tv_usec<<endl;
            }
            cout << "Height -field after solve():\n";
            cout << stringify(height);

            bool pass = true;
            for(unsigned long i(0); i < height.rows(); ++i)
            {
                for(unsigned long j(0); j < height.columns(); ++j)
                {
                    if(height[i][j] < DataType_(5.) || height[i][j] > DataType_(10.))
                        pass = false;
                }
            }
            TEST_CHECK(pass);
        }
};
RelaxSolverTest<tags::CPU, float> relax_solver_test_float("float");
RelaxSolverTest<tags::CPU, double> relax_solver_test_double("double");
RelaxSolverTest<tags::CPU::MultiCore, float> mc_relax_solver_test_float("mc float");
RelaxSolverTest<tags::CPU::MultiCore, double> mc_relax_solver_test_double("mc double");
#ifdef HONEI_SSE
RelaxSolverTest<tags::CPU::SSE, float> sse_relax_solver_test_float("sse float");
RelaxSolverTest<tags::CPU::SSE, double> sse_relax_solver_test_double("sse double");
RelaxSolverTest<tags::CPU::MultiCore::SSE, float> mc_sse_relax_solver_test_float("mc sse float");
RelaxSolverTest<tags::CPU::MultiCore::SSE, double> ms_sse_relax_solver_test_double("mc sse double");
#endif
#ifdef HONEI_CELL
RelaxSolverTest<tags::Cell, float> cell_relax_solver_test_float("cell float");
#endif
