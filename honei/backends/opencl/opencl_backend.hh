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
#ifndef OPENCL_GUARD_OPENCL_BACKEND_HH
#define OPENCL_GUARD_OPENCL_BACKEND_HH 1

#include <CL/cl.h>
#include <string>
#include <cstring>
#include <honei/util/instantiation_policy.hh>
#include <honei/util/private_implementation_pattern.hh>
#include <honei/util/exception.hh>
#include <honei/util/tags.hh>

namespace honei
{
    namespace opencl
    {
        struct DCQ
        {
            cl_device_id device;
            cl_context context;
            cl_command_queue command_queue;
            cl_device_type type;
        };

        template <typename Tag_>
        cl_device_type tag_to_device();

        template <>
        cl_device_type tag_to_device<tags::OpenCL::CPU>();

        template <>
        cl_device_type tag_to_device<tags::OpenCL::GPU>();

        template <>
        cl_device_type tag_to_device<tags::OpenCL::Accelerator>();

        class OpenCLBackend :
            public PrivateImplementationPattern<OpenCLBackend, Shared> ,
            public InstantiationPolicy<OpenCLBackend, Singleton>
        {
            public:
                friend class InstantiationPolicy<OpenCLBackend, Singleton>;

                cl_kernel create_kernel(std::string file, std::string kernel_name, cl_context context, cl_device_id device);
                DCQ prepare_device(cl_device_type type);
                void print_device_info(cl_device_id device);
                void print_program_info(cl_program program, cl_device_id device);
                void print_platform_info();
                void flush();
                cl_mem create_empty_buffer(unsigned long bytes, cl_context context);
                cl_mem create_buffer(unsigned long size, cl_context context, void * src);

            private:
                /// Constructor
                OpenCLBackend();

                /// Destructor
                ~OpenCLBackend();
        };
    }
}
#endif
