/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2008 Dirk Ribbrock <dirk.ribbrock@uni-dortmund.de>
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

#include <honei/backends/cell/cell.hh>
#include <honei/backends/cell/spe/libla/operations.hh>
#include <honei/backends/cell/spe/libutil/allocator.hh>
#include <honei/backends/cell/spe/libutil/transfer.hh>

#include <spu_intrinsics.h>

namespace honei
{
    namespace cell
    {
        namespace implementation
        {
            void product_banded_matrix_dense_vector_float(vector float * a, const vector float * b, const vector float * c,
                    const unsigned size, vector float & b_carry, const unsigned b_offset, vector float & c_carry,
                    const unsigned c_offset, const float a_scalar, const float b_scalar, const float c_scalar)
            {
                for (unsigned i(0) ; i < size ; ++i)
                {
                    extract(b_carry, b[i], b_offset);
                    extract(c_carry, c[i], c_offset);
                    a[i] = spu_madd(b_carry, c_carry, a[i]);
                    b_carry = b[i];
                    c_carry = c[i];
                }
            }
        }

        namespace operations
        {
            Operation<3, float, rtm_dma> product_banded_matrix_dense_vector_float = {
                &implementation::product_banded_matrix_dense_vector_float
            };
        }
    }
}
