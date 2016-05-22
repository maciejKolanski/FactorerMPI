#include "BruteForceAlgorithm.h"
#include <string>
using namespace std;
namespace{
    vector<string> simpleBruteForce(int n)
    {
        for(int i=2; i<n; i++)
        {
            if (n%i == 0)
                return {to_string(i), to_string(n/i)};
        }
        return vector<string>();
    }
}

vector<string> BruteForceAlgorithm::masterForValue(const char* value, std::vector<int>& freeSlaves)
{
    SetValue(value);
    MPI_Status status;
    vector<string> ret;
    char *buff = new char[getMaxDigits()];
    int slavesNumber = freeSlaves.size();
    if(mpz_cmp_ui(_value, 20) <= 0)
        return simpleBruteForce(mpz_get_si(_value));


    logger.write("Master initializing mpz");
    mpz_t step, current,sqrt_value;
    mpz_init(step);
    mpz_init(current);
    mpz_init(sqrt_value);

    mpz_sqrt(sqrt_value,_value);
    mpz_set_ui(current,2);

    setSlavesValueTag(slavesNumber);
    if( mpz_cmp_ui(sqrt_value, slavesNumber) <= 0 )
    {
        int tmp = mpz_get_ui(sqrt_value)-1;
        slavesNumber = (tmp > 1) ? tmp : 2;
    }
    mpz_cdiv_q_ui(step, sqrt_value, slavesNumber);

    logger.write("Master brute force slaves initialized");
;
    bool finished = false;
    while( finished == false || freeSlaves.size() < slavesNumber )
    {
        while( finished == false && freeSlaves.size() > 0 && mpz_cmp(current,sqrt_value) <= 0 )
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
                if(finished == false)
                    ret.push_back(std::string(buff));
                logger << "Master brute force received from " << status.MPI_SOURCE
                        << " value " << buff << " as first result\n";

                MPI_Recv(buff,getMaxDigits(),MPI_CHAR,status.MPI_SOURCE,SECRES_TAG,
                        MPI_COMM_WORLD, &status);
                if(finished == false)
                {
                    ret.push_back(std::string(buff));
                    finished = true;
                }
                logger << "Master brute force received from " << status.MPI_SOURCE
                        << " value " << buff << " as second result\n";
                break;
            case FINNISHED_TAG:
                freeSlaves.push_back(status.MPI_SOURCE);
                logger << "Master brute force received finish tag from" << status.MPI_SOURCE << "\n";
                if(freeSlaves.size() == slavesNumber)
                    finished = true;
                break;
        }
    }

    delete buff;

    return ret;
}

std::vector<std::string> BruteForceAlgorithm::Master(const char* value)
{
    std::vector<std::string> ret;
    int tasksNumber;
    MPI_Comm_size(MPI_COMM_WORLD,&tasksNumber);

    logger.write("Master brute force initalizing slaves...");
    SetValue(value,DEFAULT_BASE);
    std::vector<int> freeSlaves;
    initSlaves(freeSlaves, tasksNumber);

    ret = masterForValue(value, freeSlaves);
    logger.write("Finished first master loop");
    size_t processing = 0;
    while(processing < ret.size())
    {
        logger << "Master algo for " << ret[processing].c_str() << "\n";
        vector<string> r = masterForValue(ret[processing].c_str(),freeSlaves);
        if(r.size()==0)
        {
            processing++;
        }
        else
        {
            vector<string> tmp;
            for(int i = 0; i < ret.size(); ++i)
            {
                if(i == processing)
                {
                    for(string s : r)
                        tmp.push_back(s);
                }
                else
                {
                    tmp.push_back(ret[i]);
                }
            }
            ret = tmp;
        }
    }

    for( int rank = 1; rank < tasksNumber; ++rank )
        MPI_Send(0, 0, MPI_INT, rank, DIETAG, MPI_COMM_WORLD);
    logger << "Master brute force finished\n";

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
            case SETVALUE_TAG:
                SetValue(buff,DEFAULT_BASE);
                break;
            case FIRSTNUM_TAG:
                mpz_set_str(left,buff,base);
                break;
            case SECNUM_TAG:
                mpz_set_str(right,buff,base);
                runSlaveAlgorithm( left, right );
                MPI_Send(buff,getMaxDigits(),MPI_CHAR,0,FINNISHED_TAG,MPI_COMM_WORLD);
                break;
            case EMPTYTAG:
                break;
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
            break;
        }
        mpz_add_ui(left,left,1);
    }
    delete buff;
}


void BruteForceAlgorithm::initSlaves(std::vector<int>& freeSlaves, size_t numberOfSlaves )
{
    size_t vSize = getMaxDigits();
    AlgorithmsEnum algoE = BruteForce;

    for(size_t i = 1; i < numberOfSlaves; ++i )
    {
        MPI_Send(&algoE, 1,MPI_INT, i,MPIAlgorithm::SETALGO_TAG,MPI_COMM_WORLD);
        MPI_Send(&vSize,1, MPI_INT,i, SETVALUESIZE_TAG, MPI_COMM_WORLD);
        freeSlaves.push_back(i);
    }
}

void BruteForceAlgorithm::setSlavesValueTag(size_t numberOfSlaves)
{
    size_t vSize = getMaxDigits();
    char *valueBuff = new char[vSize];
    getString(_value,valueBuff);

    logger << "Setting value from 1 to " << numberOfSlaves << "\n";
    for(size_t i = 1; i <= numberOfSlaves; ++i )
    {
        MPI_Send(valueBuff, getMaxDigits(),MPI_CHAR,i, SETVALUE_TAG, MPI_COMM_WORLD);
    }
    delete valueBuff;

}
