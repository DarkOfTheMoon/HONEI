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

#include <honei/la/sparse_vector.hh>
#include <honei/la/sparse_vector-impl.hh>
#include <honei/util/unittest.hh>

#include <string>
#include <map>
#include <memory>

using namespace honei;
using namespace tests;

template <typename DataType_>
class SparseVectorCopyTest :
    public BaseTest
{
    public:
        SparseVectorCopyTest(const std::string & type) :
            BaseTest("sparse_vector_copy_test<" + type + ">")
        {
        }

        virtual void run() const
        {
            for (unsigned long size(20) ; size < (1 << 8) ; size <<= 1)
            {
                SparseVector<DataType_> sv1(size, size / 10);
                SparseVector<DataType_> c(sv1.copy());

                for (typename SparseVector<DataType_>::ElementIterator i(c.begin_elements()), i_end(c.end_elements()) ;
                        i != i_end ; ++i)
                {
                    typename SparseVector<DataType_>::ConstElementIterator ci(i);
                    TEST_CHECK_EQUAL_WITHIN_EPS(*ci, 0, std::numeric_limits<DataType_>::epsilon());

                    if (0 == (i.index() % 10))
                        *i = 1;
                }

                for (typename SparseVector<DataType_>::ConstElementIterator i(sv1.begin_elements()),
                        i_end(sv1.end_elements()) ; i != i_end ; ++i)
                {
                    TEST_CHECK_EQUAL_WITHIN_EPS(*i, 0, std::numeric_limits<DataType_>::epsilon());
                }
            }
        }
};
SparseVectorCopyTest<float> sparse_vector_copy_test_float("float");
SparseVectorCopyTest<double> sparse_vector_copy_test_double("double");

template <typename DataType_>
class SparseVectorCreationTest :
public BaseTest
{
public:
    SparseVectorCreationTest(const std::string & type) :
        BaseTest("sparse_vector_creation_test<" + type + ">")
    {
    }

    virtual void run() const
    {
        for (unsigned long size(10) ; size < (1 << 8) ; size <<= 1)
        {
            SparseVector<DataType_> sv1(size, size);
            SparseVector<DataType_> sv2(size, 1);
            TEST_CHECK(true);
        }
    }
};
SparseVectorCreationTest<float> sparse_vector_creation_test_float("float");
SparseVectorCreationTest<double> sparse_vector_creation_test_double("double");

template <typename DataType_>
class SparseVectorEqualityTest :
public BaseTest
{
public:
    SparseVectorEqualityTest(const std::string & type) :
        BaseTest("sparse_vector_equality_test<" + type + ">")
    {
    }

    virtual void run() const
    {
        for (unsigned long size(30) ; size < (1 << 8) ; size <<= 1)
        {
            SparseVector<DataType_> sv1(size, size);
            SparseVector<DataType_> sv2(size, size / 4 + 1);

            for (typename SparseVector<DataType_>::ElementIterator i(sv1.begin_elements()),
                    i_end(sv1.end_elements()) ; i != i_end ; ++i)
            {
                if (i.index() % 10 == 0)
                    *i = DataType_(1);
            }

            for (typename SparseVector<DataType_>::ElementIterator i(sv2.begin_elements()),
                    i_end(sv2.end_elements()) ; i != i_end ; ++i)
            {
                if (i.index() % 10 == 0)
                    *i = DataType_(1);
            }

            for (typename SparseVector<DataType_>::ConstElementIterator i(sv1.begin_elements()),
                    i_end(sv1.end_elements()), j(sv2.begin_elements()) ; i != i_end ;
                    ++i, ++j)
            {
                TEST_CHECK_EQUAL_WITHIN_EPS(*i, *j, std::numeric_limits<DataType_>::epsilon());
            }
        }
    }
};
SparseVectorEqualityTest<float> sparse_vector_equality_test_float("float");
SparseVectorEqualityTest<double> sparse_vector_equality_test_double("double");

template <typename DataType_>
class SparseVectorFunctionsTest :
public BaseTest
{
public:
    SparseVectorFunctionsTest(const std::string & type) :
        BaseTest("sparse_vector_functions_test<" + type + ">")
    {
    }

    virtual void run() const
    {
        for (unsigned long size(10) ; size < (1 << 8) ; size <<= 1)
        {
            SparseVector<DataType_> sv1(size, size / 3 + 1);

            for (typename SparseVector<DataType_>::ElementIterator i(sv1.begin_elements()),
                    i_end(sv1.end_elements()) ; i != i_end ; ++i)
            {
                if (i.index() % 10 == 0)
                    *i = DataType_(1);
            }

            TEST_CHECK_EQUAL(sv1.size(), size);
            TEST_CHECK_EQUAL(sv1.used_elements(), size / 10);
            // Do not test for capacity(), as it's an implementation detail.
        }
    }
};
SparseVectorFunctionsTest<float> sparse_vector_functions_test_float("float");
SparseVectorFunctionsTest<double> sparse_vector_functions_test_double("double");

template <typename DataType_>
class SparseVectorQuickTest :
public QuickTest
{
public:
    SparseVectorQuickTest(const std::string & type) :
        QuickTest("sparse_vector_quick_test<" + type + ">")
    {
    }

    virtual void run() const
    {
        unsigned long size(10);
        SparseVector<DataType_> sv1(size, size);
        SparseVector<DataType_> sv2(size, 3);

        sv1[0] = DataType_(3.3598);
        sv2[0] = DataType_(3.3598);

        for (typename SparseVector<DataType_>::ConstElementIterator i(sv1.begin_elements()),
                i_end(sv1.end_elements()), j(sv2.begin_elements()) ; i != i_end ;
                ++i, ++j)
        {
            TEST_CHECK_EQUAL_WITHIN_EPS(*i, *j, std::numeric_limits<DataType_>::epsilon());
        }

        TEST_CHECK_EQUAL(sv1.size(), size);
        TEST_CHECK_EQUAL(sv2.size(), size);
        TEST_CHECK_EQUAL(sv1.used_elements(), 1ul);
        TEST_CHECK_EQUAL(sv2.used_elements(), 1ul);
        TEST_CHECK_EQUAL(sv1, sv2);
        SparseVector<DataType_> sv4(size + 1, 1);
        TEST_CHECK_THROWS(sv1 == sv4, VectorSizeDoesNotMatch);
        // Do not test for capacity(), as it's an implementation detail.


        SparseVector<DataType_> sv3(size, size / 8 + 1);
        for (typename SparseVector<DataType_>::ElementIterator i(sv3.begin_elements()),
                i_end(sv3.end_elements()) ; i != i_end ; ++i)
        {
            if (i.index() % 4 == 0)
                *i = DataType_(5);
        }
        for (typename SparseVector<DataType_>::NonZeroElementIterator i(sv3.begin_non_zero_elements()),
                i_end(sv3.end_non_zero_elements()) ; i != i_end ; ++i)
        {
            TEST_CHECK_EQUAL(i.index() % 4, 0ul);
            TEST_CHECK_EQUAL(*i, DataType_(5));
        }

        SparseVector<DataType_> sv5(size, 2);
        sv5[0] = DataType_(1);
        sv5[5] = DataType_(2);
        sv5[9] = DataType_(3);
        DataType_ counter(1);
        for (typename SparseVector<DataType_>::NonZeroElementIterator i(sv5.begin_non_zero_elements()),
                i_end(sv5.end_non_zero_elements()) ; i != i_end ; ++i)
        {
            TEST_CHECK_EQUAL(*i, counter);
            if (*i == DataType_(2))
            {
                sv5[2] = DataType_(9);
            }
            TEST_CHECK_EQUAL(*i, counter);
            ++counter;
        }
        typename SparseVector<DataType_>::NonZeroConstElementIterator ati(sv5.non_zero_element_at(3));
        TEST_CHECK_EQUAL(*ati, DataType_(3));

        SparseVector<DataType_> sv6(size, 2);
        unsigned long prenz(sv6.used_elements());
        const DataType_ temp = ((const SparseVector<DataType_>)sv6)[2];
        TEST_CHECK_EQUAL(temp, DataType_(0));
        unsigned long postnz(sv6.used_elements());
        TEST_CHECK_EQUAL(postnz, prenz);
    }
};
SparseVectorQuickTest<float> sparse_vector_quick_test_float("float");
SparseVectorQuickTest<double> sparse_vector_quick_test_double("double");

template <typename DataType_>
class SparseVectorRandomAccessTest :
public BaseTest
{
public:
    SparseVectorRandomAccessTest(const std::string & type) :
        BaseTest("sparse_vector_random_access_test<" + type + ">")
    {
    }

    virtual void run() const
    {
        for (unsigned long size(10) ; size < (1 << 8) ; size <<= 1)
        {
            CONTEXT("When using size '" + stringify(size) + "':");

            std::map<unsigned long, DataType_> tests;
            SparseVector<DataType_> sv1(size, 1);
            SparseVector<DataType_> sv2(size, size / 3 + 1);

            for (unsigned long i(0) ; i < (size / 3 + 1) ; ++i)
            {
                CONTEXT("When inserting element '" + stringify(i + 1) + "':");

                unsigned long index(::rand() * size / RAND_MAX);
                DataType_ value(DataType_(::rand()) / 1.23456789);

                ASSERT(index < size, "index exceeds bounds!");

                tests[index] = value;
                sv1[index] = value;
                sv2[index] = value;
            }

            for (typename std::map<unsigned long, DataType_>::const_iterator i(tests.begin()),
                    i_end(tests.end()) ; i != i_end ; ++i)
            {
                TEST_CHECK_EQUAL_WITHIN_EPS(sv1[i->first], i->second, std::numeric_limits<DataType_>::epsilon());
                TEST_CHECK_EQUAL_WITHIN_EPS(sv2[i->first], i->second, std::numeric_limits<DataType_>::epsilon());
            }
        }
    }
};
SparseVectorRandomAccessTest<float> sparse_vector_random_access_test_float("float");
SparseVectorRandomAccessTest<double> sparse_vector_random_access_test_double("double");

template <typename DataType_>
class SparseVectorReallocTest :
    public BaseTest
{
    public:
        SparseVectorReallocTest(const std::string & type) :
            BaseTest("sparse_vector_realloc_test<" + type + ">")
        {
        }

        virtual void run() const
        {
            for (unsigned long size(10) ; size < (10 << 10) ; size <<= 1)
            {
                SparseVector<DataType_> sv1(size, size / 8 + 1), sv2(size, size / 9 + 1);
                for (typename SparseVector<DataType_>::ElementIterator i(sv1.begin_elements()), i_end(sv1.end_elements()) ;
                        i != i_end ; ++i)
                {
                    if (i.index() % 3 == 0)
                    {
                        *i = DataType_((i.index() + 1) / 0.987654321 * (i.index() % 2 == 1 ? -1 : 1));
                        // Keep this operator[] for testing purpose!
                        sv2[i.index()] = *i;
                    }
                }

                unsigned long c(0);
                for (typename SparseVector<DataType_>::NonZeroConstElementIterator i(sv1.begin_non_zero_elements()),
                    i_end(sv1.end_non_zero_elements()), j(sv2.begin_non_zero_elements()) ; i != i_end ; ++i, ++j, ++c)
                {
                    TEST_CHECK_EQUAL_WITHIN_EPS(*i, *j, std::numeric_limits<DataType_>::epsilon());

                    if (fabs(*i) <= std::numeric_limits<DataType_>::epsilon())
                        TEST_CHECK(false);
                }

                TEST_CHECK_EQUAL(c, sv2.used_elements());
                TEST_CHECK_EQUAL(sv1.used_elements(), sv2.used_elements());
                TEST_CHECK_EQUAL(c, size / 3 + 1);
            }
        }
};

SparseVectorReallocTest<float> sparse_vector_realloc_test_float("float");
SparseVectorReallocTest<double> sparse_vector_realloc_test_double("double");

