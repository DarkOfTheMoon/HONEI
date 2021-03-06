/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2007 Danny van Dyk <danny.dyk@uni-dortmund.de>
 *
 * This file is part of the Utility C++ library. LibUtil is free software;
 * you can redistribute it and/or modify it under the terms of the GNU General
 * Public License version 2, as published by the Free Software Foundation.
 *
 * LibUtil is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA  02111-1307  USA
 */

#pragma once
#ifndef LIBUTIL_GUARD_ASSERTION_HH
#define LIBUTIL_GUARD_ASSERTION_HH 1

#include <honei/util/exception.hh>

#include <string>

void external_assertion(bool is_ok, const char * const function, const char * const file,
        const long line, const std::string & message);

namespace honei
{
    /**
     * Assertion is thrown when a critical condition is not fulfilled.
     *
     * \ingroup grpdebug
     */
    class Assertion :
        public Exception
    {
        public:
            /**
             * Constructor.
             *
             * \param function Name of the function in which the assertion failed.
             * \param file Name of the source file that contains the failed assertion.
             * \param line Line number of the failed assertion.
             * \param message Message that shall be displayed.
             */
            Assertion(const char * const function, const char * const file,
                    const long line, const std::string & message);
    };


/**
 * \def ASSERT
 *
 * \brief Convenience definition that provides a way to throw Assertion exceptions.
 *
 * The thrown Assertion will be automatically provided with the correct filename,
 * line number and function name.
 *
 * \param expr Boolean expression that shall be asserted.
 * \param msg Error message that will be display in case that expr evaluates to false.
 *
 * \warning Will only be compiled in when debug support is enabled.
 *
 * \ingroup grpdebug
 */
#if defined (DEBUG)
#define ASSERT(expr, msg) \
    do { \
        if (! (expr)) \
            throw Assertion(__PRETTY_FUNCTION__, __FILE__, __LINE__, msg); \
    } while (false)
#else
#define ASSERT(expr, msg)
#endif

#if defined (DEBUG)
#define EXTERNAL_ASSERT(expr, msg) \
    do { \
        external_assertion(expr, __PRETTY_FUNCTION__, __FILE__, __LINE__, msg); \
    } while (false)
#else
#define EXTERNAL_ASSERT(expr, msg)
#endif
}

#endif
