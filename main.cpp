#include <iostream>
#include <string.h>
#include <mpi.h>
#include <memory>

#include "Logger.h"
#include "MPIAlgorithmHelper.h"
#include "MySQLFactorerCommunicator.h"
#include "CmdFactorerCommunicator.h"
using namespace std;

vector<string> RunAlgorithm(MPIAlgorithm::AlgorithmsEnum algoEm, const char* valueStr, Logger &logger );
bool Init(int *argc, char ***argv, int *myRank);
FactorerCommunicatorInterface *InitCommunicator(int argc, char ** argv);

int main(int argc, char** argv)
{

    Logger logger;
    int myRank;
    if( Init(&argc, &argv, &myRank) == false )
    {
        logger.write("Program must be run with at least 2 threads. Use mpirun -np 2.", Logger::Fatal);
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

        logger.write("Master main loop begins.");
        do{
            communicatorCommand = communicator->getCommand(algorithm, valueStr);
            logger.write(std::string("Master got command "+
                        FactorerCommunicatorInterface::commandAsString(communicatorCommand)));
            if( communicatorCommand == CommunicatorCommand::Algorithm )
            {
                logger.write(std::string("Running algorithm for " + valueStr));
                auto result = RunAlgorithm(algorithm, valueStr.c_str(), logger);
                logger.write("Algorithm finished");
                communicator->algorithmFinnished(result);
            }
        }while(communicatorCommand != CommunicatorCommand::Quit);

        }
        catch( FactorerCommunicatorException& fExc )
        {
            logger.write(fExc.what(), Logger::Fatal);
        }
        logger.write("Master main loop finnished");

        SendDieMessageToAll();
    }
    else
    {
        SlaveWait(logger);
    }

    MPI_Finalize();
    return 0;
}

vector<string> RunAlgorithm(MPIAlgorithm::AlgorithmsEnum algoE, const char* valueStr, Logger &logger )
{
    MPIAlgorithm *algo = GetAlgorithm(algoE, logger);

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
