/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2009 Markus Geveler <apryde@gmx.de>
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

#include <honei/util/attributes.hh>

#include <xmmintrin.h>
#include <emmintrin.h>
#include <iostream>

namespace honei {

    namespace sse {

        float scaled_product_sum_norm(unsigned long size, float a, float * y, float b, float * A_x)
        {
            //__m128 m1, m2, m3, m4, m5;

            union sse4
            {
                __m128 m;
                float f[4];
            } m1, m2, m3, m4, m5, m6, m7, m8;

            float result(0);

            //compute ||ay + bA_x|| (SSE)
            m1.m = _mm_set_ps1(a);
            m2.m = _mm_set_ps1(b);

            m5.m = _mm_setzero_ps();

            unsigned long quad_end(size - (size % 4));
            for (unsigned long i(0) ; i < quad_end ; i += 4)
            {
                m3.m = _mm_load_ps(A_x + i);
                m4.m = _mm_load_ps(y + i);

                m4.m = _mm_mul_ps(m1.m, m4.m);
                m3.m = _mm_mul_ps(m2.m, m3.m);

                m3.m = _mm_add_ps(m3.m, m4.m);
                m3.m = _mm_mul_ps(m3.m, m3.m);

                m5.m = _mm_add_ps(m5.m, m3.m);
            }

            result += m5.f[0];
            result += m5.f[1];
            result += m5.f[2];
            result += m5.f[3];

            //compute ||ay + bA_x|| (FPU)
            for (unsigned long i(quad_end) ; i < size ; ++i)
            {
                result += (a * y[i] + b * A_x[i]) * (a * y[i] + b * A_x[i]);
            }

            return result;
        }
    }
}
