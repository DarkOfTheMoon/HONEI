/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2010 Dirk Ribbrock <dirk.ribbrock@math.uni-dortmund.de>
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

#pragma once
#ifndef OPENCL_GUARD_OPERATIONS_HH
#define OPENCL_GUARD_OPERATIONS_HH 1

#include <CL/cl.h>
#include <string>
#include <honei/backends/opencl/opencl_backend.hh>

namespace honei
{
    namespace opencl
    {
        void difference(void * r, void * x, void * y, unsigned long size, cl_device_type type, std::string function);

        template <typename DT_>
        DT_ dot_product(void * x, void * y, unsigned long size, cl_device_type type, std::string function);

        void element_product(void * r, void * x, void * y, unsigned long size, cl_device_type type, std::string function);

        void fill_float(void * x, float a, unsigned long size, cl_device_type type);
        void fill_double(void * x, double a, unsigned long size, cl_device_type type);

        template <typename DT_>
        DT_ norm_l2_false(void * x, unsigned long size, cl_device_type type, std::string function);

        template <typename DT_>
        void scale(void * x, DT_ a, unsigned long size, cl_device_type type, std::string function);

        template <typename DT_>
        void scaled_sum(void * r, void * x, void * y, DT_ b, unsigned long size, cl_device_type type, std::string function);
        void scaled_sum(void * r, void * x, void * y, unsigned long size, cl_device_type type, std::string function);

        void sum(void * r, void * x, void * y, unsigned long size, cl_device_type type, std::string function);

        template <typename DT_>
        void product_smell_dv(void * x, void * y, void * Aj, void * Ax, void * Arl,
                unsigned long num_rows, unsigned long num_cols, unsigned long num_cols_per_row,
                unsigned long stride, unsigned long threads, cl_device_type type, std::string function);

        template <typename DT_>
        void product_bmdv_q1(void * ll, void * ld, void * lu,
                void * dl, void * dd, void *du,
                void * ul, void * ud, void *uu, void * x, void * y,
                unsigned long size, unsigned long m, cl_device_type type, std::string function);

        template <typename DT_>
        void defect_smell_dv(void * rhs, void * x, void * y, void * Aj, void * Ax, void * Arl,
                unsigned long num_rows, unsigned long num_cols, unsigned long num_cols_per_row,
                unsigned long stride, unsigned long threads, cl_device_type type, std::string function);

        template <typename DT_>
        void defect_q1(void * ll, void * ld, void * lu,
                void * dl, void * dd, void *du,
                void * ul, void * ud, void *uu, void * rhs, void * x, void * y,
                unsigned long size, unsigned long m, cl_device_type type, std::string function);

        template <typename DT_>
        void extraction_grid_dry(unsigned long start, unsigned long end,
                void * f_0, void * f_1, void * f_2,
                void * f_3, void * f_4, void * f_5,
                void * f_6, void * f_7, void * f_8,
                void * h, void * u, void * v,
                void * distribution_x, void * distribution_y, DT_ epsilon,
                cl_device_type type, std::string function);

        template <typename DT_>
        void extraction_grid_wet(unsigned long start, unsigned long end,
                void * f_0, void * f_1, void * f_2,
                void * f_3, void * f_4, void * f_5,
                void * f_6, void * f_7, void * f_8,
                void * h, void * u, void * v,
                void * distribution_x, void * distribution_y, DT_ epsilon,
                cl_device_type type, std::string function);

        template <typename DT_>
        void eq_dist_grid(unsigned long start, unsigned long end,
                void * f_0, void * f_1, void * f_2,
                void * f_3, void * f_4, void * f_5,
                void * f_6, void * f_7, void * f_8,
                void * h, void * u, void * v,
                void * distribution_x, void * distribution_y, DT_ g, DT_ e,
                cl_device_type type, std::string function);

        template <typename DT_>
        void collide_stream_grid(unsigned long start, unsigned long end,
                void * dir_1, void * dir_2, void * dir_3, void * dir_4,
                void * dir_5, void * dir_6, void * dir_7, void * dir_8,
                void * f_eq_0, void * f_eq_1, void * f_eq_2,
                void * f_eq_3, void * f_eq_4, void * f_eq_5,
                void * f_eq_6, void * f_eq_7, void * f_eq_8,
                void * f_0, void * f_1, void * f_2,
                void * f_3, void * f_4, void * f_5,
                void * f_6, void * f_7, void * f_8,
                void * f_temp_0, void * f_temp_1, void * f_temp_2,
                void * f_temp_3, void * f_temp_4, void * f_temp_5,
                void * f_temp_6, void * f_temp_7, void * f_temp_8,
                DT_ tau, unsigned long size,
                cl_device_type type, std::string function);

        void update_velocity_directions_grid(unsigned long start, unsigned long end,
                void * types, void * f_temp_1, void * f_temp_2,
                void * f_temp_3, void * f_temp_4, void * f_temp_5,
                void * f_temp_6, void * f_temp_7, void * f_temp_8,
                cl_device_type type, std::string function);
    }
}
#endif
