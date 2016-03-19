#ifndef MPIALGORITHM_H
#define MPIALGORITHM_H

#include <gmp.h>
#include <string>
#include <vector>

class MPIAlgorithm
{
    public:
        MPIAlgorithm(const std::string& number, int base = 10);

        virtual ~MPIAlgorithm(){};
        virtual std::vector<std::string> Master() = 0;
        virtual void Slave() = 0;
    protected:
        mpz_t _value;
        static constexpr size_t MAX_DIGITS = 10;

        bool getString(mpz_t number, char *ptr, size_t ptrLen = MAX_DIGITS );
};

#endif // MPIALGORITHM_H
