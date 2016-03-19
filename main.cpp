#include <iostream>
#include <string.h>
#include <mpi.h>

#include "MPIAlgorithmHelper.h"

void RunAlgorithm(MPIAlgorithm::AlgorithmsEnum algoEm, const char* valueStr);

int main(int argc, char** argv)
{
     MPI_Init (&argc, &argv);
     int myrank;
     MPI_Comm_rank (MPI_COMM_WORLD, &myrank);

    if( myrank == 0 )
    {
        bool endFlag = false;
        std::string command, valueStr;
        do{
            getline(std::cin, command); //Te getline to pole do popisu dla komunikacji przez sockety

            if( command == "bruteforce" )
            {
                getline(std::cin, valueStr);
                RunAlgorithm(MPIAlgorithm::BruteForce,valueStr.c_str()); // wystarczy wyowolac te metode
            }
            else if( command == "quit" )
            {
                endFlag = true;
            }
        }while( false == endFlag );

        SendDieMessageToAll();
    }
    else
    {
        SlaveWait();
    }

    MPI_Finalize();
    return 0;
}

void RunAlgorithm(MPIAlgorithm::AlgorithmsEnum algoE, const char* valueStr )
{
    MPIAlgorithm *algo = GetAlgorithm(algoE);

    auto returned = algo->Master(valueStr);
    delete algo;

    for( size_t i = 1; i < returned.size(); i += 2 )
        std::cout << "Returned: " << returned[i-1] << " * " << returned[i] << "\n";
    if( returned.size() == 0 )
        std::cout << "To jest liczba pierwsza!\n";
}


