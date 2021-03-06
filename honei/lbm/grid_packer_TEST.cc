/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2008 Markus Geveler <apryde@gmx.de>
 * Copyright (c) 2008 Dirk Ribbrock <dirk.ribbrock@uni-dortmund.de>
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
#include <honei/lbm/grid_packer.hh>
#include <honei/util/unittest.hh>
#include <iostream>

using namespace honei;
using namespace tests;
using namespace std;
using namespace lbm;
using namespace lbm_lattice_types;

template <typename Tag_, typename DataType_>
class GridPackerTest :
    public QuickTaggedTest<Tag_>
{
    public:
        GridPackerTest(const std::string & type) :
            QuickTaggedTest<Tag_>("grid_packer_quick_test<" + type + ">")
        {
        }

        virtual void run() const
        {
            DenseMatrix<DataType_> dummy(10, 12, DataType_(2));
            dummy(2, 5) = DataType_(5);
            DenseMatrix<bool> obst(10, 12, false);
            /*
             +++++++++++*
             +++++++++++*
             ++++*+++++++
             +++X++++++++
             */
            //obst(0,5) = true;
            obst(0, 11) = true;
            obst(1, 11) = true;
            obst(2, 4) = true;

            /*obst(0,4) = true;
            obst(1,5) = true;
            obst(1,6) = true;
            obst(0,6) = true;
            for (MutableMatrix<bool>::ElementIterator i(obst.begin_elements()) ; i.index() < 62 ; ++i)
            {
                *i = true;
            }*/
            std::cout<<obst;
            PackedGridInfo<D2Q9> info;
            PackedGridData<D2Q9, DataType_> data;

            Grid<D2Q9, DataType_> grid;

            grid.h = new DenseMatrix<DataType_>(dummy.copy());
            grid.u = new DenseMatrix<DataType_>(dummy.copy());
            grid.v = new DenseMatrix<DataType_>(dummy.copy());
            grid.b = new DenseMatrix<DataType_>(dummy.copy());
            grid.obstacles = new DenseMatrix<bool>(obst);

            GridPacker<D2Q9, lbm_boundary_types::NOSLIP, DataType_>::pack(grid, info, data);
            std::cout<<*grid.h_index;
            std::cout<<"h_index (3, 5): "<<GridPacker<D2Q9, lbm_boundary_types::NOSLIP, DataType_>::h_index(grid, 3, 5)<<std::endl;

            /*for (unsigned long i(0) ; i < info.limits->size() ; ++i)
            {
                std::cout << (*info.limits)[i] << " " << (*info.types)[i] << std::endl;
            }*/
            std::cout<<*info.limits;
/*            std::cout<<"1 "<<*info.dir_index_1;
            std::cout<<"2: "<<*info.dir_index_2;
            std::cout<<"3: "<<*info.dir_index_3;
            std::cout<<"4: "<<*info.dir_index_4;
            std::cout<<"5: "<<*info.dir_index_5;
            std::cout<<"6: "<<*info.dir_index_6;*/
            std::cout<<"dir index 7: "<<*info.dir_index_7;
            std::cout<<"dir 7: "<<*info.dir_7;
            //std::cout<<"8: "<<*info.dir_index_8;

            GridPacker<D2Q9, lbm_boundary_types::NOSLIP, DataType_>::unpack(grid, info, data);
            //std::cout << *grid.h << std::endl;

//            std::cout << GridPacker<D2Q9, lbm_boundary_types::NOSLIP, DataType_>::extract_ftemp2(grid, info, data) << std::endl;

            GridPacker<D2Q9, lbm_boundary_types::NOSLIP, DataType_>::cuda_pack(info, data);
            std::cout<<"cuda dir 7: "<<*info.cuda_dir_7;
            TEST_CHECK(true);
            grid.destroy();
            info.destroy();
            data.destroy();
        }
};
GridPackerTest<tags::CPU, float> gptest_float("float");
