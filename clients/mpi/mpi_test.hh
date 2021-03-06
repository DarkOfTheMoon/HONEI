/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2008 Dirk Ribbrock <dirk.ribbrock@uni-dortmund.de>
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

#ifndef MPI_GUARD_MPI_TEST_HH
#define MPU_GUARD_MPI_TEST_HH 1

#include <mpi.h>
#include <honei/la/dense_vector.hh>
#include <honei/la/scale.hh>
#include <honei/la/sum.hh>
#include <honei/backends/mpi/operations.hh>

#include <iostream>

namespace honei
{
    class MPITest
    {
        private:
            int _numprocs;
            int _myid;
            MPI_Status _stat;

        public:
            MPITest(int argc, char **argv)
            {
                //MPI_Init(&argc, &argv);
                mpi::mpi_init(&argc, &argv);
                //MPI_Comm_size(MPI_COMM_WORLD, &_numprocs);
                mpi::mpi_comm_size(&_numprocs);
                //MPI_Comm_rank(MPI_COMM_WORLD, &_myid);
                mpi::mpi_comm_rank(&_myid);

                if (_myid == 0)
                {
                    _master();
                }
                else
                {
                    _slave();
                }
            }

            ~MPITest()
            {
                //MPI_Finalize();
                mpi::mpi_finalize();
            }

        private:
            void _master()
            {
                DenseVector<float> dv(10, 1.001f);
                std::cout << "Master: " << dv << std::endl;
                //MPI_Bcast(dv.elements(), dv.size() * sizeof(float), MPI_BYTE, 0, MPI_COMM_WORLD);
                mpi::mpi_bcast(dv.elements(), dv.size(), 0);
                for(signed long i(1) ; i < _numprocs ; ++i)
                {
                    DenseVector<float> temp(10);
                    //MPI_Recv(temp.elements(), temp.size() * sizeof(float), MPI_BYTE, i, i, MPI_COMM_WORLD, &_stat);
                    mpi::mpi_recv(temp.elements(), temp.size(), i, i);
                    Sum<>::value(dv, temp);
                }
                std::cout << "Master + (2 * Master * slavecount) =" << std::endl;
                std::cout << dv << std::endl;
            }

            void _slave()
            {
                DenseVector<float> dv(10);
                //MPI_Bcast(dv.elements(), dv.size() * sizeof(float), MPI_BYTE, 0, MPI_COMM_WORLD);
                mpi::mpi_bcast(dv.elements(), dv.size(), 0);
                Scale<>::value(dv, 2.0f);
                //MPI_Send(dv.elements(), dv.size() * sizeof(float), MPI_BYTE, 0, _myid, MPI_COMM_WORLD);
                mpi::mpi_send(dv.elements(), dv.size(), 0, _myid);
            }
    };
}
#endif
