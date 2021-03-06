/* vim: set sw=4 sts=4 et nofoldenable : */

/*
 * Copyright (c) 2007 Volker Jung <volker.jung@uni-dortmund.de>
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

#pragma once
#ifndef LIBLA_GUARD_DENSE_VECTOR_RANGE_IMPL_HH
#define LIBLA_GUARD_DENSE_VECTOR_RANGE_IMPL_HH 1

#include <honei/la/dense_vector.hh>
#include <honei/la/dense_vector.hh>
#include <honei/la/dense_vector_range.hh>
#include <honei/la/vector_error.hh>
#include <honei/la/element_iterator.hh>
#include <honei/util/assertion.hh>
#include <honei/util/private_implementation_pattern-impl.hh>
#include <honei/util/shared_array-impl.hh>
#include <honei/util/stringify.hh>

#include <algorithm>
#include <string>
#include <limits>
#include <cmath>

namespace honei
{
    /**
     * \brief DenseVectorRange::Implementation is the private implementation class for DenseVectorRange.
     *
     * \ingroup grpvector
     */
    template <typename DataType_> struct Implementation<DenseVectorRange<DataType_> >
    {
        /// Our elements.
        SharedArray<DataType_> elements;

        /// Our size.
        const unsigned long size;

        /// Our offset.
        const unsigned long offset;

        /// Our stepsize.
        const unsigned long stepsize;

        /// Constructor.
        Implementation(const SharedArray<DataType_> & e, unsigned long s, unsigned long o) :
            elements(e),
            size(s),
            offset(o),
            stepsize(1)
        {
        }
    };

    template <typename DataType_>
    DenseVectorRange<DataType_>::DenseVectorRange(const DenseVector<DataType_> & source, const unsigned long size,
            const unsigned long offset) :
        PrivateImplementationPattern<DenseVectorRange<DataType_>, Shared>(new Implementation<DenseVectorRange<DataType_> >(
                    source.array(), size, offset))
    {
        CONTEXT("When creating DenseVectorRange:");
        ASSERT(size > 0, "size is zero!");
        ASSERT(size <= source.size(), "size of range is bigger than size of source!");
        ASSERT(offset <= source.size(), "offset is out of bounds!");
        ASSERT(offset + size <= source.size(), "end of range is out of bounds!");
    }

    template <typename DataType_>
    DenseVectorRange<DataType_>::DenseVectorRange(const SharedArray<DataType_> & e, const unsigned long size,
            const unsigned long offset) :
        PrivateImplementationPattern<DenseVectorRange<DataType_>, Shared>(new Implementation<DenseVectorRange<DataType_> >(
                    e, size, offset))
    {
        CONTEXT("When creating DenseVectorRange:");
        ASSERT(size > 0, "size is zero!");
        ASSERT(size <= e.size(), "size of range is bigger than size of source!");
        ASSERT(offset <= e.size(), "offset is out of bounds!");
        ASSERT(offset + size <= e.size(), "end of range is out of bounds!");
    }

    template <typename DataType_>
    DenseVectorRange<DataType_>::DenseVectorRange(const DenseVectorRange<DataType_> & other) :
        DenseVectorContinuousBase<DataType_>(),
        PrivateImplementationPattern<DenseVectorRange<DataType_>, Shared>(new Implementation<DenseVectorRange<DataType_> >(
                    other._imp->elements, other._imp->size, other._imp->offset))
    {
    }

    template <typename DataType_>
    DenseVectorRange<DataType_>::DenseVectorRange(const DenseVectorRange<DataType_> & source, const unsigned long size,
            const unsigned long offset) :
        PrivateImplementationPattern<DenseVectorRange<DataType_>, Shared>(new Implementation<DenseVectorRange<DataType_> >(
                    source._imp->elements, size, offset + source._imp->offset))
    {
        CONTEXT("When creating DenseVectorRange:");
        ASSERT(size > 0, "size is zero!");
        ASSERT(size <= source._imp->size, "size of range is bigger than size of source!");
        ASSERT(offset <= source._imp->size, "offset is out of bounds!");
        ASSERT(offset + size <= source._imp->size, "end of range is out of bounds!");
    }

    template <typename DataType_>
    DenseVectorRange<DataType_>::~DenseVectorRange()
    {
    }

    template <typename DataType_>
    typename DenseVectorRange<DataType_>::ConstElementIterator
    DenseVectorRange<DataType_>::begin_elements() const
    {
        return ConstElementIterator(this->_imp->elements, 0, this->_imp->offset, 1);
    }

    template <typename DataType_>
    typename DenseVectorRange<DataType_>::ConstElementIterator
    DenseVectorRange<DataType_>::end_elements() const
    {
        return ConstElementIterator(this->_imp->elements, this->_imp->size, this->_imp->offset, 1);
    }

    template <typename DataType_>
    typename DenseVectorRange<DataType_>::ConstElementIterator
    DenseVectorRange<DataType_>::element_at(unsigned long index) const
    {
        return ConstElementIterator(this->_imp->elements, index, this->_imp->offset, 1);
    }

    template <typename DataType_>
    typename DenseVectorRange<DataType_>::ElementIterator
    DenseVectorRange<DataType_>::begin_elements()
    {
        return ElementIterator(this->_imp->elements, 0, this->_imp->offset, 1);
    }

    template <typename DataType_>
    typename DenseVectorRange<DataType_>::ElementIterator
    DenseVectorRange<DataType_>::end_elements()
    {
        return ElementIterator(this->_imp->elements, this->_imp->size, this->_imp->offset, 1);
    }

    template <typename DataType_>
    typename DenseVectorRange<DataType_>::ElementIterator
    DenseVectorRange<DataType_>::element_at(unsigned long index)
    {
        return ElementIterator(this->_imp->elements, index, this->_imp->offset, 1);
    }

    template <typename DataType_>
    unsigned long DenseVectorRange<DataType_>::size() const
    {
        return this->_imp->size;
    }

    template <typename DataType_>
    const DataType_ & DenseVectorRange<DataType_>::operator[] (unsigned long index) const
    {
        CONTEXT("When retrieving DenseVectorRange element, unassignable:");
        ASSERT(index < this->_imp->size, "index is out of bounds!");
        return this->_imp->elements[index + this->_imp->offset];
    }

    template <typename DataType_>
    DataType_ & DenseVectorRange<DataType_>::operator[] (unsigned long index)
    {
        CONTEXT("When retrieving DenseVectorRange element, assignable:");
        ASSERT(index < this->_imp->size, "index is out of bounds!");
        return this->_imp->elements[index + this->_imp->offset];
    }

    template <typename DataType_>
    unsigned long DenseVectorRange<DataType_>::offset() const
    {
        return this->_imp->offset;
    }

    template <typename DataType_>
    unsigned long DenseVectorRange<DataType_>::stepsize() const
    {
        return this->_imp->stepsize;
    }

    template <typename DataType_>
    DenseVectorRange<DataType_> DenseVectorRange<DataType_>::range(unsigned long size, unsigned long offset) const
    {
        DenseVectorRange<DataType_> result(*this, size, offset);
        return result;
    }

    template <typename DataType_>
    inline DataType_ * DenseVectorRange<DataType_>::elements() const
    {
        return this->_imp->elements.get() + this->_imp->offset;
    }

    template <typename DataType_>
    inline SharedArray<DataType_> & DenseVectorRange<DataType_>::array() const
    {
        return this->_imp->elements;
    }

    template <typename DataType_>
    inline void * DenseVectorRange<DataType_>::memid() const
    {
        return this->_imp->elements.get();
    }

    template <typename DataType_>
    inline void * DenseVectorRange<DataType_>::address() const
    {
        return this->_imp->elements.get() + this->_imp->offset;
    }

    template <typename DataType_>
    void * DenseVectorRange<DataType_>::lock(LockMode mode, tags::TagValue memory) const
    {
        return MemoryArbiter::instance()->lock(mode, memory, this->memid(), this->address(), this->size() * sizeof(DataType_));
    }

    template <typename DataType_>
    void DenseVectorRange<DataType_>::unlock(LockMode mode) const
    {
        MemoryArbiter::instance()->unlock(mode, this->memid());
    }

    template <typename DataType_>
    DenseVector<DataType_> DenseVectorRange<DataType_>::copy() const
    {
        DenseVector<DataType_> result(this->_imp->size);
        this->lock(lm_read_only);

        DataType_ * source(this->_imp->elements.get());
        DataType_ * target(result.elements());
        for (unsigned long i(0) ; i < this->_imp->size ; i++)
        {
            target[i] = source[i + this->_imp->offset];
        }

        /// \todo: Use TypeTraits<DataType_>::copy()

        this->unlock(lm_read_only);
        return result;
    }

    template <typename DataType_>
    bool
    operator== (const DenseVectorRange<DataType_> & a, const DenseVectorRange<DataType_> & b)
    {
        if (a.size() != b.size())
            throw VectorSizeDoesNotMatch(a.size(), b.size());

        a.lock(lm_read_only);
        b.lock(lm_read_only);
        for (typename DenseVectorRange<DataType_>::ConstElementIterator i(a.begin_elements()), i_end(a.end_elements()),
                j(b.begin_elements()) ; i != i_end ; ++i, ++j)
        {
            if (*i != *i || *j != *j)
            {
                a.unlock(lm_read_only);
                b.unlock(lm_read_only);
                return false;
            }
            if (std::abs(*i - *j) > std::numeric_limits<DataType_>::epsilon())
            {
                a.unlock(lm_read_only);
                b.unlock(lm_read_only);
                return false;
            }
        }
        a.unlock(lm_read_only);
        b.unlock(lm_read_only);

        return true;
    }

    template <>
    bool
    operator== (const DenseVectorRange<unsigned long> & a, const DenseVectorRange<unsigned long> & b)
    {
        if (a.size() != b.size())
            throw VectorSizeDoesNotMatch(a.size(), b.size());

        a.lock(lm_read_only);
        b.lock(lm_read_only);
        for (DenseVectorRange<unsigned long>::ConstElementIterator i(a.begin_elements()), i_end(a.end_elements()),
                j(b.begin_elements()) ; i != i_end ; ++i, ++j)
        {
            if (*i != *i || *j != *j)
            {
                a.unlock(lm_read_only);
                b.unlock(lm_read_only);
                return false;
            }
            if (*i > *j && *i - *j > std::numeric_limits<unsigned long>::epsilon())
            {
                a.unlock(lm_read_only);
                b.unlock(lm_read_only);
                return false;
            }
            if (*j > *i && *j - *i > std::numeric_limits<unsigned long>::epsilon())
            {
                a.unlock(lm_read_only);
                b.unlock(lm_read_only);
                return false;
            }
        }
        a.unlock(lm_read_only);
        b.unlock(lm_read_only);

        return true;
    }

    template <typename DataType_>
    std::ostream &
    operator<< (std::ostream & lhs, const DenseVectorRange<DataType_> & b)
    {
        b.lock(lm_read_only);
        lhs << "[";
        for (typename DenseVectorRange<DataType_>::ConstElementIterator i(b.begin_elements()), i_end(b.end_elements()) ;
                i != i_end ; ++i)
        {
            lhs << "  " << *i;
        }
        lhs << "]" << std::endl;
        b.unlock(lm_read_only);

        return lhs;
    }
}

#endif
