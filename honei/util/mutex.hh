/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2007, 2008 Danny van Dyk <danny.dyk@uni-dortmund.de>
 *
 * Based upon 'mutex.hh' from Paludis, which is:
 *     Copyright (c) 2007 Ciaran McCreesh
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
#ifndef LIBUTIL_GUARD_MUTEX_HH
#define LIBUTIL_GUARD_MUTEX_HH 1

#include <honei/util/instantiation_policy.hh>

#include <pthread.h>

namespace honei
{
    class Mutex :
        public InstantiationPolicy<Mutex, NonCopyable>
    {
        private:
            /// Our attributes.
            pthread_mutexattr_t * const _attr;

            /// Our pthread mutex.
            pthread_mutex_t * const _mutex;

        public:
            /// (Explicit) constructor.
            explicit Mutex();

            /// Destructor.
            ~Mutex();

            /// Returns a pointer to our underlying posix mutex.
            pthread_mutex_t * mutex()
            {
                return _mutex;
            }
    };
}

#endif
