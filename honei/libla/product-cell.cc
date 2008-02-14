/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2007 Danny van Dyk <danny.dyk@uni-dortmund.de>
 * Copyright (c) 2007 Dirk Ribbrock <dirk.ribbrock@uni-dortmund.de>
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
#include <honei/libla/product.hh>
#include <honei/libla/sum.hh>
#include <honei/libutil/memory_backend_cell.hh>
#include <honei/libutil/spe_instruction.hh>
#include <honei/libutil/spe_manager.hh>
#include <honei/libutil/profiler.hh>
#include <list>

//#include <honei/libutil/time_stamp.hh>

namespace honei
{
    using namespace cell;

    DenseVector<float> Product<tags::Cell>::value(const DenseMatrix<float> & a, const DenseVector<float> & x)
    {
        CONTEXT("When calculating DenseMatrix<float>-DenseVector<float> product (Cell):");

        if (x.size() != a.columns())
            throw VectorSizeDoesNotMatch(x.size(), a.columns());

        DenseVector<float> result(a.rows(), 0.0f);
        std::list<SPEInstruction *> instructions;

        unsigned long spe_count(Configuration::instance()->get_value("cell::product_dense_matrix_dense_vector_float", 4ul));
        spe_count = std::min(spe_count, SPEManager::instance()->spe_count());
        std::cout << "a.rows() = " << a.rows() << std::endl;
        std::cout << "spe_count " << spe_count << std::endl;
        unsigned rows_per_spe(a.rows() / spe_count);
        unsigned ppu_rows(0);
        bool use_spe(true);

        if (a.rows() < (4 * spe_count))
        {
            ppu_rows = a.rows();
            spe_count = 0;
            rows_per_spe = 0;
            use_spe = false;
        }

        if (use_spe)
        {
            unsigned parts(spe_count);
            while (rows_per_spe > 4096)
            {
                rows_per_spe /= 2;
                parts *= 2;
            }
            rows_per_spe & ~0x3;
            ppu_rows = a.rows() - (spe_count * rows_per_spe);

            union addr
            {
                float * ptr;
                unsigned long long value;
            };

            unsigned a_t_size(a.columns() * (4096 / (a.columns())));
            while (a_t_size % 4 != 0)
            {
                a_t_size -= a.columns();
            }
            a_t_size *= 4;
            std::cout << "ROWS PER SPE : " << rows_per_spe << std::endl;

            for (unsigned i(0) ; i < parts ; i++)
            {
                addr one = { (a.elements() +  a.columns()) };
                unsigned a_offset(one.value & 0xf);

                Operand oa = { a.elements() + (rows_per_spe * i * a.columns()) };
                Operand ob = { x.elements() };
                Operand oc = { result.elements() + (rows_per_spe * i) };

                Operand od, oe, of, og;
                // Transfer only full rows.
                od.u = (4 * rows_per_spe * a.columns()) / a_t_size;
                oe.u = (4 * rows_per_spe * a.columns()) % a_t_size;
                of.u = (4 * x.size()) / 16384;
                og.u = (4 * x.size()) % 16384;

                if (0 == oe.u)
                {
                    if (od.u > 0)
                    {
                        oe.u = a_t_size;
                    }
                }
                else
                {
                    ++od.u;
                }

                if (0 == og.u)
                {
                    if (of.u > 0)
                    {
                        og.u = 16384;
                    }
                }
                else
                {
                    ++of.u;
                }

                Operand oh;
                oh.u = a.columns();
                Operand oi;
                oi.u = a_offset / sizeof(float);

                if (i % spe_count == 0) // wait for the last parts to have finished.
                {
                    for(std::list<SPEInstruction *>::iterator i(instructions.begin()), i_end(instructions.end()) ; i != i_end ; i++)
                    {
                        (*i)->wait();
                        delete *i;
                        instructions.erase(i);
                    }
                }

                SPEInstruction * instruction = new SPEInstruction(oc_product_dense_matrix_dense_vector_float, a_t_size, oa, ob, oc, od, oe, of, og, oh, oi);
                instructions.push_back(instruction);
                SPEManager::instance()->dispatch(*instruction);
            }
        }

        for (unsigned i(0) ; i < ppu_rows ; i++)
        {
            DenseVectorRange<float> row = a[(spe_count * rows_per_spe) + i];
            for (Vector<float>::ConstElementIterator c(row.begin_elements()), c_end(row.end_elements()), d(x.begin_elements()) ; c != c_end ; ++c, ++d)
            {
                result[(spe_count * rows_per_spe) + i] += *c * *d;
            }
        }

        if (use_spe)
        {
            for(std::list<SPEInstruction *>::iterator i(instructions.begin()), i_end(instructions.end()) ; i != i_end ; i++)
            {
                (*i)->wait();
                delete *i;
            }
        }

        return result;
    }

    DenseVector<float> Product<tags::Cell>::value(const BandedMatrix<float> & a, const DenseVector<float> & b)
    {
        CONTEXT("When calculating BandedMatrix<float>-DenseVector<float> product (Cell):");

        PROFILER_START("Product<Cell>::value(bm, dv)");
        if (b.size() != a.columns())
            throw VectorSizeDoesNotMatch(b.size(), a.columns());

        /// \todo Remove SPEIQ list when one SPEInstructionQueue can handle more than 8 instructions.
        std::list<SPEInstructionQueue *> iq_upper, iq_lower;
        //TimeStamp dt,ct, as1, as2;
        //ct.take();
        PROFILER_START("Product<Cell>::value(bm, dv)->DV(size, 0)");
        DenseVector<float> result(b.size(), 0.0f);
        PROFILER_STOP("Product<Cell>::value(bm, dv)->DV(size, 0)");
        //dt.take();
        //std::cout<<"dv(0): "<<dt.sec() - ct.sec() << " "<<dt.usec() - ct.usec()<<std::endl;
        /// \todo Fill the result vector on the spu side.
        //DenseVector<float> result(b.size());
        //as1.take();

        unsigned long middle_index(a.rows() - 1);
        unsigned long quad_end, end, quad_start, start, x_offset, op_offset;
        for (BandedMatrix<float>::ConstVectorIterator band(a.begin_non_zero_bands()), band_end(a.end_non_zero_bands()) ;
                band != band_end ; ++band)
        {
            // If we are above or on the diagonal band, we start at Element 0 and go on until Element band_size-band_index.
            if (band.index() >= middle_index)
            {
                {
                    // Lower result part
                    op_offset = band.index() - middle_index;
                    start = 0;
                    quad_start = 0;
                    end = std::min(band->size() - op_offset, result.size() / 2); //Calculation of the element-index to stop in iteration!
                    quad_end = end - (end % 4);

                    Operand oa = { band->elements() + quad_start };
                    Operand ob = { b.elements() + quad_start + op_offset - (op_offset % 4) };
                    Operand oc = { result.elements() + quad_start };
                    Operand od, oe, of, og;
                    /// \todo use such a transfer size, that od.u is at least 2
                    od.u = (quad_end - quad_start) / (1000 * 4);
                    oe.u = (quad_end - quad_start) % (1000 * 4);
                    if (0 == oe.u)
                    {
                        if (od.u > 0)
                        {
                            oe.u = 1000 * 4;
                        }
                    }
                    else
                    {
                        ++od.u;
                    }

                    og.u = op_offset % 4;
                    if(quad_end > quad_start)
                    {
                        if (iq_lower.empty() || iq_lower.back()->size() == 7)
                        {
                            iq_lower.push_back(new SPEInstructionQueue);
                            iq_lower.back()->push_back(SPEInstruction(oc_product_banded_matrix_dense_vector_float, 1000 * 4, oa, ob, oc, od, oe, of, og));
                        }
                        else
                        {
                            iq_lower.back()->push_back(SPEInstruction(oc_product_banded_matrix_dense_vector_float, 1000 * 4, oa, ob, oc, od, oe, of, og));
                        }
                    }
                    else
                    {
                        quad_start = 0;
                        quad_end = 0;
                    }

                    for (unsigned long index = quad_end ; index < end ; index++)
                    {
                        result.elements()[index] += band->elements()[index] * b.elements()[index + op_offset];
                    }
                }

                {
                    // Upper result part
                    op_offset = band.index() - middle_index;
                    start = result.size() / 2;
                    quad_start = start + ((4 - (start % 4)) % 4);
                    end = band->size() - op_offset; //Calculation of the element-index to stop in iteration!
                    quad_end = end - (end % 4);

                    Operand oa = { band->elements() + quad_start };
                    Operand ob = { b.elements() + quad_start + op_offset - (op_offset % 4) };
                    Operand oc = { result.elements() + quad_start };
                    Operand od, oe, of, og, oh;

                    /// \todo use such a transfer size, that od.u is at least 2
                    od.u = (quad_end - quad_start) / (1000 * 4);
                    oe.u = (quad_end - quad_start) % (1000 * 4);
                    if (0 == oe.u)
                    {
                        if (od.u > 0)
                        {
                            oe.u = 1000 * 4;
                        }
                    }
                    else
                    {
                        ++od.u;
                    }

                    og.u = op_offset % 4;
                    if(quad_end > quad_start)
                    {
                        if (iq_upper.empty() || iq_upper.back()->size() == 7)
                        {
                            iq_upper.push_back(new SPEInstructionQueue);
                            iq_upper.back()->push_back(SPEInstruction(oc_product_banded_matrix_dense_vector_float, 1000 * 4, oa, ob, oc, od, oe, of, og));
                        }
                        else
                        {
                            iq_upper.back()->push_back(SPEInstruction(oc_product_banded_matrix_dense_vector_float, 1000 * 4, oa, ob, oc, od, oe, of, og));
                        }
                    }
                    else
                    {
                        quad_start = result.size() / 2;
                        quad_end = result.size() / 2;
                    }

                    for (unsigned long index = start ; index < quad_start ; index++)
                    {
                        result.elements()[index] += band->elements()[index] * b.elements()[index + op_offset];
                    }
                    for (unsigned long index = quad_end ; index < end ; index++)
                    {
                        result.elements()[index] += band->elements()[index] * b.elements()[index + op_offset];
                    }
                }
            }

            // If we are below the diagonal band, we start at Element 'start' and go on until the last element.
            else
            {
                {
                    // Lower result part
                    op_offset = middle_index - band.index();
                    start = op_offset; //Calculation of the element-index to start in iteration!
                    quad_start = start + ((4 - (start % 4)) % 4);
                    end = result.size() / 2;
                    quad_end = end - (end % 4);
                    Operand oa = { band->elements() + quad_start};
                    Operand ob = { b.elements() + quad_start - op_offset - ((4 - (op_offset % 4)) % 4)};
                    Operand oc = { result.elements() + quad_start};
                    Operand od, oe, of, og;

                    od.u = (quad_end - quad_start) / (1000 * 4);
                    oe.u = (quad_end - quad_start) % (1000 * 4);
                    if (0 == oe.u)
                    {
                        if (od.u > 0)
                        {
                            oe.u = 1000 * 4;
                        }
                    }
                    else
                    {
                        ++od.u;
                    }

                    og.u = (4 - (op_offset % 4)) % 4;
                    if(quad_end > quad_start)
                    {
                        if (iq_lower.empty() || iq_lower.back()->size() == 7)
                        {
                            iq_lower.push_back(new SPEInstructionQueue);
                            iq_lower.back()->push_back(SPEInstruction(oc_product_banded_matrix_dense_vector_float, 1000 * 4, oa, ob, oc, od, oe, of, og));
                        }
                        else
                        {
                            iq_lower.back()->push_back(SPEInstruction(oc_product_banded_matrix_dense_vector_float, 1000 * 4, oa, ob, oc, od, oe, of, og));
                        }
                    }
                    else
                    {
                        quad_start = 0;
                        quad_end = start;
                    }

                    for (unsigned long index = start ; index < quad_start ; index++)
                    {
                        result.elements()[index] += band->elements()[index] * b.elements()[index - op_offset];
                    }
                    for (unsigned long index = quad_end ; index < end ; index++)
                    {
                        result.elements()[index] += band->elements()[index] * b.elements()[index - op_offset];
                    }
                }

                {
                    // Upper result part
                    op_offset = middle_index - band.index();
                    start = std::max(op_offset, result.size() / 2); //Calculation of the element-index to start in iteration!
                    quad_start = start + ((4 - (start % 4)) % 4);
                    end = band->size();
                    quad_end = end - (end % 4);
                    Operand oa = { band->elements() + quad_start};
                    Operand ob = { b.elements() + quad_start - op_offset - ((4 - (op_offset % 4)) % 4)};
                    Operand oc = { result.elements() + quad_start};
                    Operand od, oe, of, og;

                    od.u = (quad_end - quad_start) / (1000 * 4);
                    oe.u = (quad_end - quad_start) % (1000 * 4);
                    if (0 == oe.u)
                    {
                        if (od.u > 0)
                        {
                            oe.u = 1000 * 4;
                        }
                    }
                    else
                    {
                        ++od.u;
                    }

                    og.u = (4 - (op_offset % 4)) % 4;
                    if(quad_end > quad_start)
                    {
                        if (iq_upper.empty() || iq_upper.back()->size() == 7)
                        {
                            iq_upper.push_back(new SPEInstructionQueue);
                            iq_upper.back()->push_back(SPEInstruction(oc_product_banded_matrix_dense_vector_float, 1000 * 4, oa, ob, oc, od, oe, of, og));
                        }
                        else
                        {
                            iq_upper.back()->push_back(SPEInstruction(oc_product_banded_matrix_dense_vector_float, 1000 * 4, oa, ob, oc, od, oe, of, og));
                        }
                    }
                    else
                    {
                        quad_start = 0;
                        quad_end = start;
                    }

                    for (unsigned long index = start ; index < quad_start ; index++)
                    {
                        result.elements()[index] += band->elements()[index] * b.elements()[index - op_offset];
                    }
                    for (unsigned long index = quad_end ; index < end ; index++)
                    {
                        result.elements()[index] += band->elements()[index] * b.elements()[index - op_offset];
                    }
                }
            }
        }
        /*as2.take();
        std::cout<<"assembly: "<<as2.sec() - as1.sec() << " "<<as2.usec() - as1.usec()<<std::endl;
        TimeStamp at, bt;
        at.take();*/
        PROFILER_START("Product<Cell>::value(bm, dv)->dispatch");
        for (std::list<SPEInstructionQueue *>::iterator i(iq_lower.begin()), i_end(iq_lower.end()), j(iq_upper.begin()), j_end(iq_upper.end()) ;
                (i != i_end || j != j_end) ; )
        {
            if (i != i_end) SPEManager::instance()->dispatch(*(*i));
            if (j != j_end) SPEManager::instance()->dispatch(*(*j));
            /// \todo calc scalar parts here
            if (i != i_end) (*i)->wait();
            if (j != j_end) (*j)->wait();
            if (i != i_end) ++i;
            if (j != j_end) ++j;
        }
        for (std::list<SPEInstructionQueue *>::iterator i(iq_lower.begin()), i_end(iq_lower.end()), j(iq_upper.begin()), j_end(iq_upper.end()) ;
                (i != i_end || j != j_end) ; )
        {
            if (i != i_end) delete *i;
            if (j != j_end) delete *j;
            if (i != i_end) ++i;
            if (j != j_end) ++j;
        }
        //bt.take();
        //std::cout<<"wait: "<<bt.sec() - at.sec() << " "<<bt.usec() - at.usec()<<std::endl<<std::endl;
        PROFILER_STOP("Product<Cell>::value(bm, dv)->dispatch");
        PROFILER_STOP("Product<Cell>::value(bm, dv)");

        return result;
    }

    DenseVector<double> Product<tags::Cell>::value(const BandedMatrix<double> & a, const DenseVector<double> & b)
    {
        CONTEXT("When calculating BandedMatrix<double>-DenseVector<double> product (Cell):");

        if (b.size() != a.columns())
            throw VectorSizeDoesNotMatch(b.size(), a.columns());

        /// \todo Remove SPEIQ list when one SPEInstructionQueue can handle more than 8 instructions.
        std::list<SPEInstructionQueue *> iq_upper, iq_lower;
        //TimeStamp dt,ct, as1, as2;
        //ct.take();
        DenseVector<double> result(b.size(), 0.0f);
        //dt.take();
        //std::cout<<"dv(0): "<<dt.sec() - ct.sec() << " "<<dt.usec() - ct.usec()<<std::endl;
        /// \todo Fill the result vector on the spu side.
        //DenseVector<double> result(b.size());
        //as1.take();

        unsigned long middle_index(a.rows() - 1);
        unsigned long quad_end, end, quad_start, start, x_offset, op_offset;
        for (BandedMatrix<double>::ConstVectorIterator band(a.begin_non_zero_bands()), band_end(a.end_non_zero_bands()) ;
                band != band_end ; ++band)
        {
            // If we are above or on the diagonal band, we start at Element 0 and go on until Element band_size-band_index.
            if (band.index() >= middle_index)
            {
                {
                    // Lower result part
                    op_offset = band.index() - middle_index;
                    start = 0;
                    quad_start = 0;
                    end = std::min(band->size() - op_offset, result.size() / 2); //Calculation of the element-index to stop in iteration!
                    quad_end = end - (end % 2);

                    Operand oa = { band->elements() + quad_start };
                    Operand ob = { b.elements() + quad_start + op_offset - (op_offset % 2) };
                    Operand oc = { result.elements() + quad_start };
                    Operand od, oe, of, og;
                    /// \todo use such a transfer size, that od.u is at least 2
                    od.u = (quad_end - quad_start) / (1000 * 2);
                    oe.u = (quad_end - quad_start) % (1000 * 2);
                    if (0 == oe.u)
                    {
                        if (od.u > 0)
                        {
                            oe.u = 1000 * 2;
                        }
                    }
                    else
                    {
                        ++od.u;
                    }

                    og.u = op_offset % 2;
                    if(quad_end > quad_start)
                    {
                        if (iq_lower.empty() || iq_lower.back()->size() == 7)
                        {
                            iq_lower.push_back(new SPEInstructionQueue);
                            iq_lower.back()->push_back(SPEInstruction(oc_product_banded_matrix_dense_vector_double, 1000 * 2, oa, ob, oc, od, oe, of, og));
                        }
                        else
                        {
                            iq_lower.back()->push_back(SPEInstruction(oc_product_banded_matrix_dense_vector_double, 1000 * 2, oa, ob, oc, od, oe, of, og));
                        }
                    }
                    else
                    {
                        quad_start = 0;
                        quad_end = 0;
                    }

                    for (unsigned long index = quad_end ; index < end ; index++)
                    {
                        result.elements()[index] += band->elements()[index] * b.elements()[index + op_offset];
                    }
                }

                {
                    // Upper result part
                    op_offset = band.index() - middle_index;
                    start = result.size() / 2;
                    quad_start = start + ((2 - (start % 2)) % 2);
                    end = band->size() - op_offset; //Calculation of the element-index to stop in iteration!
                    quad_end = end - (end % 2);

                    Operand oa = { band->elements() + quad_start };
                    Operand ob = { b.elements() + quad_start + op_offset - (op_offset % 2) };
                    Operand oc = { result.elements() + quad_start };
                    Operand od, oe, of, og, oh;

                    /// \todo use such a transfer size, that od.u is at least 2
                    od.u = (quad_end - quad_start) / (1000 * 2);
                    oe.u = (quad_end - quad_start) % (1000 * 2);
                    if (0 == oe.u)
                    {
                        if (od.u > 0)
                        {
                            oe.u = 1000 * 2;
                        }
                    }
                    else
                    {
                        ++od.u;
                    }

                    og.u = op_offset % 2;
                    if(quad_end > quad_start)
                    {
                        if (iq_upper.empty() || iq_upper.back()->size() == 7)
                        {
                            iq_upper.push_back(new SPEInstructionQueue);
                            iq_upper.back()->push_back(SPEInstruction(oc_product_banded_matrix_dense_vector_double, 1000 * 2, oa, ob, oc, od, oe, of, og));
                        }
                        else
                        {
                            iq_upper.back()->push_back(SPEInstruction(oc_product_banded_matrix_dense_vector_double, 1000 * 2, oa, ob, oc, od, oe, of, og));
                        }
                    }
                    else
                    {
                        quad_start = result.size() / 2;
                        quad_end = result.size() / 2;
                    }

                    for (unsigned long index = start ; index < quad_start ; index++)
                    {
                        result.elements()[index] += band->elements()[index] * b.elements()[index + op_offset];
                    }
                    for (unsigned long index = quad_end ; index < end ; index++)
                    {
                        result.elements()[index] += band->elements()[index] * b.elements()[index + op_offset];
                    }
                }
            }

            // If we are below the diagonal band, we start at Element 'start' and go on until the last element.
            else
            {
                {
                    // Lower result part
                    op_offset = middle_index - band.index();
                    start = op_offset; //Calculation of the element-index to start in iteration!
                    quad_start = start + ((2 - (start % 2)) % 2);
                    end = result.size() / 2;
                    quad_end = end - (end % 2);
                    Operand oa = { band->elements() + quad_start};
                    Operand ob = { b.elements() + quad_start - op_offset - ((2 - (op_offset % 2)) % 2)};
                    Operand oc = { result.elements() + quad_start};
                    Operand od, oe, of, og;

                    od.u = (quad_end - quad_start) / (1000 * 2);
                    oe.u = (quad_end - quad_start) % (1000 * 2);
                    if (0 == oe.u)
                    {
                        if (od.u > 0)
                        {
                            oe.u = 1000 * 2;
                        }
                    }
                    else
                    {
                        ++od.u;
                    }

                    og.u = (2 - (op_offset % 2)) % 2;
                    if(quad_end > quad_start)
                    {
                        if (iq_lower.empty() || iq_lower.back()->size() == 7)
                        {
                            iq_lower.push_back(new SPEInstructionQueue);
                            iq_lower.back()->push_back(SPEInstruction(oc_product_banded_matrix_dense_vector_double, 1000 * 2, oa, ob, oc, od, oe, of, og));
                        }
                        else
                        {
                            iq_lower.back()->push_back(SPEInstruction(oc_product_banded_matrix_dense_vector_double, 1000 * 2, oa, ob, oc, od, oe, of, og));
                        }
                    }
                    else
                    {
                        quad_start = 0;
                        quad_end = start;
                    }

                    for (unsigned long index = start ; index < quad_start ; index++)
                    {
                        result.elements()[index] += band->elements()[index] * b.elements()[index - op_offset];
                    }
                    for (unsigned long index = quad_end ; index < end ; index++)
                    {
                        result.elements()[index] += band->elements()[index] * b.elements()[index - op_offset];
                    }
                }

                {
                    // Upper result part
                    op_offset = middle_index - band.index();
                    start = std::max(op_offset, result.size() / 2); //Calculation of the element-index to start in iteration!
                    quad_start = start + ((2 - (start % 2)) % 2);
                    end = band->size();
                    quad_end = end - (end % 2);
                    Operand oa = { band->elements() + quad_start};
                    Operand ob = { b.elements() + quad_start - op_offset - ((2 - (op_offset % 2)) % 2)};
                    Operand oc = { result.elements() + quad_start};
                    Operand od, oe, of, og;

                    od.u = (quad_end - quad_start) / (1000 * 2);
                    oe.u = (quad_end - quad_start) % (1000 * 2);
                    if (0 == oe.u)
                    {
                        if (od.u > 0)
                        {
                            oe.u = 1000 * 2;
                        }
                    }
                    else
                    {
                        ++od.u;
                    }

                    og.u = (2 - (op_offset % 2)) % 2;
                    if(quad_end > quad_start)
                    {
                        if (iq_upper.empty() || iq_upper.back()->size() == 7)
                        {
                            iq_upper.push_back(new SPEInstructionQueue);
                            iq_upper.back()->push_back(SPEInstruction(oc_product_banded_matrix_dense_vector_double, 1000 * 2, oa, ob, oc, od, oe, of, og));
                        }
                        else
                        {
                            iq_upper.back()->push_back(SPEInstruction(oc_product_banded_matrix_dense_vector_double, 1000 * 2, oa, ob, oc, od, oe, of, og));
                        }
                    }
                    else
                    {
                        quad_start = 0;
                        quad_end = start;
                    }

                    for (unsigned long index = start ; index < quad_start ; index++)
                    {
                        result.elements()[index] += band->elements()[index] * b.elements()[index - op_offset];
                    }
                    for (unsigned long index = quad_end ; index < end ; index++)
                    {
                        result.elements()[index] += band->elements()[index] * b.elements()[index - op_offset];
                    }
                }
            }
        }
        /*as2.take();
        std::cout<<"assembly: "<<as2.sec() - as1.sec() << " "<<as2.usec() - as1.usec()<<std::endl;
        TimeStamp at, bt;
        at.take();*/
        for (std::list<SPEInstructionQueue *>::iterator i(iq_lower.begin()), i_end(iq_lower.end()), j(iq_upper.begin()), j_end(iq_upper.end()) ;
                (i != i_end || j != j_end) ; )
        {
            if (i != i_end) SPEManager::instance()->dispatch(*(*i));
            if (j != j_end) SPEManager::instance()->dispatch(*(*j));
            /// \todo calc scalar parts here
            if (i != i_end) (*i)->wait();
            if (j != j_end) (*j)->wait();
            if (i != i_end) ++i;
            if (j != j_end) ++j;
        }
        for (std::list<SPEInstructionQueue *>::iterator i(iq_lower.begin()), i_end(iq_lower.end()), j(iq_upper.begin()), j_end(iq_upper.end()) ;
                (i != i_end || j != j_end) ; )
        {
            if (i != i_end) delete *i;
            if (j != j_end) delete *j;
            if (i != i_end) ++i;
            if (j != j_end) ++j;
        }
        //bt.take();
        //std::cout<<"wait: "<<bt.sec() - at.sec() << " "<<bt.usec() - at.usec()<<std::endl<<std::endl;

        return result;
    }

    DenseMatrix<float> Product<tags::Cell>::value(const DenseMatrix<float> & a, const DenseMatrix<float> & b)
    {
        CONTEXT("When calculating DenseMatrix<float>-DenseMatrix<float> product (Cell):");

        if (a.columns() != b.rows())
            throw MatrixRowsDoNotMatch(b.rows(), a.columns());

        /* \\\todo: Tiling. */

        /* We assume to have complete matrices or complete matrix tiles
         * at this point. So later here will be another for-loop that calls the
         * spe program for every pair (triple with result) of tiles.
         * This for loop will be responsible for transfering the logically
         * correct pairs of tiles to the spe.
         * The spe program itself can then compute the same way, as if it would
         * do, if it got complete matrices as operands.
         */

        unsigned a_tile_rows;
        unsigned a_tile_columns;
        unsigned b_tile_rows;
        unsigned b_tile_columns;
        unsigned r_tile_rows;
        unsigned r_tile_columns;

        // Beginning of double buffering.

        /* For the first source Matrix (a) and the result Matrix (r) we apply double buffering,
         * while we always transfer the whole second source Matrix (b).
         * For a and r we always pull as many complete rows that fit into 16 KB.
         */

        a_tile_rows = a.rows(); // Later use tiles' rows here
        a_tile_columns = a.columns(); // Later use tiles' columns here
        b_tile_rows = b.rows(); // Later user tiles' rows here
        b_tile_columns = b.columns(); // Later use tiles' columns here
        DenseMatrix<float> result(a_tile_rows, b_tile_columns, 0.0f);

        r_tile_rows = result.rows(); // Later use tiles' rows here
        r_tile_columns = result.columns(); // Later use tiles' columns here.

        Operand oa = { a.elements() }; // Later use tiles' elements here
        Operand ob = { b.elements() }; // Later use tiles' elements here
        Operand oc = { result.elements() }; // Later use tiles' elements here
        Operand od, oe;
        od.u = a.columns(); // Later use tiles' columns here
        oe.u = b.columns(); // Later use tiles' columns here

        // We now need to know how much of the tiles we can transfer at most
        // using one dma transfer.
        // For the tile of a the number of columns decides, because we can then see
        // how many rows we can transfer within 16 KB. We do the same for the tile of r.
        // There is only one problem. Because r can have less or more columns as a, we
        // can have different default transfer sizes for them. But we need exactly the
        // same number of rows of a and r to let the spe-program work correctly.
        // So we need a # of rows, for that both transfersizes are % 16 == 0.

        Operand of; // Will transfer the number of full transfers for a.
        Operand og; // Will transfer the number of blocks to reserve for b.
        Operand oh; // Will transfer the size of a last transfer for a,
        Operand oi; // Will transfer the size of a last transfer for b.
        Operand oj; // Will transfer the size of a last transfer for r.
        Operand ok; // Will transfer the default transfer size for r.

        unsigned a_row_bytes(a_tile_columns * 4); // Bytes per row
        unsigned a_rows_per_transfer(16384 / a_row_bytes);

        unsigned r_row_bytes(r_tile_columns * 4);
        unsigned r_rows_per_transfer(16384 / r_row_bytes);

        // r can have more or less columns than a.
        // We must start with the bigger one to assure that we don't
        // exceed 16 KB.
        unsigned rows_per_transfer;
        a_tile_columns > r_tile_columns ? rows_per_transfer = a_rows_per_transfer : rows_per_transfer = r_rows_per_transfer;
        unsigned r_def_t_size(rows_per_transfer * r_row_bytes);
        unsigned a_def_t_size(rows_per_transfer * a_row_bytes);

        for ( ; ; ) // Need two default aligned transfer size as closest to 16384 as possible.
        {
            if (a_def_t_size % 16 == 0 && r_def_t_size % 16 == 0)
            {
                break;
            }
            a_def_t_size -= a_row_bytes;
            r_def_t_size -= r_row_bytes;
            rows_per_transfer--;
            if (a_def_t_size <= 0 || r_def_t_size <= 0)
            {
                std::cout << "Could not find an alignment" << std::endl;
            }
        }

        unsigned b_bytes(b_tile_rows * b_tile_columns * 4);
        of.u = a_tile_rows / rows_per_transfer;
        og.u = b_bytes / 16384;
        oh.u = (a_tile_rows % rows_per_transfer) * a_row_bytes;
        oi.u = b_bytes % 16384;
        oj.u = (r_tile_rows % rows_per_transfer) * r_row_bytes;
        ok.u = r_def_t_size;

        // If there is a rest for a and r to transfer, we need one more transfer...
        // Else if there is no rest, the last transfer size is equal to the normal one.
        if (0 != oh.u || 0 != oj.u)
        {
            of.u++;
        }
        else
        {
            oh.u = a_def_t_size;
            oj.u = r_def_t_size;
        }

        // If a fits into one transfer, we should say that it is one transfer needed and set last size.
        if (0 == of.u)
        {
            of.u++;
            oh.u = a_row_bytes * a_tile_columns;
        }

        if (oi.u != 0) // if we need one more block for rest rows of b.
        {
            og.u++;
        }

        SPEInstruction instruction(oc_product_dense_matrix_dense_matrix_float, a_def_t_size, oa, ob, oc, od, oe, of, og, oh, oi, oj, ok);

        SPEManager::instance()->dispatch(instruction);

        instruction.wait();

        return result;
    }

    DenseMatrix<float>
        Product<tags::Cell>::value(const SparseMatrix<float> & a, const DenseMatrix<float> & b)
        {
            CONTEXT("When calculating SparseMatrix<float>-DenseMatrix<float> product (Cell):");

            if (a.columns() != b.rows())
                throw MatrixRowsDoNotMatch(b.rows(), a.columns());

            DenseMatrix<float> result(a.rows(), b.columns(), 0.0f);

            for(SparseMatrix<float>::ConstElementIterator i(a.begin_non_zero_elements()), i_end(a.end_non_zero_elements()) ; i != i_end ; ++i)
            {
                ScaledSum<tags::Cell>::value(result[i.row()], b[i.column()], *i);
            }

            return result;
        }
}
