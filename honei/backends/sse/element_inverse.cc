/* vim: set sw=4 sts=4 et nofoldenable : */

/*
 * Copyright (c) 2008 Dirk Ribbrock <dirk.ribbrock@uni-dortmund.de>
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

#include <limits>
#include <xmmintrin.h>
#include <emmintrin.h>

namespace honei
{
    namespace sse
    {
        void element_inverse(float * x, unsigned long size)
        {
            __m128 m1, m2, m3, m4, m5, m6, m7, m8;
            float HONEI_ALIGNED(16) a_data(float(1));
            m8 = _mm_load1_ps(&a_data);
            float HONEI_ALIGNED(16) b_data(std::numeric_limits<float>::infinity());
            m7 = _mm_load1_ps(&b_data);

            unsigned long x_address((unsigned long)x);
            unsigned long x_offset(x_address % 16);

            unsigned long z_offset(x_offset / 4);
            z_offset = (4 - z_offset) % 4;

            unsigned long quad_start(z_offset);
            unsigned long quad_end(size - ((size - quad_start) % 20));

            if (size < 24)
            {
                quad_end = 0;
                quad_start = 0;
            }

            for (unsigned long index(quad_start) ; index < quad_end ; index += 20)
            {
                m1 = _mm_load_ps(x + index);
                m2 = _mm_load_ps(x + index + 4);
                m3 = _mm_load_ps(x + index + 8);
                m4 = _mm_load_ps(x + index + 12);
                m5 = _mm_load_ps(x + index + 16);

                m1 = _mm_div_ps(m8, m1);
                m2 = _mm_div_ps(m8, m2);
                m3 = _mm_div_ps(m8, m3);
                m4 = _mm_div_ps(m8, m4);
                m5 = _mm_div_ps(m8, m5);

                m6 = _mm_cmpneq_ps(m1, m7);
                m1 = _mm_and_ps(m1, m6);

                m6 = _mm_cmpneq_ps(m2, m7);
                m2 = _mm_and_ps(m2, m6);

                m6 = _mm_cmpneq_ps(m3, m7);
                m3 = _mm_and_ps(m3, m6);

                m6 = _mm_cmpneq_ps(m4, m7);
                m4 = _mm_and_ps(m4, m6);

                m6 = _mm_cmpneq_ps(m5, m7);
                m5 = _mm_and_ps(m5, m6);

                _mm_store_ps(x + index, m1);
                _mm_store_ps(x + index + 4, m2);
                _mm_store_ps(x + index + 8, m3);
                _mm_store_ps(x + index + 12, m4);
                _mm_store_ps(x + index + 16, m5);
            }

            for (unsigned long index = 0 ; index < quad_start ; ++index)
            {
                if (x[index] != float(0))
                    x[index] = float(1) / x[index];
            }
            for (unsigned long index = quad_end ; index < size ; ++index)
            {
                if (x[index] != float(0))
                    x[index] = float(1) / x[index];
            }
        }

        void element_inverse(double * x, unsigned long size)
        {
            __m128d m1, m2, m3, m4, m5, m6, m7, m8;
            double HONEI_ALIGNED(16) a_data(double(1));
            m8 = _mm_load1_pd(&a_data);
            double HONEI_ALIGNED(16) b_data(std::numeric_limits<double>::infinity());
            m7 = _mm_load1_pd(&b_data);

            unsigned long x_address((unsigned long)x);
            unsigned long x_offset(x_address % 16);

            unsigned long z_offset(x_offset / 8);

            unsigned long quad_start(z_offset);
            unsigned long quad_end(size - ((size - quad_start) % 10));
            if (size < 12)
            {
                quad_end = 0;
                quad_start = 0;
            }

            for (unsigned long index = quad_start ; index < quad_end ; index += 10)
            {
                m1 = _mm_load_pd(x + index);
                m2 = _mm_load_pd(x + index + 2);
                m3 = _mm_load_pd(x + index + 4);
                m4 = _mm_load_pd(x + index + 6);
                m5 = _mm_load_pd(x + index + 8);

                m1 = _mm_div_pd(m8, m1);
                m2 = _mm_div_pd(m8, m2);
                m3 = _mm_div_pd(m8, m3);
                m4 = _mm_div_pd(m8, m4);
                m5 = _mm_div_pd(m8, m5);

                m6 = _mm_cmpneq_pd(m1, m7);
                m1 = _mm_and_pd(m1, m6);

                m6 = _mm_cmpneq_pd(m2, m7);
                m2 = _mm_and_pd(m2, m6);

                m6 = _mm_cmpneq_pd(m3, m7);
                m3 = _mm_and_pd(m3, m6);

                m6 = _mm_cmpneq_pd(m4, m7);
                m4 = _mm_and_pd(m4, m6);

                m6 = _mm_cmpneq_pd(m5, m7);
                m5 = _mm_and_pd(m5, m6);

                _mm_store_pd(x + index, m1);
                _mm_store_pd(x + index + 2, m2);
                _mm_store_pd(x + index + 4, m3);
                _mm_store_pd(x + index + 6, m4);
                _mm_store_pd(x + index + 8, m5);
            }

            for (unsigned long index(0) ; index < quad_start ; ++index)
            {
                if (x[index] != double(0))
                    x[index] = double(1) / x[index];
            }

            for (unsigned long index(quad_end) ; index < size ; ++index)
            {
                if (x[index] != double(0))
                    x[index] = double(1) / x[index];
            }
        }
    }
}

