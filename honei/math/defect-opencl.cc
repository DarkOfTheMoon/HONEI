/* vim: set sw=4 sts=4 et nofoldenable : */

/*
 * Copyright (c)  2008, 2010 Dirk Ribbrock <dirk.ribbrock@uni-dortmund.de>
 *
 * This file is part of the HONEI C++ library. HONEI is free software;
 * you can redistribute it and/or modify it under the terms of the GNU General
 * Public License version 2, as published by the Free Software Foundation.
 *
 * HONEI is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <honei/math/defect.hh>
#include <honei/backends/opencl/operations.hh>
#include <honei/util/profiler.hh>

namespace honei
{
    namespace opencl
    {
        template <typename Tag_, typename DT_>
        void common_defect(DenseVectorContinuousBase<DT_> & result, const DenseVectorContinuousBase<DT_> & rhs, const SparseMatrixELL<DT_> & a, const DenseVectorContinuousBase<DT_> & b)
        {
            if (b.size() != a.columns())
            {
                throw VectorSizeDoesNotMatch(b.size(), a.columns());
            }
            if (a.rows() != result.size())
            {
                throw VectorSizeDoesNotMatch(a.rows(), result.size());
            }
            if (b.size() != rhs.size())
            {
                throw VectorSizeDoesNotMatch(b.size(), rhs.size());
            }

            void * rhs_cl(rhs.lock(lm_read_only, Tag_::memory_value));
            void * b_cl(b.lock(lm_read_only, Tag_::memory_value));
            void * result_cl(result.lock(lm_write_only, Tag_::memory_value));
            void * Aj_cl(a.Aj().lock(lm_read_only, Tag_::memory_value));
            void * Ax_cl(a.Ax().lock(lm_read_only, Tag_::memory_value));
            void * Arl_cl(a.Arl().lock(lm_read_only, Tag_::memory_value));
            std::string opname("defect_smell_dv_");
            opname += typeid(DT_).name();
            defect_smell_dv<DT_>(rhs_cl, b_cl, result_cl, Aj_cl, Ax_cl, Arl_cl,
                    a.rows(), a.columns(), a.num_cols_per_row(), a.stride(), a.threads(), tag_to_device<Tag_>(), opname);
            result.unlock(lm_write_only);
            rhs.unlock(lm_read_only);
            b.unlock(lm_read_only);
            a.Aj().unlock(lm_read_only);
            a.Ax().unlock(lm_read_only);
            a.Arl().unlock(lm_read_only);
        }

        template <typename Tag_, typename DT_>
        void common_defect(DenseVectorContinuousBase<DT_> & result, const DenseVectorContinuousBase<DT_> & rhs, const BandedMatrixQx<Q1Type, DT_> & a, const DenseVectorContinuousBase<DT_> & b)
        {
            if (b.size() != a.columns())
            {
                throw VectorSizeDoesNotMatch(b.size(), a.columns());
            }
            if (result.size() != a.rows())
            {
                throw VectorSizeDoesNotMatch(result.size(), a.rows());
            }
            if (b.size() != rhs.size())
            {
                throw VectorSizeDoesNotMatch(b.size(), rhs.size());
            }

            void * rhs_cl(rhs.lock(lm_read_only, Tag_::memory_value));
            void * b_cl(b.lock(lm_read_only, Tag_::memory_value));
            void * result_cl(result.lock(lm_write_only, Tag_::memory_value));
            void * ll_cl(a.band(LL).lock(lm_read_only, Tag_::memory_value));
            void * ld_cl(a.band(LD).lock(lm_read_only, Tag_::memory_value));
            void * lu_cl(a.band(LU).lock(lm_read_only, Tag_::memory_value));
            void * dl_cl(a.band(DL).lock(lm_read_only, Tag_::memory_value));
            void * dd_cl(a.band(DD).lock(lm_read_only, Tag_::memory_value));
            void * du_cl(a.band(DU).lock(lm_read_only, Tag_::memory_value));
            void * ul_cl(a.band(UL).lock(lm_read_only, Tag_::memory_value));
            void * ud_cl(a.band(UD).lock(lm_read_only, Tag_::memory_value));
            void * uu_cl(a.band(UU).lock(lm_read_only, Tag_::memory_value));

            std::string opname("defect_q1_");
            opname += typeid(DT_).name();
            defect_q1<DT_>(ll_cl, ld_cl, lu_cl,
                    dl_cl, dd_cl, du_cl,
                    ul_cl, ud_cl, uu_cl,
                    rhs_cl, b_cl, result_cl, a.size(), a.root(), tag_to_device<Tag_>(), opname);

            rhs.unlock(lm_read_only);
            result.unlock(lm_write_only);
            b.unlock(lm_read_only);
            a.band(LL).unlock(lm_read_only);
            a.band(LD).unlock(lm_read_only);
            a.band(LU).unlock(lm_read_only);
            a.band(DL).unlock(lm_read_only);
            a.band(DD).unlock(lm_read_only);
            a.band(DU).unlock(lm_read_only);
            a.band(UL).unlock(lm_read_only);
            a.band(UD).unlock(lm_read_only);
            a.band(UU).unlock(lm_read_only);
        }
    }
}

using namespace honei;

template <typename DT_>
DenseVector<DT_> Defect<tags::OpenCL::CPU>::value(const DenseVectorContinuousBase<DT_> & rhs,
        const SparseMatrixELL<DT_> & a,
        const DenseVectorContinuousBase<DT_> & b)
{
    CONTEXT("When calculating Defect<DT_> (OpenCL CPU):");
    PROFILER_START("Defect tags::OpenCL::CPU");
    DenseVector<DT_> result(a.rows());
    opencl::common_defect<tags::OpenCL::CPU>(result, rhs, a, b);
    PROFILER_STOP("Defect tags::OpenCL::CPU");
    return result;
}
template DenseVector<float> Defect<tags::OpenCL::CPU>::value(const DenseVectorContinuousBase<float> &, const SparseMatrixELL<float> &, const DenseVectorContinuousBase<float> &);
template DenseVector<double> Defect<tags::OpenCL::CPU>::value(const DenseVectorContinuousBase<double> &, const SparseMatrixELL<double> &, const DenseVectorContinuousBase<double> &);

template <typename DT_>
DenseVector<DT_> Defect<tags::OpenCL::GPU>::value(const DenseVectorContinuousBase<DT_> & rhs,
        const SparseMatrixELL<DT_> & a,
        const DenseVectorContinuousBase<DT_> & b)
{
    CONTEXT("When calculating Defect<DT_> (OpenCL GPU):");
    PROFILER_START("Defect tags::OpenCL::GPU");
    DenseVector<DT_> result(a.rows());
    opencl::common_defect<tags::OpenCL::GPU>(result, rhs, a, b);
    PROFILER_STOP("Defect tags::OpenCL::GPU");
    return result;
}
template DenseVector<float> Defect<tags::OpenCL::GPU>::value(const DenseVectorContinuousBase<float> &, const SparseMatrixELL<float> &, const DenseVectorContinuousBase<float> &);
template DenseVector<double> Defect<tags::OpenCL::GPU>::value(const DenseVectorContinuousBase<double> &, const SparseMatrixELL<double> &, const DenseVectorContinuousBase<double> &);

template <typename DT_>
DenseVectorContinuousBase<DT_> & Defect<tags::OpenCL::CPU>::value(DenseVectorContinuousBase<DT_> & result, const DenseVectorContinuousBase<DT_> & rhs,
        const SparseMatrixELL<DT_> & a,
        const DenseVectorContinuousBase<DT_> & b)
{
    CONTEXT("When calculating Defect<DT_> (OpenCL CPU):");
    PROFILER_START("Defect tags::OpenCL::CPU");
    opencl::common_defect<tags::OpenCL::CPU>(result, rhs, a, b);
    PROFILER_STOP("Defect tags::OpenCL::CPU");
    return result;
}
template DenseVectorContinuousBase<float> & Defect<tags::OpenCL::CPU>::value(DenseVectorContinuousBase<float> &, const DenseVectorContinuousBase<float> &, const SparseMatrixELL<float> &, const DenseVectorContinuousBase<float> &);
template DenseVectorContinuousBase<double> & Defect<tags::OpenCL::CPU>::value(DenseVectorContinuousBase<double> &, const DenseVectorContinuousBase<double> &, const SparseMatrixELL<double> &, const DenseVectorContinuousBase<double> &);

template <typename DT_>
DenseVectorContinuousBase<DT_> & Defect<tags::OpenCL::GPU>::value(DenseVectorContinuousBase<DT_> & result, const DenseVectorContinuousBase<DT_> & rhs,
        const SparseMatrixELL<DT_> & a,
        const DenseVectorContinuousBase<DT_> & b)
{
    CONTEXT("When calculating Defect<DT_> (OpenCL GPU):");
    PROFILER_START("Defect tags::OpenCL::GPU");
    opencl::common_defect<tags::OpenCL::GPU>(result, rhs, a, b);
    PROFILER_STOP("Defect tags::OpenCL::GPU");
    return result;
}
template DenseVectorContinuousBase<float> & Defect<tags::OpenCL::GPU>::value(DenseVectorContinuousBase<float> &, const DenseVectorContinuousBase<float> &, const SparseMatrixELL<float> &, const DenseVectorContinuousBase<float> &);
template DenseVectorContinuousBase<double> & Defect<tags::OpenCL::GPU>::value(DenseVectorContinuousBase<double> &, const DenseVectorContinuousBase<double> &, const SparseMatrixELL<double> &, const DenseVectorContinuousBase<double> &);

template <typename DT_>
DenseVector<DT_> Defect<tags::OpenCL::CPU>::value(const DenseVectorContinuousBase<DT_> & rhs,
        const BandedMatrixQx<Q1Type, DT_> & a,
        const DenseVectorContinuousBase<DT_> & b)
{
    CONTEXT("When calculating Defect<DT_> (OpenCL CPU):");
    PROFILER_START("Defect tags::OpenCL::CPU");
    DenseVector<DT_> result(a.rows());
    opencl::common_defect<tags::OpenCL::CPU>(result, rhs, a, b);
    PROFILER_STOP("Defect tags::OpenCL::CPU");
    return result;
}
template DenseVector<float> Defect<tags::OpenCL::CPU>::value(const DenseVectorContinuousBase<float> &, const BandedMatrixQx<Q1Type, float> &, const DenseVectorContinuousBase<float> &);
template DenseVector<double> Defect<tags::OpenCL::CPU>::value(const DenseVectorContinuousBase<double> &, const BandedMatrixQx<Q1Type, double> &, const DenseVectorContinuousBase<double> &);

template <typename DT_>
DenseVector<DT_> Defect<tags::OpenCL::GPU>::value(const DenseVectorContinuousBase<DT_> & rhs,
        const BandedMatrixQx<Q1Type, DT_> & a,
        const DenseVectorContinuousBase<DT_> & b)
{
    CONTEXT("When calculating Defect<DT_> (OpenCL GPU):");
    PROFILER_START("Defect tags::OpenCL::GPU");
    DenseVector<DT_> result(a.rows());
    opencl::common_defect<tags::OpenCL::GPU>(result, rhs, a, b);
    PROFILER_STOP("Defect tags::OpenCL::GPU");
    return result;
}
template DenseVector<float> Defect<tags::OpenCL::GPU>::value(const DenseVectorContinuousBase<float> &, const BandedMatrixQx<Q1Type, float> &, const DenseVectorContinuousBase<float> &);
template DenseVector<double> Defect<tags::OpenCL::GPU>::value(const DenseVectorContinuousBase<double> &, const BandedMatrixQx<Q1Type, double> &, const DenseVectorContinuousBase<double> &);

template <typename DT_>
DenseVectorContinuousBase<DT_> & Defect<tags::OpenCL::CPU>::value(DenseVectorContinuousBase<DT_> & result, const DenseVectorContinuousBase<DT_> & rhs,
        const BandedMatrixQx<Q1Type, DT_> & a,
        const DenseVectorContinuousBase<DT_> & b)
{
    CONTEXT("When calculating Defect<DT_> (OpenCL CPU):");
    PROFILER_START("Defect tags::OpenCL::CPU");
    opencl::common_defect<tags::OpenCL::CPU>(result, rhs, a, b);
    PROFILER_STOP("Defect tags::OpenCL::CPU");
    return result;
}
template DenseVectorContinuousBase<float> & Defect<tags::OpenCL::CPU>::value(DenseVectorContinuousBase<float> &, const DenseVectorContinuousBase<float> &, const BandedMatrixQx<Q1Type, float> &, const DenseVectorContinuousBase<float> &);
template DenseVectorContinuousBase<double> & Defect<tags::OpenCL::CPU>::value(DenseVectorContinuousBase<double> &, const DenseVectorContinuousBase<double> &, const BandedMatrixQx<Q1Type, double> &, const DenseVectorContinuousBase<double> &);

template <typename DT_>
DenseVectorContinuousBase<DT_> & Defect<tags::OpenCL::GPU>::value(DenseVectorContinuousBase<DT_> & result, const DenseVectorContinuousBase<DT_> & rhs,
        const BandedMatrixQx<Q1Type, DT_> & a,
        const DenseVectorContinuousBase<DT_> & b)
{
    CONTEXT("When calculating Defect<DT_> (OpenCL GPU):");
    PROFILER_START("Defect tags::OpenCL::GPU");
    opencl::common_defect<tags::OpenCL::GPU>(result, rhs, a, b);
    PROFILER_STOP("Defect tags::OpenCL::GPU");
    return result;
}
template DenseVectorContinuousBase<float> & Defect<tags::OpenCL::GPU>::value(DenseVectorContinuousBase<float> &, const DenseVectorContinuousBase<float> &, const BandedMatrixQx<Q1Type, float> &, const DenseVectorContinuousBase<float> &);
template DenseVectorContinuousBase<double> & Defect<tags::OpenCL::GPU>::value(DenseVectorContinuousBase<double> &, const DenseVectorContinuousBase<double> &, const BandedMatrixQx<Q1Type, double> &, const DenseVectorContinuousBase<double> &);
