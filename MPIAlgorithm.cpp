#include "MPIAlgorithm.h"

MPIAlgorithm::MPIAlgorithm(const std::string& number, int base )
{
    mpz_init(_value);
    mpz_set_str(_value, number.c_str(), base);
}

bool MPIAlgorithm::getString(mpz_t number, char *ptr, size_t ptrLen  )
{
    if( mpz_sizeinbase (number, 10) + 2 > ptrLen )
        return false;

    mpz_get_str(ptr,10,number);
    return true;
}
