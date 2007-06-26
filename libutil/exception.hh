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

#ifndef LIBUTIL_GUARD_EXCEPTION_HH
#define LIBUTIL_GUARD_EXCEPTION_HH 1

#include <string>

namespace pg512 ///< \todo Namespace name?
{
    /**
     * Backtrace class context.
     *
     * \ingroup grpexceptions
     * \nosubgrouping
     */
    class Context
    {
        private:
            /// Unwanted operations. Do not implement. See EffCpp, Item 27.
            /// \{
            Context(const Context &);
            const Context & operator= (const Context &);
            /// \}

            struct ContextData;
            ContextData * const _context_data;

        public:
            /**
             * Constructor.
             *
             * \param file Name of the source file that contains the context.
             * \param line Line number of the context.
             * \param context A sentence that describes this context.
             */
            Context(const char * const file, const long line, const std::string & context);

            /// Desctructor.
            ~Context();

            /**
             * Current context (forwards to libebt).
             */
            static std::string backtrace(const std::string & delimiter);
    };

#ifdef HAVE_DEBUG
#define CONTEXT(c) \
    Context c__LINE__(__FILE__, __LINE__, c)
#else
#define CONTEXT(c)
#endif

    /**
     * Base exception class.
     *
     * \ingroup grpexceptions
     * \nosubgrouping
     */
    class Exception :
        public std::exception
    {
        private:
            const std::string _message;
            mutable std::string _what_str;
            struct ContextData;
            ContextData * const _context_data;

            /// Unwanted operations. Do not implement. See EffCpp, Item 27.
            const Exception & operator= (const Exception &);

        protected:
            /**
             * Constructor.
             *
             * \param message The exception's message.
             */
            Exception(const std::string & message) throw ();

            /// Copy-constructor.
            Exception(const Exception & e);

        public:
            /// Destructor.
            virtual ~Exception() throw ();

            /// Return a descriptive error message.
            const std::string & message() const throw ();

            /// Return a backtrace.
            std::string backtrace(const std::string & delimiter) const;

            /// Return true if the backtrace is empty.
            bool empty() const;

            /// Return a descriptive exception name.
            const char * what() const throw ();
    };

    /**
     * InternalError is an Exception that is thrown if something that is
     * never supposed to happen happens.
     *
     * \ingroup grpexceptions
     * \nosubgrouping
     */
    class InternalError :
        public Exception
    {
        public:
            /**
             * Constructor.
             *
             * \param message A short error message.
             */
            InternalError(const std::string & message) throw ();
    };
}

#endif
