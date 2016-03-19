#include "MPIAlgorithm.h"

MPIAlgorithm::MPIAlgorithm(const char* number, int base )
{
    mpz_init(_value);
    mpz_set_str(_value, number, base);
    maxDigits = mpz_sizeinbase(_value, 10)+2;
}


bool MPIAlgorithm::getString(mpz_t number, char *ptr, size_t ptrLen  ) const
{
    if( mpz_sizeinbase (number, 10) + 2 > ptrLen )
        return false;

    mpz_get_str(ptr,10,number);
    return true;
}

bool MPIAlgorithm::getString(mpz_t number, char *ptr ) const
{
    return getString(number,ptr,getMaxDigits());
}
