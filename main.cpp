#include <iostream>
#include <string.h>
#include <mpi.h>
#include <memory>

#include "MPIAlgorithmHelper.h"
#include "MySQLFactorerCommunicator.h"
#include "CmdFactorerCommunicator.h"
using namespace std;

vector<string> RunAlgorithm(MPIAlgorithm::AlgorithmsEnum algoEm, const char* valueStr);
bool Init(int *argc, char ***argv, int *myRank);
FactorerCommunicatorInterface *InitCommunicator(int argc, char ** argv);

int main(int argc, char** argv)
{
    int myRank;
    if( Init(&argc, &argv, &myRank) == false )
    {
        cerr << "Program wymaga przynajmniej 2 watkow!\n";
        MPI_Finalize();
        return 1;
    }

    if( myRank == 0 )
    {
        try{
        unique_ptr<FactorerCommunicatorInterface> communicator(InitCommunicator(argc, argv));
        CommunicatorCommand communicatorCommand;
        MPIAlgorithm::AlgorithmsEnum algorithm;
        string valueStr;

        do{
            communicatorCommand = communicator->getCommand(algorithm, valueStr);
            if( communicatorCommand == CommunicatorCommand::Algorithm )
            {
                auto result = RunAlgorithm(algorithm, valueStr.c_str());
                communicator->algorithmFinnished(result);
            }
        }while(communicatorCommand != CommunicatorCommand::Quit);

        }
        catch( FactorerCommunicatorException& fExc )
        {
            cerr << fExc.what();
        }

        SendDieMessageToAll();
    }
    else
    {
        SlaveWait();
    }

    MPI_Finalize();
    return 0;
}

vector<string> RunAlgorithm(MPIAlgorithm::AlgorithmsEnum algoE, const char* valueStr )
{
    MPIAlgorithm *algo = GetAlgorithm(algoE);

    auto returned = algo->Master(valueStr);
    delete algo;

    return returned;
}

bool Init(int *argc, char ***argv, int *myRank)
{
     int commSize;
     MPI_Init (argc, argv);
     MPI_Comm_rank (MPI_COMM_WORLD, myRank);

     MPI_Comm_size(MPI_COMM_WORLD, &commSize);
     if( commSize < 2 )
     {
        return false;
     }
     else
     {
        return true;
     }
}

FactorerCommunicatorInterface *InitCommunicator(int argc, char ** argv)
{
    FactorerCommunicatorInterface *retInterface = nullptr;
    if( argc > 1 )
    {
        if( strcmp(argv[1],"cmd") == 0)
        {
            retInterface = new CmdFactorerCommunicator;
        }
    }
    else
    {
        retInterface = new MySQLFactorerCommunicator("156.17.235.48","3306","projekt","projekt","factorDB");
    }

    return retInterface;
}
