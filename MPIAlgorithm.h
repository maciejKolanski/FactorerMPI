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

        std::string getString(mpz_t number);
};

#endif // MPIALGORITHM_H
