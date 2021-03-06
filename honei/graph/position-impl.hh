/* vim: set sw=4 sts=4 et nofoldenable : */

/* 
 * Copyright (c) 2007 Mathias Kadolsky <mathkad@gmx.de> 
 *
 * This file is part of the Graph C library. LibGraph is free software;
 * you can redistribute it and/or modify it under the terms of the GNU General
 * Public License version 2, as published by the Free Software Foundation.
 *
 * LibGraph is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA  02111-1307  USA
 */

#pragma once
#ifndef LIBGRAPH_GUARD_POSITIONS_IMPL_HH
#define LIBGRAPH_GUARD_POSITIONS_IMPL_HH 1

#include <honei/graph/position.hh>
#include <honei/la/sum.hh>

#include <cmath>

#include <iostream>
/**
 * \file
 *
 * Interface declarations for position calculation classes.
 *
 * \ingroup grplibgraph
 **/
 namespace honei
{
    namespace methods
    {
        /**
         * Implementation tag for weighted Fruchterman-Reingold method.
         */
        struct WeightedFruchtermanReingold
        {
        };

        /**
         * Implementation tag for weighted Kamada-Kawai method.
         */
        struct WeightedKamadaKawai
        {
        };

        /**
         * Implementation class template for the Components of varying methods.
         */
        template <typename Tag_ , typename DataType_, typename GraphTag_>
        struct ImplementationComponents
        {
            static void initializeForces(const DenseMatrix<DataType_> & spring_force_parameters, const DenseMatrix<DataType_> & coordinates,
            DenseMatrix<DataType_> & spring_forces, DenseMatrix<DataType_> & node_forces)
            {
                typename DenseMatrix<DataType_>::ConstElementIterator x(spring_force_parameters.begin_elements());
                long int _size(coordinates.rows() * coordinates.columns());
                long int _row_number(0);
                for (unsigned long i(0); i < coordinates.rows(); i++)
                {
                    DenseMatrix<DataType_> position_differences(coordinates.rows(), coordinates.columns(), DataType_(0));
                    TypeTraits<DataType_>::copy(coordinates.elements(), position_differences.elements(), coordinates.rows() * coordinates.columns());
                    for (unsigned long j(0); j < coordinates.rows(); j++)
                    {
                        typename DenseMatrix<DataType_>::Row pos_row(position_differences[j]), spring_row(spring_forces[i]);
                        Difference<Tag_>::value(pos_row, coordinates[i]);
                        Scale<Tag_>::value(pos_row, * x);
                        Sum<Tag_>::value(spring_row, pos_row);
                        ++x;
                    }
                    TypeTraits<DataType_>::copy(position_differences.elements(), node_forces[_row_number].elements(), _size);
                    _row_number += coordinates.rows();
                }
            }

            static void calculateForces(DenseMatrix<DataType_> & spring_forces, int & max_node, const DenseMatrix<DataType_> & coordinates,
            DenseMatrix<DataType_> & node_forces, const DenseVectorRange<DataType_> & graph_distance, DataType_ & result)
            {
                int _previous_max_node(max_node);
                long int _size(coordinates.rows() * coordinates.columns());
                TypeTraits<DataType_>::fill(spring_forces[_previous_max_node].elements(), spring_forces.columns(), DataType_(0));
                TypeTraits<DataType_>::copy(coordinates.elements(), node_forces.elements() + _previous_max_node * _size, _size);
                DenseVector<DataType_> _force_difference_of_max_node(coordinates.columns(), DataType_(0));
                long int pos_1(_previous_max_node * coordinates.rows());
                long int pos_2(_previous_max_node);
                for (unsigned long i(0) ; i != coordinates.rows() ; ++i)
                {
                    // Calculate force parameter vector of node with maximal force(_auxiliary)
                    typename DenseMatrix<DataType_>::Row nf_row(node_forces[pos_1]), spring_row(spring_forces[i]);
                    Difference<Tag_>::value(nf_row, coordinates[_previous_max_node]);
                    DataType_ _auxiliary(Norm<vnt_l_two, false, Tag_>::value(nf_row));
                    DataType_ _auxiliary2(_auxiliary * DataType_(2));
                    _auxiliary = _auxiliary + graph_distance[i];
                    (_auxiliary != 0) ? _auxiliary = _auxiliary2 / _auxiliary - DataType_(1): _auxiliary = DataType_(0);
                    // Subtract the node forces of _previous_max_node from the spring_forces
                    Difference<Tag_>::value(spring_row, node_forces[pos_2]);
                    // Calcultae the new force differences(node_forces)
                    Scale<Tag_>::value(nf_row, DataType_(-1) * _auxiliary);
                    Difference<Tag_>::value(_force_difference_of_max_node, node_forces[pos_1]);
                    // Calculate the _spring_forces = _spring_forces + node_forces
                    Sum<Tag_>::value(spring_row, node_forces[pos_1]);
                    // Calculate the resulting forces, the maximal force and the node with maximal force
                    DataType_ resulting_force(Norm<vnt_l_two, true, Tag_>::value(spring_forces[i]));
                    resulting_force > result ? result = resulting_force, max_node = i : 0;
                    // Write new node_forces back
                    TypeTraits<DataType_>::copy(node_forces[pos_1].elements(), node_forces[pos_2].elements(), coordinates.columns());
                    Scale<Tag_>::value(nf_row, DataType_(-1));
                    pos_2 += coordinates.rows();
                    pos_1 ++;
                }
                // Calculate the _spring_forces at _previous_max_node = _spring_forces + _force_difference_of_max_node
                typename DenseMatrix<DataType_>::Row spring_row(spring_forces[_previous_max_node]);
                Sum<Tag_>::value(spring_row, _force_difference_of_max_node);
                DataType_ resulting_force(Norm<vnt_l_two, true, Tag_>::value(spring_row));
                // Calculate the resulting forces, the maximal force and the node with maximal force for _previous_max_node
                resulting_force > result ? result = resulting_force, max_node = _previous_max_node: 0;
            }
        };
    }
}

#endif
