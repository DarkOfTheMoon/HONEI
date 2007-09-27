/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#ifndef ALLBENCH
#include <benchmark/benchmark.cc>
#include <tr1/memory>
#include <string>
#endif

#include <libla/product.hh>

//using namespace std;
using namespace honei;


template <typename Tag_, typename DataType_>
class BandedMatrixDenseVectorProductBench :
    public Benchmark
{
    private:
        unsigned long _size;
        int _count;
    public:
        BandedMatrixDenseVectorProductBench(const std::string & id, unsigned long size, int count) :
            Benchmark(id)
        {
            _size = size;
            _count = count;
        }

        virtual void run()
        {
            DenseVector<DataType_> dv1(_size, DataType_(2));
            std::cout<<_size<< "bla";
            BandedMatrix<DataType_> bm1(_size, dv1);
            DenseVector<DataType_> dv4(_size, DataType_(3));
            DenseVector<DataType_> dv5(dv4.copy());
            bm1.insert_band(1, dv4);
            bm1.insert_band(-1, dv5);
            DenseVector<DataType_> dv2(_size, DataType_(4));
            for (unsigned long i(0) ; i < _count ; i++)
            {
                BENCHMARK(Product<Tag_>::value(bm1, dv2));
            }
            evaluate(_size*_size);
        }
};

BandedMatrixDenseVectorProductBench<tags::CPU, float> BMDVPBenchfloat("Banded Matrix Dense Vector Product Benchmark - matrix size: 64^4, float", 64ul*64ul*64ul, 1);
BandedMatrixDenseVectorProductBench<tags::CPU, double> BMDVPBenchdouble("Banded Matrix Dense Vector Product Benchmark - matrix size: 32x32, double", 32, 10);
#ifdef HONEI_SSE
BandedMatrixDenseVectorProductBench<tags::CPU::SSE, float> SSEBMDVPBenchfloat("SSE Banded Matrix Dense Vector Product Benchmark - matrix size: 64^4, float", 64ul*64ul*64ul*64ul, 10);
#endif

template <typename DataType_>
class DenseMatrixProductBench :
    public Benchmark
{
    private:
        unsigned long _size;
        int _count;
    public:
        DenseMatrixProductBench(const std::string & id, unsigned long size, int count) :
            Benchmark(id)
        {
            _size = size;
            _count = count;
        }

        virtual void run()
        {
            DataType_ p0;
            for(int i = 0; i < _count; ++i)
            {
                DenseMatrix<DataType_> dm0(_size, _size, DataType_(rand()));
                DenseMatrix<DataType_> dm1(_size, _size, DataType_(rand()));
                BENCHMARK(Product<>::value(dm0, dm1));
            }
            evaluate(_size*_size*_size*2);
        }
};

DenseMatrixProductBench<float> DMPBenchfloat("Dense Matrix Product Benchmark - matrix size: 32x32, float", 32, 10);
DenseMatrixProductBench<double> DMPBenchdouble("Dense Matrix Product Benchmark - matrix size: 32x32, double", 32, 10);
//DenseMatrixProductBench<float> DMPBenchfloat2("Dense Matrix Product Benchmark - matrix size: 256x256, float", 256, 10);
//DenseMatrixProductBench<double> DMPBenchdouble2("Dense Matrix Product Benchmark - matrix size: 256x256, double", 256, 10);
