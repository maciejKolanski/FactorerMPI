#include "BruteForceAlgorithm.h"

BruteForceAlgorithm::BruteForceAlgorithm(const char* value, int base )
    :MPIAlgorithm(value,base)
    {
        mpz_init(_sqrt_value);
        mpz_sqrt(_sqrt_value,_value);
    }

std::vector<std::string> BruteForceAlgorithm::Master()
{
    std::vector<std::string> ret;
    int tasksNumber;
    char buff[getMaxDigits()];
    MPI_Comm_size(MPI_COMM_WORLD,&tasksNumber);
    MPI_Status status;

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
            getString(current,buff);
            MPI_Send(buff, getMaxDigits(), MPI_CHAR, *(freeSlaves.end()-1), FIRSTNUM_TAG, MPI_COMM_WORLD);

            mpz_add(current,current,step);

            getString(current,buff);
            MPI_Send(buff, getMaxDigits(), MPI_CHAR, *(freeSlaves.end()-1), SECNUM_TAG, MPI_COMM_WORLD);

            mpz_add_ui(current,current,1);
            freeSlaves.pop_back();
        }

        MPI_Recv(buff,getMaxDigits(),MPI_CHAR,MPI_ANY_SOURCE, MPI_ANY_TAG,
            MPI_COMM_WORLD, &status);

        switch( status.MPI_TAG)
        {
            case FIRSTRES_TAG:
                ret.push_back(std::string(buff));
                MPI_Recv(buff,getMaxDigits(),MPI_CHAR,status.MPI_SOURCE,SECRES_TAG,
                        MPI_COMM_WORLD, &status);
                ret.push_back(std::string(buff));
                break;
            case FINNISHED_TAG:
                freeSlaves.push_back(status.MPI_SOURCE);
                break;
        }
    }


    for( int rank = 1; rank < tasksNumber; ++rank )
    {
        MPI_Send(0, 0, MPI_INT, rank, DIETAG, MPI_COMM_WORLD);
    }

    return ret;
}

void BruteForceAlgorithm::Slave()
{
    MPI_Status status;
    char buff[getMaxDigits()];
    int myrank;
    MPI_Comm_rank (MPI_COMM_WORLD, &myrank);        /* get current process id */
    mpz_t left, right;
    mpz_init(left);
    mpz_init(right);

    while(true)
    {
        MPI_Recv(buff,getMaxDigits(),MPI_CHAR,0, MPI_ANY_TAG,
            MPI_COMM_WORLD, &status);

        switch(status.MPI_TAG)
        {
            case FIRSTNUM_TAG:
                mpz_set_str(left,buff,10);
                break;
            case SECNUM_TAG:
                mpz_set_str(right,buff,10);
                runAlgorithm( left, right );
                MPI_Send(buff,getMaxDigits(),MPI_CHAR,0,FINNISHED_TAG,MPI_COMM_WORLD);
                break;
            case DIETAG:
                printf("Wysłano DIETAG do procesu %d\n",myrank);
                return;
        }
    }
}

void BruteForceAlgorithm::runAlgorithm(mpz_t left, mpz_t right)
{
    mpz_t mod, q;
    mpz_init(mod);
    mpz_init(q);
    char *buff = new char[getMaxDigits()];

    while( mpz_cmp(left,right) <= 0 )
    {
        mpz_cdiv_qr(q,mod,_value,left);
        if(  mpz_cmp_ui(mod,0) == 0 )
        {
            getString(left,buff);
            MPI_Send(buff,getMaxDigits(),MPI_CHAR,0,FIRSTRES_TAG,MPI_COMM_WORLD);

            getString(q,buff);
            MPI_Send(buff,getMaxDigits(),MPI_CHAR,0,SECRES_TAG,MPI_COMM_WORLD);
        }
        mpz_add_ui(left,left,1);
    }
    delete buff;
}
