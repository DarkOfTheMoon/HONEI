
/* vim: set sw=4 sts=4 et nofoldenable : */

/*
 * Copyright (c) 2007 Dirk Ribbrock <dirk.ribbrock@uni-dortmund.de>
 * Copyright (c) 2007 Sven Mallach <sven.mallach@uni-dortmund.de>
 *
 * This file is part of the LA C++ library. LibLa is free software;
 * you can redistribute it and/or modify it under the terms of the GNU General
 * Public License version 2, as published by the Free Software Foundation.
 *
 * LibLa is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <libla/product.hh>
#include <cstring>
#include <xmmintrin.h>

using namespace honei;

DenseVector<float> Product<tags::CPU::SSE>::value(const BandedMatrix<float> & a, const DenseVector<float> & b)
{
    CONTEXT("When multiplying BandedMatrix with DenseVector with SSE:");


    if (a.size() != b.size())
        throw MatrixSizeDoesNotMatch(b.size(), a.size());

    DenseVector<float> result(a.rows(), float(0));

    __m128 m1, m2, m3, m4, m5, m6;

    float __attribute__((aligned(16))) band1_data[4];
    float __attribute__((aligned(16))) band2_data[4];
    float __attribute__((aligned(16))) vector1_data[4];
    float __attribute__((aligned(16))) vector2_data[4];
    float __attribute__((aligned(16))) result1_data[4];
    float __attribute__((aligned(16))) result2_data[4];


    unsigned long middle_index(a.rows() - 1);

    // If we are above or on the diagonal band, we start at Element 0 and go on until Element band_size-band_index.
    for (BandedMatrix<float>::ConstVectorIterator vi(a.band_at(middle_index)), vi_end(a.end_bands()) ;
            vi != vi_end ; ++vi)
    {
        if (! vi.exists())
            continue;
        unsigned long end(vi->size() - (vi.index() - middle_index)); //Calculation of the element-index to stop in iteration!
        unsigned long quad_end(end - (end % 8));

        for (unsigned long index = 0 ; index < quad_end ; index+=8)
        {
            for (int i = 0 ; i < 4 ; ++i)
            {
                band1_data[i] = vi->elements()[index + i];
                band2_data[i] = vi->elements()[index + i + 4];
                vector1_data[i] = b.elements()[index + i];
                vector2_data[i] = b.elements()[index + i + 4];
                result1_data[i] = result.elements()[index + i];
                result2_data[i] = result.elements()[index + i + 4];
            }
            m1 = _mm_load_ps(band1_data);
            m2 = _mm_load_ps(vector1_data);
            m3 = _mm_load_ps(result1_data);
            m4 = _mm_load_ps(band2_data);
            m5 = _mm_load_ps(vector2_data);
            m6 = _mm_load_ps(result2_data);

            m1 = _mm_mul_ps(m1, m2);
            m1 = _mm_add_ps(m1, m3);
            m4 = _mm_mul_ps(m4, m5);
            m4 = _mm_add_ps(m4, m6);

            _mm_store_ps(result1_data, m1);
            _mm_store_ps(result2_data, m4);

            for (int i = 0 ; i < 4 ; ++i)
            {
                result.elements()[index + i] = result1_data[i];
                result.elements()[index + i + 4] = result2_data[i];
            }
        }

        for (unsigned long index = quad_end ; index < end ; index++) 
        {
            result.elements()[index] += vi->elements()[index] * b.elements()[index];
        }
    }

    // If we are below the diagonal band, we start at Element index and go on until the last element.
    for (BandedMatrix<float>::ConstVectorIterator vi(a.begin_bands()), vi_end(a.band_at(middle_index)) ;
            vi != vi_end ; ++vi)
    {
        if (! vi.exists())
            continue;
        unsigned long start(middle_index - vi.index()); //Calculation of the element-index to start in iteration!
        unsigned long end(a.size());
        unsigned long quad_end(end - ((end - start) % 8));

        for (unsigned long index = 0 ; index < quad_end - start ; index+=8)
        {
            for (int i = 0 ; i < 4 ; ++i)
            {
                band1_data[i] = vi->elements()[start + index + i];
                band2_data[i] = vi->elements()[start + index + i + 4];
                vector1_data[i] = b.elements()[start + index + i];
                vector2_data[i] = b.elements()[start + index + i + 4];
                result1_data[i] = result.elements()[start + index + i];
                result2_data[i] = result.elements()[start + index + i + 4];
            }
            m1 = _mm_load_ps(band1_data);
            m2 = _mm_load_ps(vector1_data);
            m3 = _mm_load_ps(result1_data);
            m4 = _mm_load_ps(band2_data);
            m5 = _mm_load_ps(vector2_data);
            m6 = _mm_load_ps(result2_data);

            m1 = _mm_mul_ps(m1, m2);
            m1 = _mm_add_ps(m1, m3);
            m4 = _mm_mul_ps(m4, m5);
            m4 = _mm_add_ps(m4, m6);

            _mm_store_ps(result1_data, m1);
            _mm_store_ps(result2_data, m4);

            for (int i = 0 ; i < 4 ; ++i)
            {
                result.elements()[start + index + i] = result1_data[i];
                result.elements()[start + index + i + 4] = result2_data[i];
            }
        }

        for (unsigned long index = quad_end ; index < end ; index++)
        {
            result.elements()[index] += vi->elements()[index] * b.elements()[index];
        }
    }
    return result;
}
