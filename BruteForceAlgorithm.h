#ifndef BRUTEFORCEALGORITHM_H
#define BRUTEFORCEALGORITHM_H
#include <mpi/mpi.h>
#include <gmp.h>

#include "MPIAlgorithm.h"


class BruteForceAlgorithm : public MPIAlgorithm
{
    public:
        BruteForceAlgorithm(const std::string& number, int base = 10 );
        ~BruteForceAlgorithm(){};

        std::vector<std::string> Master();
        void Slave();
    private:
        void runAlgorithm(mpz_t left, mpz_t right);

        enum Tags{ DIETAG, FIRSTNUM_TAG, SECNUM_TAG, FIRSTRES_TAG, SECRES_TAG, FINNISHED_TAG };

        mpz_t _sqrt_value;
};

#endif // BRUTEFORCEALGORITHM_H
