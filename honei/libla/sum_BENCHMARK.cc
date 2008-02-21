/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#ifndef ALLBENCH
#include <benchmark/benchmark.cc>
#include <tr1/memory>
#include <string>
#endif

#include <honei/libla/sum.hh>


using namespace std;
using namespace honei;

template <typename Tag_, typename DataType_>
class ScalarDenseVectorSumBench :
    public Benchmark
{
    private:
        unsigned long _size;
        int _count;

    public:
        ScalarDenseVectorSumBench(const std::string & id, unsigned long size, int count) :
            Benchmark(id)
        {
            register_tag(Tag_::name);
            _size  = size;
            _count = count;
        }



        virtual void run()
        {
            DataType_ alpha = DataType_ (2);
            DenseVector<DataType_> dv(_size, DataType_(42));
            for(int i(0) ; i < _count ; ++i)
            {
                BENCHMARK(Sum<Tag_>::value(dv, DataType_ (alpha)));
            }
            BenchmarkInfo info(Sum<>::get_benchmark_info(dv, alpha));
            evaluate(info);
    }
};
ScalarDenseVectorSumBench<tags::CPU, float>  SDVSBenchfloat ("VectorShift Benchmark - vector size: 64^4, float",  64ul*64*64*64, 10);
ScalarDenseVectorSumBench<tags::CPU, double> SDVSBenchdouble("VectorShift Benchmark - svector size: 64^4, double", 64ul*64*64*64, 10);
ScalarDenseVectorSumBench<tags::CPU::MultiCore, float>  MCSDVSBenchfloat ("MC VectorShift Benchmark - vector size: 64^4, float",  64ul*64*64*64, 10);
ScalarDenseVectorSumBench<tags::CPU::MultiCore, double> MCSDVSBenchdouble("MC VectorShift Benchmark - vector size: 64^4, double", 64ul*64*64*64, 10);
#ifdef HONEI_SSE
ScalarDenseVectorSumBench<tags::CPU::SSE, float>  SSESDVSBenchfloat ("SSE VectorShift Benchmark - vector size: 64^4, float",  64ul*64*64*64, 10);
ScalarDenseVectorSumBench<tags::CPU::SSE, double> SSESDVSBenchdouble("SSE VectorShift Benchmark - vector size: 64^4, double", 64ul*64*64*64, 10);
ScalarDenseVectorSumBench<tags::CPU::MultiCore::SSE, float>  MCSSESDVSBenchfloat ("MC::SSE VectorShift Benchmark - vector size: 64^4, float",  64ul*64*64*64, 10);
ScalarDenseVectorSumBench<tags::CPU::MultiCore::SSE, double> MCSSESDVSBenchdouble("MC::SSE VectorShift Benchmark - vector size: 64^4, double", 64ul*64*64*64, 10);
#endif

template <typename Tag_, typename DataType_>
class ScalarDenseMatrixSumBench :
    public Benchmark
{
    private:
        unsigned long _size;
        int _count;

    public:
        ScalarDenseMatrixSumBench(const std::string & id, unsigned long size, int count) :
            Benchmark(id)
        {
            register_tag(Tag_::name);
            _size  = size;
            _count = count;
        }



        virtual void run()
        {
            DataType_ alpha = DataType_ (2);
            DenseMatrix<DataType_> dm0(_size, _size, DataType_(42));
            for(int i(0) ; i < _count ; ++i)
            {
                BENCHMARK(Sum<Tag_>::value(dm0, DataType_ (alpha)));
            }
            BenchmarkInfo info(Sum<>::get_benchmark_info(dm0, alpha));
            evaluate(info);
    }
};
ScalarDenseMatrixSumBench<tags::CPU, float>  SDMSBenchfloat ("MatrixShift Benchmark - size: 4096x4096, float",  4096, 10);
ScalarDenseMatrixSumBench<tags::CPU, double> SDMSBenchdouble("MatrixShift Benchmark - size: 4096x4096, double", 4096, 10);
ScalarDenseMatrixSumBench<tags::CPU::MultiCore, float>  MCSDMSBenchfloat ("MC MatrixShift Benchmark - size: 4096x4096, float",  4096, 10);
ScalarDenseMatrixSumBench<tags::CPU::MultiCore, double> MCSDMSBenchdouble("MC MatrixShift Benchmark - size: 4096x4096, double", 4096, 10);
#ifdef HONEI_SSE
ScalarDenseMatrixSumBench<tags::CPU::SSE, float>  SSESDMSBenchfloat ("SSE MatrixShift Benchmark - size: 4096x4096, float",  4096, 10);
ScalarDenseMatrixSumBench<tags::CPU::SSE, double> SSESDMSBenchdouble("SSE MatrixShift Benchmark - size: 4096x4096, double", 4096, 10);
ScalarDenseMatrixSumBench<tags::CPU::MultiCore::SSE, float>  MCSSESDMSBenchfloat ("MC::SSE MatrixShift Benchmark - size: 4096x4096, float",  4096, 10);
ScalarDenseMatrixSumBench<tags::CPU::MultiCore::SSE, double> MCSSESDMSBenchdouble("MC::SSE MatrixShift Benchmark - size: 4096x4096, double", 4096, 10);
#endif

template <typename Tag_, typename DataType_>
class DenseVectorSumBench :
    public Benchmark
{
    private:
        unsigned long _size;
        int _count;
    public:
        DenseVectorSumBench(const std::string & id, unsigned long size, int count) :
            Benchmark(id)
        {
            register_tag(Tag_::name);
            _size = size;
            _count = count;
        }

        virtual void run()
        {
            DenseVector<DataType_> dv0(_size, DataType_(rand()));
            DenseVector<DataType_> dv1(_size, DataType_(rand()));
            for(int i(0) ; i < _count; ++i)
            {
                BENCHMARK(
                        for (unsigned long j(0) ; j < 10 ; ++j)
                        {
                        Sum<Tag_>::value(dv0, dv1);
                        }
                        );
            }
            BenchmarkInfo info(Sum<>::get_benchmark_info(dv0, dv1));
            evaluate(info * 10);
        }
};
DenseVectorSumBench<tags::CPU, float> DVSBenchfloat1("Dense Vector Sum Benchmark - vector size: 64^4, float", 64ul*64*64*64, 10);
DenseVectorSumBench<tags::CPU, double> DVSBenchdouble1("Dense Vector Sum Benchmark - vector size: 64^4, double", 64ul*64*64*64, 10);
DenseVectorSumBench<tags::CPU::MultiCore, float> MCDVSBenchfloat1("MC Dense Vector Sum Benchmark - vector size: 64^4, float", 64ul*64*64*64, 10);
DenseVectorSumBench<tags::CPU::MultiCore, double> MCDVSBenchdouble1("MC Dense Vector Sum Benchmark - vector size: 64^4, double", 64ul*64*64*64, 10);
#ifdef HONEI_SSE
DenseVectorSumBench<tags::CPU::SSE, float> SSEDVSBenchfloat1("SSE Dense Vector Sum Benchmark - vector size: 64^4, float", 64ul*64ul*64ul*64ul, 50);
DenseVectorSumBench<tags::CPU::SSE, double> SSEDVSBenchdouble1("SSE Dense Vector Sum Benchmark - vector size: 64^4, double", 64ul*64ul*64ul*64ul, 50);
DenseVectorSumBench<tags::CPU::MultiCore::SSE, float> MCSSEDVSBenchfloat1("MC::SSE Dense Vector Sum Benchmark - vector size: 64^4, float", 64ul*64ul*64ul*64ul, 50);
DenseVectorSumBench<tags::CPU::MultiCore::SSE, double> MCSSEDVSBenchdouble1("MC::SSE Dense Vector Sum Benchmark - vector size: 64^4, double", 64ul*64ul*64ul*64ul, 50);
#endif
#ifdef HONEI_CELL
DenseVectorSumBench<tags::Cell, float> dvs_bench_cell_float("Cell Dense Vector Sum Benchmark - vector size: 64^4, float", 64ul * 64 * 64 * 64, 50);
DenseVectorSumBench<tags::Cell, double> dvs_bench_cell_double("Cell Dense Vector Sum Benchmark - vector size: 64^3 * 35, double",
        64ul * 64 * 64 * 35, 50);
#endif

template <typename Tag_, typename DataType_>
class DenseVectorRangeSumBench :
    public Benchmark
{
    private:
        unsigned long _size;
        int _count;
    public:
        DenseVectorRangeSumBench(const std::string & id, unsigned long size, int count) :
            Benchmark(id)
        {
            register_tag(Tag_::name);
            _size = size;
            _count = count;
        }

        virtual void run()
        {
            DenseVector<DataType_> dv0(_size, DataType_(rand()));
            DenseVector<DataType_> dv1(_size, DataType_(rand()));
            DenseVectorRange<DataType_> dvr0 (dv0, _size - 2, 0);
            DenseVectorRange<DataType_> dvr1 (dv1, _size - 2, 1);
            for(int i(0) ; i < _count; ++i)
            {
                BENCHMARK(Sum<Tag_>::value(dvr0, dvr1));
            }
            BenchmarkInfo info(Sum<>::get_benchmark_info(dvr0, dvr1));
            evaluate(info);
        }
};

DenseVectorRangeSumBench<tags::CPU, float> DVRSBenchfloat1("Dense Vector Range Sum Benchmark - vector size: 64^4, float", 64ul*64*64*64, 10);
DenseVectorRangeSumBench<tags::CPU, double> DVRSBenchdouble1("Dense Vector Range Sum Benchmark - vector size: 64^4, double", 64ul*64*64*64, 10);
#ifdef HONEI_SSE
DenseVectorRangeSumBench<tags::CPU::SSE, float> SSEDVRSBenchfloat1("SSE Dense Vector Range Sum Benchmark - vector size: 64^4, float", 64ul*64ul*64ul*64ul, 10);
DenseVectorRangeSumBench<tags::CPU::SSE, double> SSEDVRSBenchdouble1("SSE Dense Vector Range Sum Benchmark - vector size: 64^4, double", 64ul*64ul*64ul*64ul, 10);
#endif
#ifdef HONEI_CELL
DenseVectorRangeSumBench<tags::Cell, float> CELLDVRSBenchfloat1("Cell Dense Vector Range Sum Benchmark - vector size: 64^4, float", 64ul*64*64*64, 10);
DenseVectorRangeSumBench<tags::Cell, double> CELLDVRSBenchdouble1("Cell Dense Vector Range Sum Benchmark - vector size: 64^3 * 35, double", 64ul*64*64 * 35, 10);
#endif

template <typename Tag_, typename DataType_>
class DenseMatrixSumBench :
    public Benchmark
{
    private:
        unsigned long _sizex, _sizey;
        int _count;
    public:
        DenseMatrixSumBench(const std::string & id, unsigned long sizex, unsigned long sizey, int count) :
            Benchmark(id)
        {
            register_tag(Tag_::name);
            _sizex = sizex;
            _sizey = sizey;
            _count = count;
        }

        virtual void run()
        {
            DenseMatrix<DataType_> dm0(_sizex, _sizey, DataType_(rand()));
            DenseMatrix<DataType_> dm1(_sizex, _sizey, DataType_(rand()));
            for(int i(0) ; i < _count; ++i)
            {
                BENCHMARK(Sum<Tag_>::value(dm0, dm1));
            }
            BenchmarkInfo info(Sum<>::get_benchmark_info(dm0, dm1));
            evaluate(info);
        }
};

DenseMatrixSumBench<tags::CPU, float> DMSBenchfloat1("Dense Matrix Sum Benchmark - Matrix size: 4096x4096, float", 4096, 4096, 10);
DenseMatrixSumBench<tags::CPU, double> DMSBenchdouble1("Dense Matrix Sum Benchmark - Matrix size: 4096x4096, double", 4096, 4096, 10);
DenseMatrixSumBench<tags::CPU::MultiCore, float> DMSBenchfloat0mc("MC: Dense Matrix Sum Benchmark - Matrix size: 4096x4096, float", 4096, 4096, 10);
DenseMatrixSumBench<tags::CPU::MultiCore, double> DMSBenchdouble0mc("MC: Dense Matrix Sum Benchmark - Matrix size: 4096x4096, double", 4096, 4096, 10);
#ifdef HONEI_SSE
DenseMatrixSumBench<tags::CPU::SSE, float> DMSBenchfloat1sse("SSE Dense Matrix Sum Benchmark - Matrix size: 4096x4096, float", 4096, 4096, 10);
DenseMatrixSumBench<tags::CPU::SSE, double> DMSBenchdouble1sse("SSE Dense Matrix Sum Benchmark - Matrix size: 4096x4096, double", 4096, 4096, 10);
DenseMatrixSumBench<tags::CPU::MultiCore::SSE, float> DMSBenchfloat1mcsse("MC SSE : Dense Matrix Sum Benchmark - Matrix size: 4096x4096, float", 4096, 4096, 10);
DenseMatrixSumBench<tags::CPU::MultiCore::SSE, double> DMSBenchdouble1mcsse("MC SSE: Dense Matrix Sum Benchmark - Matrix size: 4096x4096, double", 4096, 4096, 10);
#endif
#ifdef HONEI_CELL
DenseMatrixSumBench<tags::Cell, float> DMSBenchfloat1cell("Cell Dense Matrix Sum Benchmark - Matrix size: 4096x4096, 4096, float", 4096, 4096, 10);
DenseMatrixSumBench<tags::Cell, double> DMSBenchdouble1cell("Cell Dense Matrix Sum Benchmark - Matrix size: 4096x4096, 4096, double", 4096, 4096, 10);
#endif

template <typename Tag_, typename DataType_>
class DenseMatrixBandedMatrixSumBench :
    public Benchmark
{
    private:
        unsigned long _size;
        int _count;
    public:
        DenseMatrixBandedMatrixSumBench(const std::string & id, unsigned long size, int count) :
            Benchmark(id)
        {
            register_tag(Tag_::name);
            _size = size;
            _count = count;
        }

        virtual void run()
        {
            DenseMatrix<DataType_> dm0(_size, _size, DataType_(rand()));
            DenseVector<DataType_> dv (_size, DataType_(rand()));
            BandedMatrix<DataType_> bm(_size, dv);
            for(int i = 0 ; i < _size; i += 10) {
                bm.insert_band(i, dv);
                bm.insert_band(-i, dv);
            }
            
            for(int i = 0 ; i < _count ; ++i)
            {
                BENCHMARK(Sum<Tag_>::value(dm0, bm));
            }
            BenchmarkInfo info(Sum<>::get_benchmark_info(dm0, bm));
            evaluate(info);
        }
};

DenseMatrixBandedMatrixSumBench<tags::CPU, float> DMBMSBenchfloat1("Dense Matrix Banded Matrix Sum Benchmark - Matrix size: 4096x4096, float", 4096, 10);
DenseMatrixBandedMatrixSumBench<tags::CPU, double> DMBMSBenchdouble1("Dense Matrix Banded Matrix Sum Benchmark - Matrix size: 4096x4096, double", 4096, 10);
DenseMatrixBandedMatrixSumBench<tags::CPU::MultiCore, float> DMBMSBenchfloat1mc("MC: Dense Matrix Banded Matrix Sum Benchmark - Matrix size: 4096x4096, float", 4096, 10);
DenseMatrixBandedMatrixSumBench<tags::CPU::MultiCore, double> DMSBMBenchdouble1mc("MC: Dense Matrix Banded Matrix Sum Benchmark - Matrix size: 4096x4096, double", 4096, 10);
#ifdef HONEI_SSE
DenseMatrixBandedMatrixSumBench<tags::CPU::SSE, float> DMBMSBenchfloat1sse("SSE Dense Matrix Banded Matrix Sum Benchmark - Matrix size: 4096x4096, float", 4096, 10);
DenseMatrixBandedMatrixSumBench<tags::CPU::SSE, double> DMBMSBenchdouble1sse("SSE Dense Matrix Banded Matrix Sum Benchmark - Matrix size: 4096x4096, double", 4096, 10);
DenseMatrixBandedMatrixSumBench<tags::CPU::MultiCore::SSE, float> DMBMSBenchfloat1mcsse("MC::SSE: Dense Matrix Banded Matrix Sum Benchmark - Matrix size: 4096x4096, float", 4096, 10);
DenseMatrixBandedMatrixSumBench<tags::CPU::MultiCore::SSE, double> DMSBMBenchdouble1mcsse("MC::SSE: Dense Matrix Banded Matrix Sum Benchmark - Matrix size: 4096x4096, double", 4096, 10);
#endif

template <typename DT_, typename Tag_>
class DenseVectorSumBenchTestPlot :
    public Benchmark
{
    private:
        int _x;

    public:
        DenseVectorSumBenchTestPlot(const std::string & id, int x) :
            Benchmark(id)
        {
            register_tag(Tag_::name);
            _plots = true;
            _x = x;
        }

        virtual void run()
        {
            BenchmarkInfo info;
            std::list<BenchmarkInfo> infolist;
            std::list<int> cores;
            // increasing sizes
            if (_x == 0)
            {
                for (unsigned long j(1) ; j < 51 ; ++j)
                {
                    cores.push_back(1);
                    DenseVector<DT_> dv0(j * 200000, DT_(rand()));
                    DenseVector<DT_> dv1(j * 200000, DT_(rand()));
                    for(int i(0) ; i < 20 ; ++i)
                    {
                        BENCHMARK(Sum<Tag_>::value(dv0, dv1));
                    }
                    info = Sum<>::get_benchmark_info(dv0, dv1);
                    infolist.push_back(info);
                    std::cout << "finished run " << j << " / " << 50 << std::endl;
                }
            }
            // increasing maximum number of parts
            if (_x == 1)
            {
                for (unsigned long j(1) ; j < 21 ; ++j)
                {
                    Configuration::instance()->set_value("mc::sum[DVCB,DVCB]::max-count", j);
                    cores.push_back(j);
                    DenseVector<DT_> dv0(5000000, DT_(rand()));
                    DenseVector<DT_> dv1(5000000, DT_(rand()));

                    for(int i(0) ; i < 20 ; ++i)
                    {
                        BENCHMARK(Sum<Tag_>::value(dv0, dv1));
                    }
                    info = Sum<>::get_benchmark_info(dv0, dv1);
                    infolist.push_back(info);
                    std::cout << "finished run " << j << " / " << 20 << std::endl;
                }
            }
            // increasing maximum number of parts and sizes
            if (_x == 2)
            {
                for (unsigned long j(1) ; j < 21 ; ++j)
                {
                    for (unsigned long k(1) ; k < 21 ; ++k)
                    {
                        Configuration::instance()->set_value("mc::sum[DVCB,DVCB]::max-count", j);
                        cores.push_back(j);
                        DenseVector<DT_> dv0(k * 100000, DT_(rand()));
                        DenseVector<DT_> dv1(k * 100000, DT_(rand()));
    
                        for(int i(0) ; i < 20 ; ++i)
                        {
                            BENCHMARK(Sum<Tag_>::value(dv0, dv1));
                        }
                        info = Sum<>::get_benchmark_info(dv0, dv1);
                        infolist.push_back(info);
                        std::cout << "finished run " << (j-1)*20 + k << " / " << 400 << std::endl;
                    }
                }
            }

            evaluate_to_plotfile(infolist, cores, 20);
        }
};
DenseVectorSumBenchTestPlot<double, tags::CPU> DVSBTP("Dense Vector Sum Benchmark - vector size: x * 200.000, 0<x<51 - double", 0);
DenseVectorSumBenchTestPlot<double, tags::CPU::MultiCore> MCDVSBTP("MC: Dense Vector Sum Benchmark - vector size: x * 200.000, 0<x<51 - double", 0);
DenseVectorSumBenchTestPlot<double, tags::CPU::MultiCore> MCDVSBTP1("MC: Dense Vector Sum Benchmark - vector size: 5.000.000, 1 - 20 parts - double", 1);
DenseVectorSumBenchTestPlot<double, tags::CPU::MultiCore> MCDVSBTP2("MC: Dense Vector Sum Benchmark - vector size: x * 100.000, 0<x<21, 1 - 20 parts - double", 2);

