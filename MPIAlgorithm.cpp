#include "MPIAlgorithm.h"

MPIAlgorithm::MPIAlgorithm(const std::string& number, int base )
{
    mpz_init(_value);
    mpz_set_str(_value, number.c_str(), base);
}

std::string MPIAlgorithm::getString(mpz_t number)
{
    char * buff = new char[mpz_sizeinbase(number, 10) + 2];
    if( buff == nullptr )
        return std::string();

    mpz_get_str(buff,10,_value);
    std::string ret(buff);
    delete buff;
    return ret;
}
