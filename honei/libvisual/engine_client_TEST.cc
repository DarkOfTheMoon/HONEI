/* vim: set sw=4 sts=4 et foldmethod=syntax : */
/*
 * Copyright (c) 2008 Dirk Ribbrock <dirk.ribbrock@uni-dortmund.de>
 *
 * This file is part of the LibVisual C++ library. LibVisual is free software;
 * you can redistribute it and/or modify it under the terms of the GNU General
 * Public License version 2, as published by the Free Software Foundation.
 *
 * LibVisual is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <honei/libvisual/engine_client.hh>
#include <unittest/unittest.hh>
#include <honei/libutil/stringify.hh>
#include <string>


using namespace honei;
using namespace tests;

template <typename Tag_, typename DataType_>
class EngineClientTest :
    public BaseTest
{
    public:
        EngineClientTest(const std::string & type) :
            BaseTest("EngineClient test<" + type + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            DenseMatrix<double> hf(4, 4);
            EngineClient<Tag_, DataType_> engine_client;

            engine_client.init("localhost", 4711, 12345);
            engine_client.do_step(hf);
            engine_client.quit_server();

            engine_client.init("localhost", 4711, 12345);
            engine_client.do_step(hf);
            engine_client.do_step(hf);
            engine_client.do_step(hf);
            engine_client.do_step(hf);
            engine_client.do_step(hf);
            engine_client.do_step(hf);
            engine_client.restart_scenario();
            engine_client.do_step(hf);
            engine_client.do_step(hf);
            engine_client.quit_server();

            engine_client.init("localhost", 4711, 12345);
            engine_client.do_step(hf);
            engine_client.do_step(hf);
            engine_client.shutdown_server();
            TEST_CHECK(true);
        }
};
//EngineClientTest<tags::CPU, float> engine_client_test_double("float");