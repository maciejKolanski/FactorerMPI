#include "BruteForceAlgorithm.h"
#include <string>

std::vector<std::string> BruteForceAlgorithm::Master(const char* value)
{
    logger.write("Master brute force initalizing number to factor...");
    SetValue(value,DEFAULT_BASE);

    std::vector<std::string> ret;
    int tasksNumber;
    char *buff = new char[getMaxDigits()];
    MPI_Comm_size(MPI_COMM_WORLD,&tasksNumber);
    MPI_Status status;

    getString(_value, buff);
    logger << "Master brute force algorithm initialized with value: " << buff << "\n";

    mpz_t step, current,sqrt_value;
    mpz_init(step);
    mpz_init(current);
    mpz_init(sqrt_value);

    mpz_sqrt(sqrt_value,_value);
    mpz_set_ui(current,2);
    if( mpz_cmp_ui(sqrt_value,tasksNumber) <= 0 )
    {
        int tmp = mpz_get_ui(sqrt_value)-1;
        tasksNumber = (tmp > 1) ? tmp : 2;
    }
    mpz_cdiv_q_ui(step,sqrt_value,tasksNumber-1);

    std::vector<int> freeSlaves;
    initSlaves(freeSlaves,tasksNumber);

    logger.write("Master brute force slaves initialized");

    while( mpz_cmp(current,sqrt_value) <= 0 || freeSlaves.size() != tasksNumber-1 )
    {
        while( freeSlaves.size() != 0 && mpz_cmp(current,sqrt_value) <= 0 )
        {
            getString(current,buff);
            MPI_Send(buff, getMaxDigits(), MPI_CHAR, *(freeSlaves.end()-1), FIRSTNUM_TAG, MPI_COMM_WORLD);
            logger << "Master brute force, value " << buff
                        << " sent as first number to slave" << *(freeSlaves.end()-1) << "\n";

            mpz_add(current,current,step);

            getString(current,buff);
            MPI_Send(buff, getMaxDigits(), MPI_CHAR, *(freeSlaves.end()-1), SECNUM_TAG, MPI_COMM_WORLD);
            logger << "Master brute force, value " << buff
                        << " sent as second number to slave" << *(freeSlaves.end()-1) << "\n";

            mpz_add_ui(current,current,1);
            freeSlaves.pop_back();
        }

        MPI_Recv(buff,getMaxDigits(),MPI_CHAR,MPI_ANY_SOURCE, MPI_ANY_TAG,
            MPI_COMM_WORLD, &status);

        switch(status.MPI_TAG)
        {
            case FIRSTRES_TAG:
                ret.push_back(std::string(buff));
                logger << "Master brute force received from " << status.MPI_SOURCE
                        << " value " << buff << " as first result\n";

                MPI_Recv(buff,getMaxDigits(),MPI_CHAR,status.MPI_SOURCE,SECRES_TAG,
                        MPI_COMM_WORLD, &status);
                ret.push_back(std::string(buff));

                logger << "Master brute force received from " << status.MPI_SOURCE
                        << " value " << buff << " as second result\n";
                break;
            case FINNISHED_TAG:
                freeSlaves.push_back(status.MPI_SOURCE);
                logger << "Master brute force received finnish tag from" << status.MPI_SOURCE << "\n";
                break;
        }
    }


    for( int rank = 1; rank < tasksNumber; ++rank )
        MPI_Send(0, 0, MPI_INT, rank, EMPTYTAG, MPI_COMM_WORLD);

    logger << "Master brute force finnished\n";
    delete buff;
    return ret;
}

int BruteForceAlgorithm::Slave()
{
    MPI_Status status;
    char *buff;
    int myrank, setValueSize;
    MPI_Comm_rank (MPI_COMM_WORLD, &myrank);        /* get current process id */
    mpz_t left, right;
    mpz_init(left);
    mpz_init(right);


    MPI_Recv(&setValueSize,1,MPI_INT,0, SETVALUESIZE_TAG,
            MPI_COMM_WORLD, &status);
    buff = new char[setValueSize];

    MPI_Recv(buff,setValueSize,MPI_CHAR,0, SETVALUE_TAG,
            MPI_COMM_WORLD, &status);
    SetValue(buff,DEFAULT_BASE);

    while(true)
    {
        MPI_Recv(buff,getMaxDigits(),MPI_CHAR,0, MPI_ANY_TAG,
            MPI_COMM_WORLD, &status);

        switch(status.MPI_TAG)
        {
            case FIRSTNUM_TAG:
                mpz_set_str(left,buff,base);
                break;
            case SECNUM_TAG:
                mpz_set_str(right,buff,base);
                runSlaveAlgorithm( left, right );
                MPI_Send(buff,getMaxDigits(),MPI_CHAR,0,FINNISHED_TAG,MPI_COMM_WORLD);
                break;
            case EMPTYTAG:
            case DIETAG:
                delete buff;
                return status.MPI_TAG;
        }
    }
}

void BruteForceAlgorithm::runSlaveAlgorithm(mpz_t left, mpz_t right)
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


void BruteForceAlgorithm::initSlaves(std::vector<int>& freeSlaves, size_t numberOfSlaves )
{
    size_t vSize = getMaxDigits();
    char *valueBuff = new char[vSize];
    AlgorithmsEnum algoE = BruteForce;
    getString(_value,valueBuff);

    for(size_t i = 1; i < numberOfSlaves; ++i )
    {
        MPI_Send(&algoE, 1,MPI_INT, i,MPIAlgorithm::SETALGO_TAG,MPI_COMM_WORLD);
        MPI_Send(&vSize,1, MPI_INT,i, SETVALUESIZE_TAG, MPI_COMM_WORLD);
        MPI_Send(valueBuff,getMaxDigits(),MPI_CHAR,i,SETVALUE_TAG,MPI_COMM_WORLD);
        freeSlaves.push_back(i);
    }
    delete valueBuff;
}
