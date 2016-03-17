#include "BruteForceAlgorithm.h"

BruteForceAlgorithm::BruteForceAlgorithm(const std::string& value, int base )
    :MPIAlgorithm(value,base)
    {
        mpz_init(_sqrt_value);
        mpz_sqrt(_sqrt_value,_value);
    }

std::vector<std::string> BruteForceAlgorithm::Master()
{
    int tasksNumber;
    MPI_Comm_size(MPI_COMM_WORLD,&tasksNumber);

    mpz_t step, current;
    mpz_init(step);
    mpz_init(current);

    mpz_set_ui(current,2);
    mpz_cdiv_q_ui(step,_sqrt_value,tasksNumber);

    std::vector<int> freeSlaves;
    for(int i = 1; i < tasksNumber; ++i )
        freeSlaves.push_back(i);

    while( mpz_cmp(current,_sqrt_value) < 0 || freeSlaves.size() != tasksNumber-1 )
    {
        while( freeSlaves.size() != 0 && mpz_cmp(current,_sqrt_value) < 0 )
        {
            std::string currentStr(getString(current));
            MPI_Send(currentStr.c_str(), currentStr.size(), MPI_CHAR, *(freeSlaves.end()-1), FIRSTNUM_TAG, MPI_COMM_WORLD);

            mpz_add(current,current,step);
            currentStr(current);
            MPI_Send(currentStr.c_str(), currentStr.size(), MPI_CHAR, *(freeSlaves.end()-1), SECNUM_TAG, MPI_COMM_WORLD);

            mpz_add_ui(current,current,1);
            freeSlaves.pop_back();
        }

        //Receive
    }


    for( int rank = 1; rank < tasksNumber; ++rank )
    {
        MPI_Send(0, 0, MPI_INT, rank, DIETAG, MPI_COMM_WORLD);
    }

    return {getString(_value)};
}

void BruteForceAlgorithm::Slave()
{
    MPI_Status status;
    int myrank;
    MPI_Comm_rank (MPI_COMM_WORLD, &myrank);        /* get current process id */

    gmp_printf("Odebrano %Zd w procesie %d\n",_value, myrank);

    for(;;)
    {
        MPI_Recv(0,0,MPI_CHAR,0, MPI_ANY_TAG,
            MPI_COMM_WORLD, &status);
        if( status.MPI_TAG == DIETAG )
        {
            printf("Wysłano DIETAG do procesu %d\n",myrank);
            return;
        }
    }
}
