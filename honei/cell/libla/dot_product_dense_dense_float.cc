/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2007 Danny van Dyk <danny.dyk@uni-dortmund.de>
 * Copyright (c) 2007, 2008 Sven Mallach <sven.mallach@honei.org>
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
#include <honei/cell/libutil/transfer.hh>

#include <spu_intrinsics.h>
#include <spu_mfcio.h>

using namespace honei::cell;

/*
 * dense_dense_float_dot_product
 *
 * Calculate the dot product of two instances of DenseVectorContinuousBase
 *
 * \size Default transfer buffer size in bytes.
 * \operand a Address of return value.
 * \operand b Base address of first container.
 * \operand c Base address of second container.
 * \operand d Number of transfers needed.
 * \operand e Last transfer buffer size in bytes.
 * \operand f Alignment offset of container a
 * \operand g Alignment offset of container b.
 */
unsigned dot_product_dense_dense_float(const Instruction & inst)
{
    EffectiveAddress ea_a(inst.b.ea), ea_b(inst.c.ea);

    Allocation * block_a[2] = { acquire_block(), acquire_block() };
    Allocation * block_b[2] = { acquire_block(), acquire_block() };

    Pointer<float> a[2] = { { block_a[0]->address }, { block_a[1]->address } };
    Pointer<float> b[2] = { { block_b[0]->address }, { block_b[1]->address } };

    unsigned counter(inst.d.u);
    unsigned size(counter > 1 ? inst.size : inst.e.u);
    unsigned nextsize;
    unsigned current(0), next(1);

    unsigned b_offset(inst.f.u);
    vector float b_carry = { inst.g.f, inst.h.f, inst.i.f, inst.j.f };

    mfc_get(a[current].untyped, ea_a, size, current, 0, 0);
    mfc_get(b[current].untyped, ea_b, size, current, 0, 0);
    ea_a += size;
    ea_b += size;

    Subscriptable<float> acc = { spu_splats(0.0f) };

    while (counter > 1)
    {
        nextsize = (counter == 2 ? inst.e.u : inst.size);

        mfc_get(a[next].untyped, ea_a, nextsize, next, 0, 0);
        mfc_get(b[next].untyped, ea_b, nextsize, next, 0, 0);
        ea_a += nextsize;
        ea_b += nextsize;

        mfc_write_tag_mask(1 << current);
        mfc_read_tag_status_all();

        for (unsigned i(0) ; i < size / sizeof(vector float) ; ++i)
        {
            extract(b_carry, b[current].vectorised[i], b_offset);
            acc.value = spu_madd(a[current].vectorised[i], b_carry, acc.value);
            b_carry = b[current].vectorised[i];
        }

        --counter;

        unsigned temp(next);
        next = current;
        current = temp;

        size = nextsize;
    }

    mfc_write_tag_mask(1 << current);
    mfc_read_tag_status_all();

    for (unsigned i(0) ; i < size / sizeof(vector float) ; ++i)
    {
        extract(b_carry, b[current].vectorised[i], b_offset);
        acc.value = spu_madd(a[current].vectorised[i], b_carry, acc.value);
        b_carry = b[current].vectorised[i];
    }

    release_all_blocks();

    MailableResult<float> result = { acc.array[0] + acc.array[1] + acc.array[2] + acc.array[3] };

    return result.mail;
}