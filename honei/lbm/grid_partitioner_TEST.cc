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
#include <honei/lbm/grid_partitioner.hh>
#include <honei/lbm/grid_packer.hh>
#include <honei/util/unittest.hh>
#include <iostream>

using namespace honei;
using namespace tests;
using namespace std;
using namespace lbm;
using namespace lbm_lattice_types;

template <typename Tag_, typename DataType_>
class GridPartitionerTest :
    public QuickTaggedTest<Tag_>
{
    public:
        GridPartitionerTest(const std::string & type) :
            QuickTaggedTest<Tag_>("grid_partitioner_quick_test<" + type + ">")
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
            obst(0,5) = true;
            obst(0, 11) = true;
            obst(1, 11) = true;
            obst(2, 4) = true;

            obst(0,4) = true;
            obst(1,5) = true;
            obst(1,6) = true;
            obst(0,6) = true;
            /*for (MutableMatrix<bool>::ElementIterator i(obst.begin_elements()) ; i.index() < 62 ; ++i)
            {
                *i = true;
            }*/
            PackedGridInfo<D2Q9> info;
            PackedGridData<D2Q9, DataType_> data;

            Grid<D2Q9, DataType_> grid;

            grid.h = new DenseMatrix<DataType_>(dummy.copy());
            grid.u = new DenseMatrix<DataType_>(dummy.copy());
            grid.v = new DenseMatrix<DataType_>(dummy.copy());
            grid.b = new DenseMatrix<DataType_>(dummy.copy());
            grid.obstacles = new DenseMatrix<bool>(obst);
            std::cout<<"Obstacles: "<<obst;

            std::vector<PackedGridInfo<D2Q9> > info_list;
            std::vector<PackedGridData<D2Q9, DataType_> > data_list;
            std::vector<PackedGridFringe<D2Q9> > fringe_list;

            GridPacker<D2Q9, lbm_boundary_types::NOSLIP, DataType_>::pack(grid, info, data);
            std::cout<<"global dir index 1: "<<*info.dir_index_1;
            std::cout<<"global dir 1: "<<*info.dir_1;
            std::cout<<"global limits: "<<*info.limits;
            std::cout<<"global h size: "<<data.h->size()<<std::endl;
            GridPartitioner<D2Q9, DataType_>::decompose(5, info, data, info_list, data_list, fringe_list);
            GridPartitioner<D2Q9, DataType_>::synch(info, data, info_list, data_list, fringe_list);
            GridPartitioner<D2Q9, DataType_>::compose(info, data, info_list, data_list);

            for (unsigned long i(0) ; i < info_list.size() ; ++i)
            {
                std::cout<<info_list[i].offset<<std::endl;
                /*std::cout<<"fringe dir 3 index "<<*fringe_list[i].dir_index_3;
                std::cout<<"fringe dir 3 targets "<<*fringe_list[i].dir_targets_3;
                std::cout<<"fringe external dir 3 index "<<*fringe_list[i].external_dir_index_3;*/
                std::cout<<"fringe h index "<<*fringe_list[i].h_index;
                std::cout<<"fringe h targets "<<*fringe_list[i].h_targets;
                std::cout<<"fringe external h index "<<*fringe_list[i].external_h_index;
                std::cout<<"fringe external h targets "<<*fringe_list[i].external_h_targets;
                std::cout<<"limits "<<*info_list[i].limits;
                std::cout<<"h size: "<<data_list[i].h->size()<<std::endl;
                std::cout<<"fringe dir 1 targets "<<*fringe_list[i].dir_targets_1;
                std::cout<<"fringe dir 1 external targets "<<*fringe_list[i].external_dir_targets_1;
            }

            TEST_CHECK(true);
            GridPartitioner<D2Q9, DataType_>::destroy(info_list, data_list, fringe_list);
            info.destroy();
            data.destroy();
            grid.destroy();
        }
};
GridPartitionerTest<tags::CPU, float> gptest_float("float");

template <typename Tag_, typename DataType_>
class DirPartitionerTest :
    public QuickTaggedTest<Tag_>
{
    public:
        DirPartitionerTest(const std::string & type) :
            QuickTaggedTest<Tag_>("dir_partitioner_quick_test<" + type + ">")
        {
        }

        virtual void run() const
        {

            std::vector<unsigned long> dir;
            std::vector<unsigned long> dir_index;
            std::vector<unsigned long> barrier;
            dir_index.push_back(5);
            dir_index.push_back(7);
            dir_index.push_back(7);
            dir_index.push_back(10);
            dir_index.push_back(12);
            dir_index.push_back(15);
            dir_index.push_back(20);
            dir_index.push_back(30);
            dir.push_back(0);
            dir.push_back(10);
            dir.push_back(20);
            dir.push_back(30);
            //std::cout<<"start: "<<start<<" end: "<<end<<std::endl;
            std::cout<<"dir index vorher: "<<std::endl;
            for (unsigned long i(0) ; i < dir_index.size() - 1 ; i+=2)
            {
                std::cout<<dir_index[i]<<", "<<dir_index[i+1]<<std::endl;
            }
            std::cout<<"dir vorher: "<<std::endl;
            for (unsigned long i(0) ; i < dir.size() ; ++i)
            {
                std::cout<<dir[i]<<", ";
            }
            std::cout<<endl;
            GridPartitioner<D2Q9, DataType_>::partition_directions(dir_index, dir, barrier, 0, 2);
            GridPartitioner<D2Q9, DataType_>::partition_directions(dir_index, dir, barrier, 2, 8);
            GridPartitioner<D2Q9, DataType_>::partition_directions(dir_index, dir, barrier, 8, 14);
            GridPartitioner<D2Q9, DataType_>::partition_directions(dir_index, dir, barrier, 14, 19);
            std::cout<<"dir index nachher: "<<std::endl;
            for (unsigned long i(0) ; i < dir_index.size() - 1 ; i+=2)
            {
                std::cout<<dir_index[i]<<", "<<dir_index[i+1]<<std::endl;
            }
            std::cout<<"dir nachher: "<<std::endl;
            for (unsigned long i(0) ; i < dir.size() ; ++i)
            {
                std::cout<<dir[i]<<", ";
            }
            std::cout<<endl;
            std::cout<<"barrier nachher: "<<std::endl;
            for (unsigned long i(0) ; i < barrier.size() ; ++i)
            {
                std::cout<<barrier[i]<<", ";
            }
            std::cout<<endl;

            TEST_CHECK(true);
        }
};
DirPartitionerTest<tags::CPU, float> dptest_float("float");
