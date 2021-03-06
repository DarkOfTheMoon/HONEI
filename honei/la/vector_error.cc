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

#include <honei/la/vector_error.hh>
#include <honei/util/stringify.hh>

#include <string>

using namespace honei;

VectorError::VectorError(const std::string & message) throw () :
    Exception(message)
{
}

VectorSizeDoesNotMatch::VectorSizeDoesNotMatch(unsigned long size, unsigned long expected_size) throw () :
    VectorError("Vector size '" + stringify(size) + "' does not match expected size '"
            + stringify(expected_size) + "'")
{
}

VectorAlignmentDoesNotMatch::VectorAlignmentDoesNotMatch(unsigned long align, unsigned long expected_align) throw () :
    VectorError("Vector alignment '" + stringify(align) + "' does not match expected alignment '"
            + stringify(expected_align) + "'")
{
}
