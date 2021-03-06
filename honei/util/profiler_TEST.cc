/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2008 Danny van Dyk <danny.dyk@uni-dortmund.de>
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

// This test case needs profiler support.
#ifndef HONEI_PROFILER
#define HONEI_PROFILER 1
#endif

#include <honei/util/condition_variable.hh>
#include <honei/util/lock.hh>
#include <honei/util/mutex.hh>
#include <honei/util/profiler.hh>
#include <honei/util/unittest.hh>

#include <cmath>
#include <honei/util/tr1_boost.hh>

using namespace honei;
using namespace tests;

namespace
{
    void bar()
    {
        PROFILER_START("this-should-take-longer");

        for (unsigned i(0) ; i < 10000 ; ++i)
        {
            float f(i * 1.23456789f / 100);
            f = sqrt(f);
        }

        PROFILER_START("this-is-a-test-tag");

        for (unsigned i(0) ; i < 10000 ; ++i)
        {
            float f(i * 1.23456789f / 1000);
            f = 1 / f;
        }

        PROFILER_STOP("this-is-a-test-tag");

        for (unsigned i(0) ; i < 10000 ; ++i)
        {
            float f(i * 1.23456789f / 10);
            f = fabs(f);
        }

        PROFILER_STOP("this-should-take-longer");
    }

    void foo()
    {
        PROFILER_START("");
        bar();
        PROFILER_STOP("");
    }
}

class ProfilerTest :
    public QuickTest
{
    private:
        shared_ptr<Mutex> _mutex;

        shared_ptr<ConditionVariable> _done;

        bool _complete;

        bool _passed;

    public:
        ProfilerTest() :
            QuickTest("profiler_test"),
            _mutex(new Mutex),
            _done(new ConditionVariable),
            _complete(false),
            _passed(false)
        {
            ProfilerMessage(ProfilerMessage::EvaluationFunction(bind(
                            mem_fn(&ProfilerTest::evaluation), this,
                            HONEI_PLACEHOLDERS_1, HONEI_PLACEHOLDERS_2,
                            HONEI_PLACEHOLDERS_3, HONEI_PLACEHOLDERS_4,
                            HONEI_PLACEHOLDERS_5, HONEI_PLACEHOLDERS_6,
                            HONEI_PLACEHOLDERS_7, HONEI_PLACEHOLDERS_8)));
        }

        void evaluation(const std::string & function, const std::string & tag, unsigned count, float highest, float average, float /*median*/, float lowest, float /*total*/)
        {
            static unsigned calls(0);

            _passed = (highest >= average);
            _passed &= (average >= lowest);

            if ("bar" == function)
            {
                _passed &= (30 == count);
                _passed &= (("this-should-take-longer" == tag) || ("this-is-a-test-tag" == tag));
            }
            else if ("foo" == function)
            {
                _passed &= ("" == tag);
                _passed &= (10 == count);
            }
            else
            {
                _passed = false;
            }

            ++calls;

            if (3 == calls)
            {
                Lock l(*_mutex);
                _complete = true;
                _done->signal();
            }
        }

        virtual void run() const
        {
            for (unsigned i(0) ; i < 10 ; ++i)
            {
                bar();
                foo();
                bar();
            }

            PROFILER_EVALUATE;

            {
                Lock l(*_mutex);
                if (! _complete)
                {
                    _done->wait(*_mutex);
                }
            }

            TEST_CHECK(_passed);
        }
} profiler_test;
