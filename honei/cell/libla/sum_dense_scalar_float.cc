/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2007 Danny van Dyk <danny.dyk@uni-dortmund.de>
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

#include <honei/cell/cell.hh>
#include <honei/cell/libutil/allocator.hh>
#include <honei/cell/libutil/debug.hh>

#include <spu_intrinsics.h>
#include <spu_mfcio.h>

using namespace honei::cell;

/*
 * dense_scalar_float_sum
 *
 * Calculate the sum of a dense entity and a scalar.
 *
 * \size Default transfer buffer size in bytes.
 * \operand a Base address of first entity.
 * \operand b Scalar summand.
 * \operand c Number of transfers needed.
 * \operand d Last transfer buffer size in bytes.
 */
void sum_dense_scalar_float(const Instruction & inst)
{
    EffectiveAddress ea_a(inst.a.ea), ea_result(inst.a.ea);

    Allocation * block_a[2] = { acquire_block(), acquire_block() };

    Pointer<float> a[2] = { { block_a[0]->address }, { block_a[1]->address } };

    unsigned counter(inst.c.u);
    unsigned size(counter > 1 ? inst.size : inst.d.u);
    unsigned nextsize;
    unsigned current(1), next(2);

    mfc_get(a[current - 1].untyped, ea_a, size, current, 0, 0);
    ea_a += size;

    while (counter > 1)
    {
        nextsize = (counter == 2 ? inst.d.u : inst.size);

        mfc_get(a[next - 1].untyped, ea_a, nextsize, next, 0, 0);
        ea_a += nextsize;

        mfc_write_tag_mask(1 << current);
        mfc_read_tag_status_all();

        vector float b = { inst.b.f, inst.b.f, inst.b.f, inst.b.f };
        for (unsigned i(0) ; i < size / sizeof(vector float) ; ++i)
        {
            a[current - 1].vectorised[i] = spu_add(a[current - 1].vectorised[i], b);
        }

        mfc_putb(a[current - 1].untyped, ea_result, size, current, 0, 0);
        ea_result += size;

        --counter;

        unsigned temp(next);
        next = current;
        current = temp;

        size = nextsize;
    }

    mfc_write_tag_mask(1 << current);
    mfc_read_tag_status_all();

    vector float b = { inst.b.f, inst.b.f, inst.b.f, inst.b.f };
    for (unsigned i(0) ; i < size / sizeof(vector float) ; ++i)
    {
        a[current - 1].vectorised[i] = spu_add(a[current - 1].vectorised[i], b);
    }

    mfc_putb(a[current - 1].untyped, ea_result, size, current, 0, 0);

    mfc_write_tag_mask(1 << current);
    mfc_read_tag_status_all();

    release_block(*block_a[0]);
    release_block(*block_a[1]);
}