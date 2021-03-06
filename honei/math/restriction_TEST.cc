/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2008 - 2010 Markus Geveler <apryde@gmx.de>
 * Copyright (c) 2008 Dirk Ribbrock <dirk.ribbrock@uni-dortmund.de>
 *
 * This file is part of the Math C++ library. LibMath is free software;
 * you can redistribute it and/or modify it under the terms of the GNU General
 * Public License version 2, as published by the Free Software Foundation.
 *
 * LibMath is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <honei/math/restriction.hh>
#include <honei/math/matrix_io.hh>
#include <honei/math/transposition.hh>
#include <honei/util/unittest.hh>
#include <honei/util/stringify.hh>
#include <iostream>
#include <honei/la/dense_matrix.hh>
using namespace honei;
using namespace tests;
using namespace std;

template <typename Tag_, typename DT1_>
class RestrictionTest:
    public BaseTest
{
    public:
        RestrictionTest(const std::string & tag) :
            BaseTest("Restriction test <" + tag + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            for (float level(0) ; level < 10 ; ++level)
            {
                unsigned long N_fine((unsigned long)pow((pow(2, level + 1) + 1), 2));
                //unsigned long width_fine = (unsigned long)sqrt((double)N_fine);
                unsigned long N_coarse((unsigned long)pow((pow(2, level) + 1), 2));
                //unsigned long width_coarse = (unsigned long)sqrt((double)N_coarse);

                DenseVector<DT1_> fine(N_fine, DT1_(1));
                DenseVector<DT1_> coarse(N_coarse, DT1_(4711));
                DenseVector<DT1_> coarse_ref(N_coarse, DT1_(4711));
                /*for (unsigned long i(0) ; i < fine.size() ; ++i)
                {
                    fine[i] = DT1_(i % 1000);
                }*/
                DenseVector<unsigned long> mask(8);
                for(unsigned long i(0) ; i < 8 ; ++i)
                {
                    mask[i] = 2;
                }

                SparseMatrix<DT1_> sm(1, 1);
                SparseMatrixELL<DT1_> dummy(sm);
                Restriction<Tag_, methods::NONE>::value(coarse, fine, mask, dummy);
                Restriction<tags::CPU, methods::NONE>::value(coarse_ref, fine, mask, dummy);
                coarse.lock(lm_read_only);
                coarse_ref.lock(lm_read_only);
                std::cout << "At level: " << level + 1 << std::endl;
                for(unsigned long i(0) ; i < coarse.size() ; ++i)
                {
                    if (coarse[i] != coarse_ref[i])
                        std::cout << "Not matching: " << i << std::endl;
                }
                TEST_CHECK_EQUAL(coarse, coarse_ref);

                coarse.unlock(lm_read_only);
                coarse_ref.unlock(lm_read_only);
            }
        }
};
#ifdef HONEI_CUDA
RestrictionTest<tags::GPU::CUDA, float> cuda_restriction_test_float("float");
#ifdef HONEI_CUDA_DOUBLE
RestrictionTest<tags::GPU::CUDA, double> cuda_restriction_test_double("double");
#endif
#endif

template <typename Tag_, typename DT1_>
class RestrictionVisualTest:
    public BaseTest
{
    public:
        RestrictionVisualTest(const std::string & tag) :
            BaseTest("Restriction visual test <" + tag + ">")
        {
            register_tag(Tag_::name);
        }

        virtual void run() const
        {
            std::string prol_file(HONEI_SOURCEDIR);
            prol_file += "/honei/math/testdata/poisson/";
            prol_file += "prol_2.ell";
            SparseMatrixELL<DT1_> prolmat(MatrixIO<io_formats::ELL>::read_matrix(prol_file, DT1_(0)));
            SparseMatrix<DT1_> prol(prolmat);
            SparseMatrix<DT1_> res(Transposition<Tag_>::value(prol));
            SparseMatrixELL<DT1_> resmat(res);
            std::cout << resmat << std::endl;
        }
};
#ifdef HONEI_CUDA
RestrictionVisualTest<tags::GPU::CUDA, float> cuda_restriction_vis__test_float("float");
#ifdef HONEI_CUDA_DOUBLE
RestrictionVisualTest<tags::GPU::CUDA, double> cuda_restriction_vis_test_double("double");
#endif
#endif
