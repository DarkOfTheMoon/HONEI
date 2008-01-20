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

#ifndef LIBUTIL_GUARD_THREAD_HH
#define LIBUTIL_GUARD_THREAD_HH 1

#include <tr1/functional>

namespace honei
{
    /**
     * Thread uses TR1's function wrapper to execute a given function in a
     * separate thread.
     *
     * Thread uses POSIX threads internally and allows minimal synchronisation.
     * Thread guarantees that its function is finalised after destruction.
     */
    class Thread
    {
        private:
            struct Implementation;

            /// Our implementation.
            Implementation * _imp;

            /// \name Unwanted operations.
            /// \{

            /// Unwanted copy-constructor: Do not implement. See EffCpp, Item 27.
            Thread(const Thread &);

            /// Unwanted assignment operator: Do not implement. See EffCpp, Item 27.
            Thread & operator= (const Thread &);

            /// \}

        public:
            /// Our function type.
            typedef std::tr1::function<void ()> Function;

            /// \name Constructor and destructor
            /// \{

            /**
             * Constructor.
             *
             * \param function A Function object that wraps the thread's main function.
             */
            Thread(const Function & function);

            /// Destructor.
            ~Thread();

            /// \}

            /// Return wether we have yet completed executing our function.
            bool completed() const;
    };
}

#endif