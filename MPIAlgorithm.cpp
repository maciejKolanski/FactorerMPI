#include "MPIAlgorithm.h"

MPIAlgorithm::MPIAlgorithm(Logger &a_logger)
    :logger(a_logger),
     maxDigits(0)
    {};

void MPIAlgorithm::SetValue(const char* number, int _base )
{
    if( ! (_base > 1 && _base < 63) )
        _base = 10;

    base = _base;
    mpz_init(_value);
    mpz_set_str(_value, number, base);
    if(maxDigits < (mpz_sizeinbase(_value, base)+2))
        maxDigits = mpz_sizeinbase(_value, base)+2;
    if(maxDigits < 8)
        maxDigits = 8;
}


bool MPIAlgorithm::getString(mpz_t number, char *ptr, size_t ptrLen  ) const
{
    if( mpz_sizeinbase (number, base) + 2 > ptrLen )
        return false;

    mpz_get_str(ptr,base,number);
    return true;
}

bool MPIAlgorithm::getString(mpz_t number, char *ptr ) const
{
    return getString(number,ptr,getMaxDigits());
}


