#include "MPIAlgorithmHelper.h"

MPIAlgorithm* GetAlgorithm(MPIAlgorithm::AlgorithmsEnum algorithms)
{
    switch(algorithms)
    {
        case MPIAlgorithm::BruteForce:
            return new BruteForceAlgorithm;
    }
    return nullptr;
}

void SlaveWait()
{
    MPIAlgorithm::AlgorithmsEnum algoE;
    MPI_Status status;
    while(1)
    {
        MPI_Recv(&algoE,1,MPI_INT,0,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
        switch( status.MPI_TAG )
        {
            case MPIAlgorithm::SETALGO_TAG:
            {
                MPIAlgorithm *algo = GetAlgorithm(algoE);
                algo->Slave();
                delete algo;
                break;
            }
            case MPIAlgorithm::DIETAG:
                return;
        }
    }
}

void SendDieMessageToAll()
{
    int size;
    MPI_Comm_size(MPI_COMM_WORLD,&size);

    for( int i = 1; i < size; ++i )
    {
            MPI_Send(0,0,MPI_CHAR,i,MPIAlgorithm::DIETAG,MPI_COMM_WORLD);
    }
}
