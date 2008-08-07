/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2008 Markus Geveler <apryde@gmx.de>
 *
 * This file is part of the LBM C++ library. LBM is free software;
 * you can redistribute it and/or modify it under the terms of the GNU General
 * Public License version 2, as published by the Free Software Foundation.
 *
 * LBM is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA  02111-1307  USA
 */


#ifndef LBM_GUARD_SOLVER_LABSWE_GRID_HH
#define LBM_GUARD_SOLVER_LABSWE_GRID_HH 1

/**
 * \file
 * Implementation of a SWE solver using LBM and PackedGrid.
 *
 * \ingroup grpliblbm
 **/

//#define SOLVER_POSTPROCESSING 1

#include <honei/lbm/tags.hh>
#include <honei/la/dense_vector.hh>
#include <honei/la/dense_matrix.hh>
#include <honei/la/sum.hh>
#include <honei/la/scale.hh>
#include <honei/la/element_product.hh>
#include <honei/la/element_inverse.hh>
#include <honei/lbm/collide_stream_grid.hh>
#include <honei/lbm/equilibrium_distribution_grid.hh>
#include <honei/lbm/source_grid.hh>
#include <cmath>
#include <grid.hh>

using namespace honei::lbm;

namespace honei
{

    template<typename Tag_,
        typename ResPrec_,
        typename SourceType_,
        typename SourceSheme_,
        typename GridType_,
        typename LatticeType_,
        typename BoundaryType_>
            class SolverLABSWE
            {
            };

    template<typename Tag_, typename ResPrec_>
        class SolverLABSWE<Tag_, ResPrec_, lbm_source_types::SIMPLE, lbm_source_schemes::BASIC, lbm_grid_types::RECTANGULAR, lbm_lattice_types::D2Q9, lbm_boundary_types::NOSLIP>
        {
            private:
                /** Global variables.
                 *
                 **/

                PackedGridData<D2Q9, ResPrec_> * data;
                PackedGridInfo<D2Q9> * info;

                ResPrec_ _relaxation_time, _delta_x, _delta_y, _delta_t;
                DenseVector<ResPrec_>* _height;
                DenseVector<ResPrec_>* _bottom;
                DenseVector<ResPrec_>* _d_bottom_x;
                DenseVector<ResPrec_>* _d_bottom_y;
                DenseVector<ResPrec_>* _u;
                DenseVector<ResPrec_>* _v;

                DenseVector<ResPrec_>* _distribution_0;
                DenseVector<ResPrec_>* _distribution_1;
                DenseVector<ResPrec_>* _distribution_2;
                DenseVector<ResPrec_>* _distribution_3;
                DenseVector<ResPrec_>* _distribution_4;
                DenseVector<ResPrec_>* _distribution_5;
                DenseVector<ResPrec_>* _distribution_6;
                DenseVector<ResPrec_>* _distribution_7;
                DenseVector<ResPrec_>* _distribution_8;

                DenseVector<ResPrec_>* _temp_distribution_0;
                DenseVector<ResPrec_>* _temp_distribution_1;
                DenseVector<ResPrec_>* _temp_distribution_2;
                DenseVector<ResPrec_>* _temp_distribution_3;
                DenseVector<ResPrec_>* _temp_distribution_4;
                DenseVector<ResPrec_>* _temp_distribution_5;
                DenseVector<ResPrec_>* _temp_distribution_6;
                DenseVector<ResPrec_>* _temp_distribution_7;
                DenseVector<ResPrec_>* _temp_distribution_8;

                DenseVector<ResPrec_>* _eq_distribution_0;
                DenseVector<ResPrec_>* _eq_distribution_1;
                DenseVector<ResPrec_>* _eq_distribution_2;
                DenseVector<ResPrec_>* _eq_distribution_3;
                DenseVector<ResPrec_>* _eq_distribution_4;
                DenseVector<ResPrec_>* _eq_distribution_5;
                DenseVector<ResPrec_>* _eq_distribution_6;
                DenseVector<ResPrec_>* _eq_distribution_7;
                DenseVector<ResPrec_>* _eq_distribution_8;

                unsigned long _grid_width, _grid_height, _time;

                DenseVector<ResPrec_>* _source_x;
                DenseVector<ResPrec_>* _source_y;

                /** Global constants.
                 *
                 **/
                ResPrec_ _n_alpha, _e, _gravity, _pi;
                DenseVector<ResPrec_>* _distribution_vector_x;
                DenseVector<ResPrec_>* _distribution_vector_y;

                /** Capsule for the extration of SWE physical quantities.
                 *
                 **/
                void _extract()
                {
                    CONTEXT("When extracting physical quantities in LABSWE:");

                    ///Set temple dis to dis:
                    *_distribution_0 = *_temp_distribution_0;
                    *_distribution_1 = *_temp_distribution_1;
                    *_distribution_2 = *_temp_distribution_2;
                    *_distribution_3 = *_temp_distribution_3;
                    *_distribution_4 = *_temp_distribution_4;
                    *_distribution_5 = *_temp_distribution_5;
                    *_distribution_6 = *_temp_distribution_6;
                    *_distribution_7 = *_temp_distribution_7;
                    *_distribution_8 = *_temp_distribution_8;

                    DenseVector<ResPrec_> accu(_distribution_0->copy());

                    Sum<Tag_>::value(accu, *_distribution_1);
                    Sum<Tag_>::value(accu, *_distribution_2);
                    Sum<Tag_>::value(accu, *_distribution_3);
                    Sum<Tag_>::value(accu, *_distribution_4);
                    Sum<Tag_>::value(accu, *_distribution_5);
                    Sum<Tag_>::value(accu, *_distribution_6);
                    Sum<Tag_>::value(accu, *_distribution_7);
                    Sum<Tag_>::value(accu, *_distribution_8);

                    *_height = accu;

                    DenseVector<ResPrec_> d0c(_distribution_0->copy());
                    DenseVector<ResPrec_> d1c(_distribution_1->copy());
                    DenseVector<ResPrec_> d2c(_distribution_2->copy());
                    DenseVector<ResPrec_> d3c(_distribution_3->copy());
                    DenseVector<ResPrec_> d4c(_distribution_4->copy());
                    DenseVector<ResPrec_> d5c(_distribution_5->copy());
                    DenseVector<ResPrec_> d6c(_distribution_6->copy());
                    DenseVector<ResPrec_> d7c(_distribution_7->copy());
                    DenseVector<ResPrec_> d8c(_distribution_8->copy());
                    DenseVector<ResPrec_> d0c_2(_distribution_0->copy());
                    DenseVector<ResPrec_> d1c_2(_distribution_1->copy());
                    DenseVector<ResPrec_> d2c_2(_distribution_2->copy());
                    DenseVector<ResPrec_> d3c_2(_distribution_3->copy());
                    DenseVector<ResPrec_> d4c_2(_distribution_4->copy());
                    DenseVector<ResPrec_> d5c_2(_distribution_5->copy());
                    DenseVector<ResPrec_> d6c_2(_distribution_6->copy());
                    DenseVector<ResPrec_> d7c_2(_distribution_7->copy());
                    DenseVector<ResPrec_> d8c_2(_distribution_8->copy());

                    Scale<Tag_>::value( d0c, (*_distribution_vector_x)[0]);
                    Scale<Tag_>::value( d1c, (*_distribution_vector_x)[1]);
                    Scale<Tag_>::value( d2c, (*_distribution_vector_x)[2]);
                    Scale<Tag_>::value( d3c, (*_distribution_vector_x)[3]);
                    Scale<Tag_>::value( d4c, (*_distribution_vector_x)[4]);
                    Scale<Tag_>::value( d5c, (*_distribution_vector_x)[5]);
                    Scale<Tag_>::value( d6c, (*_distribution_vector_x)[6]);
                    Scale<Tag_>::value( d7c, (*_distribution_vector_x)[7]);
                    Scale<Tag_>::value( d8c, (*_distribution_vector_x)[8]);

                    DenseVector<ResPrec_> accu2(d0c.copy());

                    Sum<Tag_>::value(accu2, d1c);
                    Sum<Tag_>::value(accu2, d2c);
                    Sum<Tag_>::value(accu2, d3c);
                    Sum<Tag_>::value(accu2, d4c);
                    Sum<Tag_>::value(accu2, d5c);
                    Sum<Tag_>::value(accu2, d6c);
                    Sum<Tag_>::value(accu2, d7c);
                    Sum<Tag_>::value(accu2, d8c);

                    DenseVector<ResPrec_> h_inv(_height->copy());
                    ElementInverse<Tag_>::value(h_inv);
                    DenseVector<ResPrec_> h_inv_2(h_inv.copy());
                    *_u = ElementProduct<Tag_>::value(h_inv, accu2);

                    Scale<Tag_>::value( d0c_2, (*_distribution_vector_y)[0]);
                    Scale<Tag_>::value( d1c_2, (*_distribution_vector_y)[1]);
                    Scale<Tag_>::value( d2c_2, (*_distribution_vector_y)[2]);
                    Scale<Tag_>::value( d3c_2, (*_distribution_vector_y)[3]);
                    Scale<Tag_>::value( d4c_2, (*_distribution_vector_y)[4]);
                    Scale<Tag_>::value( d5c_2, (*_distribution_vector_y)[5]);
                    Scale<Tag_>::value( d6c_2, (*_distribution_vector_y)[6]);
                    Scale<Tag_>::value( d7c_2, (*_distribution_vector_y)[7]);
                    Scale<Tag_>::value( d8c_2, (*_distribution_vector_y)[8]);

                    DenseVector<ResPrec_> accu3(d0c_2.copy());

                    Sum<Tag_>::value(accu3, d1c_2);
                    Sum<Tag_>::value(accu3, d2c_2);
                    Sum<Tag_>::value(accu3, d3c_2);
                    Sum<Tag_>::value(accu3, d4c_2);
                    Sum<Tag_>::value(accu3, d5c_2);
                    Sum<Tag_>::value(accu3, d6c_2);
                    Sum<Tag_>::value(accu3, d7c_2);
                    Sum<Tag_>::value(accu3, d8c_2);

                    *_v = ElementProduct<Tag_>::value(h_inv_2, accu3);

                }

                /** Applies the noslip boundaries in north and south directions.
                 *
                 **/
                void _apply_noslip_boundaries()
                {
                    //TODO
                    /*for(unsigned long i(0) ; i < _grid_width ; ++i)
                    {
                        (*_temp_distribution_2)(0 , i) = (*_temp_distribution_6)(0 , i);
                        (*_temp_distribution_3)(0 , i) = (*_temp_distribution_7)(0 , i);
                        (*_temp_distribution_4)(0 , i) = (*_temp_distribution_8)(0 , i);

                        (*_temp_distribution_6)(_grid_height - 1 , i) = (*_temp_distribution_2)(_grid_height - 1, i);
                        (*_temp_distribution_7)(_grid_height - 1 , i) = (*_temp_distribution_3)(_grid_height - 1, i);
                        (*_temp_distribution_8)(_grid_height - 1 , i) = (*_temp_distribution_4)(_grid_height - 1, i);
                    }*/

                };

                void _apply_noslip_boundaries_2()
                {
                    /*for(unsigned long i(0) ; i < _grid_height; ++i)
                    {
                        (*_temp_distribution_8)(i , 0) = (*_temp_distribution_4)(i , 0);
                        (*_temp_distribution_1)(i , 0) = (*_temp_distribution_5)(i , 0);
                        (*_temp_distribution_2)(i , 0) = (*_temp_distribution_6)(i , 0);

                        (*_temp_distribution_4)(i , _grid_width - 1) = (*_temp_distribution_8)(i , _grid_width - 1);
                        (*_temp_distribution_5)(i , _grid_width - 1) = (*_temp_distribution_1)(i , _grid_width - 1);
                        (*_temp_distribution_6)(i , _grid_width - 1) = (*_temp_distribution_2)(i , _grid_width - 1);
                    }*/
                }

            public:
                SolverLABSWE(PackedGridData<D2Q9, ResPrec_> * data, PackedGridInfo<D2Q9> * info, ResPrec_ dx, ResPrec_ dy, ResPrec_ dt, unsigned long gx, unsigned long gy, DenseVector<ResPrec_>* bottom) :
                    _delta_x(dx),
                    _delta_y(dy),
                    _delta_t(dt),
                    _grid_width(gx),
                    _grid_height(gy),
                    _height(data->h),
                    _bottom(bottom),
                    _u(data->u),
                    _v(data->v),
                    _pi(3.14159265),
                    _gravity(9.80665),
                    _n_alpha(ResPrec_(6.)),
                    _relaxation_time(ResPrec_(1.5)),
                    _time(0),
                    _distribution_0(data->f_0),
                    _distribution_1(data->f_1),
                    _distribution_2(data->f_2),
                    _distribution_3(data->f_3),
                    _distribution_4(data->f_4),
                    _distribution_5(data->f_5),
                    _distribution_6(data->f_6),
                    _distribution_7(data->f_7),
                    _distribution_8(data->f_8),

                    _eq_distribution_0(data->f_eq_0),
                    _eq_distribution_1(data->f_eq_1),
                    _eq_distribution_2(data->f_eq_2),
                    _eq_distribution_3(data->f_eq_3),
                    _eq_distribution_4(data->f_eq_4),
                    _eq_distribution_5(data->f_eq_5),
                    _eq_distribution_6(data->f_eq_6),
                    _eq_distribution_7(data->f_eq_7),
                    _eq_distribution_8(data->f_eq_8),

                    _temp_distribution_0(data->f_temp_0),
                    _temp_distribution_1(data->f_temp_1),
                    _temp_distribution_2(data->f_temp_2),
                    _temp_distribution_3(data->f_temp_3),
                    _temp_distribution_4(data->f_temp_4),
                    _temp_distribution_5(data->f_temp_5),
                    _temp_distribution_6(data->f_temp_6),
                    _temp_distribution_7(data->f_temp_7),
                    _temp_distribution_8(data->f_temp_8)
                    {
                        CONTEXT("When creating LABSWE solver:");
                        _e = _delta_x / _delta_t;
                    }

                ~SolverLABSWE()
                {
                    //                CONTEXT("When destroying LABSWE solver.");
                }

                void set_distribution(DenseVector<ResPrec_>* dis_0,
                        DenseVector<ResPrec_>* dis_1,
                        DenseVector<ResPrec_>* dis_2,
                        DenseVector<ResPrec_>* dis_3,
                        DenseVector<ResPrec_>* dis_4,
                        DenseVector<ResPrec_>* dis_5,
                        DenseVector<ResPrec_>* dis_6,
                        DenseVector<ResPrec_>* dis_7,
                        DenseVector<ResPrec_>* dis_8)
                {
                    _distribution_0 = dis_0;
                    _distribution_1 = dis_1;
                    _distribution_2 = dis_2;
                    _distribution_3 = dis_3;
                    _distribution_4 = dis_4;
                    _distribution_5 = dis_5;
                    _distribution_6 = dis_6;
                    _distribution_7 = dis_7;
                    _distribution_8 = dis_8;
                }

                void set_eq_distribution(DenseVector<ResPrec_>* dis_0,
                        DenseVector<ResPrec_>* dis_1,
                        DenseVector<ResPrec_>* dis_2,
                        DenseVector<ResPrec_>* dis_3,
                        DenseVector<ResPrec_>* dis_4,
                        DenseVector<ResPrec_>* dis_5,
                        DenseVector<ResPrec_>* dis_6,
                        DenseVector<ResPrec_>* dis_7,
                        DenseVector<ResPrec_>* dis_8)
                {
                    _eq_distribution_0 = dis_0;
                    _eq_distribution_1 = dis_1;
                    _eq_distribution_2 = dis_2;
                    _eq_distribution_3 = dis_3;
                    _eq_distribution_4 = dis_4;
                    _eq_distribution_5 = dis_5;
                    _eq_distribution_6 = dis_6;
                    _eq_distribution_7 = dis_7;
                    _eq_distribution_8 = dis_8;
                }

                void set_vectors(DenseVector<ResPrec_>* vec_x,
                        DenseVector<ResPrec_>* vec_y)
                {
                    _distribution_vector_x = vec_x;
                    _distribution_vector_y = vec_y;
                }

                void set_source(DenseVector<ResPrec_>* s_x,
                        DenseVector<ResPrec_>* s_y)
                {
                    _source_x = s_x;
                    _source_y = s_y;
                }

                void set_temp_distribution(DenseVector<ResPrec_>* dis_0,
                        DenseVector<ResPrec_>* dis_1,
                        DenseVector<ResPrec_>* dis_2,
                        DenseVector<ResPrec_>* dis_3,
                        DenseVector<ResPrec_>* dis_4,
                        DenseVector<ResPrec_>* dis_5,
                        DenseVector<ResPrec_>* dis_6,
                        DenseVector<ResPrec_>* dis_7,
                        DenseVector<ResPrec_>* dis_8)
                {
                    _temp_distribution_0 = dis_0;
                    _temp_distribution_1 = dis_1;
                    _temp_distribution_2 = dis_2;
                    _temp_distribution_3 = dis_3;
                    _temp_distribution_4 = dis_4;
                    _temp_distribution_5 = dis_5;
                    _temp_distribution_6 = dis_6;
                    _temp_distribution_7 = dis_7;
                    _temp_distribution_8 = dis_8;
                }

                void set_slopes(DenseVector<ResPrec_>* d_x,
                        DenseVector<ResPrec_>* d_y)
                {
                    _d_bottom_x = d_x;
                    _d_bottom_y = d_y;
                }

                void do_preprocessing()
                {
                    CONTEXT("When performing LABSWE preprocessing.");
                    (*_distribution_vector_x)[0] = ResPrec_(0.);
                    (*_distribution_vector_x)[1] = ResPrec_(_e * cos(ResPrec_(0.)));
                    (*_distribution_vector_x)[2] = ResPrec_(sqrt(ResPrec_(2.)) * _e * cos(_pi / ResPrec_(4.)));
                    (*_distribution_vector_x)[3] = ResPrec_(_e * cos(_pi / ResPrec_(2.)));
                    (*_distribution_vector_x)[4] = ResPrec_(sqrt(ResPrec_(2.)) * _e * cos(ResPrec_(3.) * _pi / ResPrec_(4.)));
                    (*_distribution_vector_x)[5] = ResPrec_(_e * cos(_pi));
                    (*_distribution_vector_x)[6] = ResPrec_(sqrt(ResPrec_(2.)) * _e * cos(ResPrec_(5.) * _pi / ResPrec_(4.)));
                    (*_distribution_vector_x)[7] = ResPrec_(_e * cos(ResPrec_(3.) * _pi / ResPrec_(2.)));
                    (*_distribution_vector_x)[8] = ResPrec_(sqrt(ResPrec_(2.)) * _e * cos(ResPrec_(7.) * _pi / ResPrec_(4.)));
                    (*_distribution_vector_y)[0] = ResPrec_(0.);
                    (*_distribution_vector_y)[1] = ResPrec_(_e * sin(ResPrec_(0.)));
                    (*_distribution_vector_y)[2] = ResPrec_(sqrt(ResPrec_(2.)) * _e * sin(_pi / ResPrec_(4.)));
                    (*_distribution_vector_y)[3] = ResPrec_(_e * sin(_pi / ResPrec_(2.)));
                    (*_distribution_vector_y)[4] = ResPrec_(sqrt(ResPrec_(2.)) * _e * sin(ResPrec_(3.) * _pi / ResPrec_(4.)));
                    (*_distribution_vector_y)[5] = ResPrec_(_e * sin(_pi));
                    (*_distribution_vector_y)[6] = ResPrec_(sqrt(ResPrec_(2.)) * _e * sin(ResPrec_(5.) * _pi / ResPrec_(4.)));
                    (*_distribution_vector_y)[7] = ResPrec_(_e * sin(ResPrec_(3.) * _pi / ResPrec_(2.)));
                    (*_distribution_vector_y)[8] = ResPrec_(sqrt(ResPrec_(2.)) * _e * sin(ResPrec_(7.) * _pi / ResPrec_(4.)));

                    ///Compute bottom slopes in x and y direction TODO
                    /*for(unsigned long i(0); i < _grid_height; ++i)
                    {
                        for(unsigned long j(0); j < _grid_width; ++j)
                        {
                            ///BASIC scheme implies: only direct predecessors are involved
                            if(i > 0 && j > 0)
                            {
                                (*_d_bottom_x)(i,j) = ((*_bottom)(i,j) - (*_bottom)(i,j - 1)) / _delta_x;
                                (*_d_bottom_y)(i,j) = ((*_bottom)(i,j) - (*_bottom)(i - 1,j)) / _delta_y;
                            }
                            else if(i == 0 && j == 0)
                            {
                                (*_d_bottom_x)(i,j) = 0.;
                                (*_d_bottom_y)(i,j) = 0.;
                            }
                            else if(i == 0)
                            {
                                (*_d_bottom_x)(i,j) = ((*_bottom)(i,j) - (*_bottom)(i,j - 1)) / _delta_x;
                                (*_d_bottom_y)(i,j) = 0;
                            }
                            else
                            {
                                (*_d_bottom_x)(i,j) = 0;
                                (*_d_bottom_y)(i,j) = ((*_bottom)(i,j) - (*_bottom)(i - 1,j)) / _delta_y;
                            }
                        }
                    }*/

                    ///Compute initial equilibrium distribution:
                    EquilibriumDistribution<Tag_, lbm_applications::LABSWE>::
                        value(_gravity, _e, *_distribution_vector_x, *_distribution_vector_y, *data);

                    *_distribution_0 = *_eq_distribution_0;
                    *_distribution_1 = *_eq_distribution_1;
                    *_distribution_2 = *_eq_distribution_2;
                    *_distribution_3 = *_eq_distribution_3;
                    *_distribution_4 = *_eq_distribution_4;
                    *_distribution_5 = *_eq_distribution_5;
                    *_distribution_6 = *_eq_distribution_6;
                    *_distribution_7 = *_eq_distribution_7;
                    *_distribution_8 = *_eq_distribution_8;

#ifdef SOLVER_VERBOSE
                    std::cout << "feq after preprocessing (1) " << std::endl;
                    std::cout << *_eq_distribution_1;

                    std::cout << "h after preprocessing:" << std::endl;
                    std::cout << *_height << std::endl;
#endif
                }


                /** Capsule for the solution: Single step time marching.
                 *
                 **/
                void solve()
                {
                    ++_time;

                    ///Compute source terms:
                    /*Source<Tag_, lbm_applications::LABSWE, lbm_source_types::SIMPLE, lbm_source_schemes::BASIC>::
                        value(*_source_x, *_height, *_d_bottom_x, _gravity);
                    Source<Tag_, lbm_applications::LABSWE, lbm_source_types::SIMPLE, lbm_source_schemes::BASIC>::
                        value(*_source_y, *_height, *_d_bottom_y, _gravity);
                    */
                    /*Source<Tag_, lbm_applications::LABSWE, lbm_source_types::CONSTANT, lbm_source_schemes::BASIC>::
                    //value(*_source_x, ResPrec_(0.000024));
                    value(*_source_x, ResPrec_(0.));
                    Source<Tag_, lbm_applications::LABSWE, lbm_source_types::CONSTANT, lbm_source_schemes::BASIC>::
                    value(*_source_y, ResPrec_(0.));*/
                    ///Streaming and collision:

                    CollideStream<Tag_, lbm_applications::LABSWE, lbm_boundary_types::NOSLIP, lbm_lattice_types::D2Q9>::
                        value(*info,
                              *data,
                              *_source_x, *_source_y,
                              *_distribution_vector_x,
                              *_distribution_vector_y,
                              _relaxation_time);

                    ///Boundary correction:
                    _apply_noslip_boundaries();
                    _apply_noslip_boundaries_2();

                    ///Compute physical quantities:
                    _extract();

                    EquilibriumDistribution<Tag_, lbm_applications::LABSWE>::
                        value(_gravity, _e, *_distribution_vector_x, *_distribution_vector_y, *data);
                };
        };
}
#endif
