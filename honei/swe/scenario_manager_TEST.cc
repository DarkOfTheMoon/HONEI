/* vim: set number sw=4 sts=4 et nofoldenable : */

/*
 * Copyright (c) 2008 Markus Geveler <apryde@gmx.de>
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

//#define SOLVER_BENCHMARK 1
#include <honei/swe/relax_solver.hh>
#include <honei/la/dense_vector.hh>
#include <honei/la/dense_matrix.hh>
#include <honei/util/unittest.hh>
#include <honei/util/stringify.hh>
#include <string>
#include <honei/swe/scenario.hh>
#include <honei/swe/scenario_manager.hh>
#include <sys/time.h>

using namespace honei;
using namespace tests;
using namespace std;
using namespace swe_solvers;

template <typename DataType_>
class ScenarioManagerTest :
    public BaseTest
{
    public:
        ScenarioManagerTest(const std::string & type) :
            BaseTest("scenario_manager_test<" + type + ">")
        {
        }

        virtual void run() const
        {
            unsigned long dwidth = 40;
            unsigned long dheight = 40;
            unsigned long timesteps = 1;

            DenseMatrix<DataType_> height(dheight, dwidth, DataType_(5));
            //SCENARIO setup
            for(ulint i = 0; i< height.rows(); ++i)
            {
                for(ulint j=height.columns()-10; j<height.columns(); ++j)
                {
                    height[i][j] = DataType_(10);
                }
            }

            DenseMatrix<DataType_> bottom(dheight, dwidth, DataType_(1));
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
            ScenarioManager<DataType_, swe_solvers::RELAX, boundaries::REFLECT> scen_man;
            Scenario<DataType_, swe_solvers::RELAX, boundaries::REFLECT> scenario(dwidth, dheight);

            scen_man.allocate_scenario(&scenario);
            scen_man.allocate_scalarfields(&height, &bottom, &u1, &u2);
            scen_man.allocate_relax_vectors(&u, &v, &w, &c, &d);
            scen_man.allocate_bottom_slopes(&bx, &by);
            scen_man.set_environmental_variables(deltax, deltay, deltat, manning, eps);

            if(scen_man.validate())
            {
                RelaxSolver<tags::CPU, DataType_, DataType_, DataType_, DataType_, DataType_, source_types::SIMPLE, boundaries::REFLECT, precision_modes::FIXED> relax_solver
                    (scenario);
                relax_solver.do_preprocessing();
                std::cout << "Height -field after preprocessing:\n";
                string outHeight = stringify(height);
                std::cout <<  outHeight;

                timeval start, end;
                for (ulint i = 1; i <= timesteps; ++i)
                {
                    gettimeofday(&start, 0);
                    relax_solver.solve();
                    gettimeofday(&end, 0);
                    std::cout << "Timestep "<< i <<" / " << timesteps << " finished." <<std::endl;
                    std::cout << "Solvetime: "<< end.tv_sec - start.tv_sec << " " << end.tv_usec - start.tv_usec<<std::endl;
                }
                std::cout << "Height -field after solve():\n";
                std::cout << stringify(height);

                bool pass = true;
                for(unsigned long i(0); i < height.rows(); ++i)
                {
                    for(unsigned long j(0); j < height.columns(); ++j)
                    {
                        if(height[i][j] < DataType_(4.8) || height[i][j] > DataType_(10.))
                            pass = false;
                    }
                }
                TEST_CHECK(pass);
            }
        }
};
ScenarioManagerTest<float> scenario_manager_test_float("float");

template <typename DT1_, typename DT2_>
class ScenarioManagerConversionTest :
    public BaseTest
{
    public:
        ScenarioManagerConversionTest(const std::string & type) :
            BaseTest("scenario_manager_conversion_test<" + type + ">")
        {
        }

        virtual void run() const
        {
            unsigned long dwidth = 40;
            unsigned long dheight = 40;
            unsigned long timesteps = 1;

            DenseMatrix<DT1_> height(dheight, dwidth, DT1_(5));
            //SCENARIO setup
            for(ulint i = 0; i< height.rows(); ++i)
            {
                for(ulint j=height.columns()-10; j<height.columns(); ++j)
                {
                    height[i][j] = DT1_(10);
                }
            }

            DenseMatrix<DT1_> bottom(dheight, dwidth, DT1_(1));
            std::cout<<bottom<<std::endl;

            DenseMatrix<DT1_> u1(dheight, dwidth, DT1_(0));
            DenseMatrix<DT1_> u2(dheight, dwidth, DT1_(0));
            unsigned long entries = 3*((dwidth*dheight)+4*(dwidth+dheight+4));
            DenseVector<DT1_> u(entries, DT1_(1));
            DenseVector<DT1_> v(entries, DT1_(1));
            DenseVector<DT1_> w(entries, DT1_(1)); 
            DenseVector<DT1_> bx (entries/3, DT1_(0));
            DenseVector<DT1_> by (entries/3, DT1_(0));
            DenseVector<DT1_> c (3,DT1_(5));
            DenseVector<DT1_> d (3,DT1_(5));
            c[0] = 10;
            c[1] = 6;
            c[2] = 11;
            d[0] = 10;
            d[1] = 5;
            d[2] = 11;

            DT1_ deltax = 5;
            DT1_ deltay = 5;
            DT1_ deltat = 5./24.;

            double eps = 10e-6;
            DT1_ manning = DT1_(0);
            ScenarioManager<DT1_, swe_solvers::RELAX, boundaries::REFLECT> scen_man;
            Scenario<DT1_, swe_solvers::RELAX, boundaries::REFLECT> scenario(dwidth, dheight);

            scen_man.allocate_scenario(&scenario);
            scen_man.allocate_scalarfields(&height, &bottom, &u1, &u2);
            scen_man.allocate_relax_vectors(&u, &v, &w, &c, &d);
            scen_man.allocate_bottom_slopes(&bx, &by);
            scen_man.set_environmental_variables(deltax, deltay, deltat, manning, eps);
//-------------------------------------------------------------------------------------------------------------------
            DenseMatrix<DT2_> height_2(dheight, dwidth, DT2_(5));
            //SCENARIO setup
            for(ulint i = 0; i< height_2.rows(); ++i)
            {
                for(ulint j=height_2.columns()-10; j<height_2.columns(); ++j)
                {
                    height_2[i][j] = DT2_(10);
                }
            }

            DenseMatrix<DT2_> bottom_2(dheight, dwidth, DT2_(1));
            std::cout<<bottom<<std::endl;

            DenseMatrix<DT2_> u1_2(dheight, dwidth, DT2_(0));
            DenseMatrix<DT2_> u2_2(dheight, dwidth, DT2_(0));
            DenseVector<DT2_> u_2(entries, DT2_(1));
            DenseVector<DT2_> v_2(entries, DT2_(1));
            DenseVector<DT2_> w_2(entries, DT2_(1)); 
            DenseVector<DT2_> bx_2 (entries/3, DT2_(0));
            DenseVector<DT2_> by_2 (entries/3, DT2_(0));
            DenseVector<DT2_> c_2 (3,DT2_(5));
            DenseVector<DT2_> d_2 (3,DT2_(5));
            c_2[0] = 10;
            c_2[1] = 6;
            c_2[2] = 11;
            d_2[0] = 10;
            d_2[1] = 5;
            d_2[2] = 11;

            DT2_ deltax_2 = 5;
            DT2_ deltay_2 = 5;
            DT2_ deltat_2 = 5./24.;

            DT2_ manning_2 = DT2_(0);
            ScenarioManager<DT2_, swe_solvers::RELAX, boundaries::REFLECT> scen_man_2;
            Scenario<DT2_, swe_solvers::RELAX, boundaries::REFLECT> scenario_2(dwidth, dheight);

            scen_man_2.allocate_scenario(&scenario_2);
            scen_man_2.allocate_scalarfields(&height_2, &bottom_2, &u1_2, &u2_2);
            scen_man_2.allocate_relax_vectors(&u_2, &v_2, &w_2, &c_2, &d_2);
            scen_man_2.allocate_bottom_slopes(&bx_2, &by_2);
            scen_man_2.set_environmental_variables(deltax_2, deltay_2, deltat_2, manning_2, eps);

            ScenarioManager<DT2_, swe_solvers::RELAX, boundaries::REFLECT>::convert_scenario(scenario_2, scenario);

            if(scen_man_2.validate())
            {
                RelaxSolver<tags::CPU, DT2_, DT2_, DT2_, DT2_, DT2_, source_types::SIMPLE, boundaries::REFLECT, precision_modes::FIXED> relax_solver
                    (scenario_2);
                relax_solver.do_preprocessing();
                std::cout << "Height -field after preprocessing:\n";
                string outHeight = stringify(height_2);
                std::cout <<  outHeight;

                timeval start, end;
                for (ulint i = 1; i <= timesteps; ++i)
                {
                    gettimeofday(&start, 0);
                    relax_solver.solve();
                    gettimeofday(&end, 0);
                    std::cout << "Timestep "<< i <<" / " << timesteps << " finished." <<std::endl;
                    std::cout << "Solvetime: "<< end.tv_sec - start.tv_sec << " " << end.tv_usec - start.tv_usec<<std::endl;
                }
                std::cout << "Height -field after solve():\n";
                std::cout << stringify(height_2);

                bool pass = true;
                for(unsigned long i(0); i < height_2.rows(); ++i)
                {
                    for(unsigned long j(0); j < height_2.columns(); ++j)
                    {
                        if(height_2[i][j] < DT2_(4.8) || height_2[i][j] > DT2_(10.))
                            pass = false;
                    }
                }
                TEST_CHECK(pass);
            }
        }
};
ScenarioManagerConversionTest<double, float> scenario_manager_conv_test_float("double to float");

