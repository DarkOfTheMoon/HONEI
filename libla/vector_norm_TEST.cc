/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2007 Danny van Dyk <danny.dyk@uni-dortmund.de>
 * Copyright (c) 2007 Dirk Ribbrock <dirk.ribbrock@uni-dortmund.de>
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
 
#include <libla/dense_vector.hh>
#include <libla/vector_norm.hh>
#include <unittest/unittest.hh>

#include <limits>
#include <tr1/memory>
#include <iostream>

using namespace pg512;
using  namespace tests;

template <typename DataType_>
class DenseVectorNormValueTest :
    public BaseTest
{
    public:
        DenseVectorNormValueTest(const std::string & type) :
            BaseTest("dense_vector_norm_value_test<" + type + ">")
        {
        }

        virtual void run() const
        {
            for (unsigned long size(1) ; size < (1 << 14) ; size <<= 1)
            {
                std::tr1::shared_ptr<DenseVector<DataType_> > dv(new DenseVector<DataType_>(size));
                for (typename Vector<DataType_>::ElementIterator i(dv->begin_elements()), i_end(dv->end_elements()) ;
                        i != i_end ; ++i)
                {
                    *i = static_cast<DataType_>((i.index() + 1) / 1.23456789);
                }

                DataType_ s(size);

                DataType_ vmax(VectorNorm<DataType_, vnt_max>::value(*dv));
                DataType_ smax(s / 1.23456789);
                TEST_CHECK_EQUAL_WITHIN_EPS(vmax, smax, std::numeric_limits<DataType_>::epsilon());

                DataType_ v1(VectorNorm<DataType_, vnt_l_one>::value(*dv));
                DataType_ s1(s * (s + 1) / 2 / 1.23456789);
                DataType_ eps1(s1 * 10 * std::numeric_limits<DataType_>::epsilon());
                TEST_CHECK_EQUAL_WITHIN_EPS(v1, s1, eps1);

                DataType_ v2(VectorNorm<DataType_, vnt_l_two, false>::value(*dv));
                DataType_ s2(s * (s + 1) * (2 * s + 1) / 6 / 1.23456789 / 1.23456789);
                DataType_ eps2(s2 * 20 * std::numeric_limits<DataType_>::epsilon());
                TEST_CHECK_EQUAL_WITHIN_EPS(v2, s2, eps2);
            }
        }
};

DenseVectorNormValueTest<float> dense_vector_norm_value_test_float("float");
DenseVectorNormValueTest<double> dense_vector_norm_value_test_double("double");

template <typename DataType_>
class DenseVectorNormQuickTest :
    public QuickTest
{
    public:
        DenseVectorNormQuickTest(const std::string & type) :
            QuickTest("dense_vector_norm_quick_test<" + type + ">")
        {
        }

        virtual void run() const
        {
            unsigned long size(5);
            std::tr1::shared_ptr<DenseVector<DataType_> > dv(new DenseVector<DataType_>(size));
            for (typename Vector<DataType_>::ElementIterator i(dv->begin_elements()), i_end(dv->end_elements()) ;
                    i != i_end ; ++i)
            {
                *i = static_cast<DataType_>((i.index() + 1) / 1.23456789);
            }

            DataType_ s(size);

            DataType_ vmax(VectorNorm<DataType_, vnt_max>::value(*dv));
            DataType_ smax(s / 1.23456789);
            TEST_CHECK_EQUAL_WITHIN_EPS(vmax, smax, std::numeric_limits<DataType_>::epsilon());

            DataType_ v1(VectorNorm<DataType_, vnt_l_one>::value(*dv));
            DataType_ s1(s * (s + 1) / 2 / 1.23456789);
            DataType_ eps1(s1 * 10 * std::numeric_limits<DataType_>::epsilon());
            TEST_CHECK_EQUAL_WITHIN_EPS(v1, s1, eps1);

            DataType_ v2(VectorNorm<DataType_, vnt_l_two, false>::value(*dv));
            DataType_ s2(s * (s + 1) * (2 * s + 1) / 6 / 1.23456789 / 1.23456789);
            DataType_ eps2(s2 * 20 * std::numeric_limits<DataType_>::epsilon());
            TEST_CHECK_EQUAL_WITHIN_EPS(v2, s2, eps2);
    }
};
DenseVectorNormQuickTest<float>  dense_vector_norm_quick_test_float("float");
DenseVectorNormQuickTest<double> dense_vector_norm_quick_test_double("double");
