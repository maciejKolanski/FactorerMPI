#ifndef BRUTEFORCEALGORITHM_H
#define BRUTEFORCEALGORITHM_H
#include <mpi/mpi.h>
#include <gmp.h>

#include "MPIAlgorithm.h"


class BruteForceAlgorithm : public MPIAlgorithm
{
    public:
        ~BruteForceAlgorithm(){};

    private:
        std::vector<std::string> Master(const char* value) override;
        int Slave() override;
        void runSlaveAlgorithm(mpz_t left, mpz_t right);
        void initSlaves(std::vector<int>& freeSlaves, size_t numberOfSlaves );

        enum Tags{ SETVALUE_TAG=10, SETVALUESIZE_TAG,FIRSTNUM_TAG, SECNUM_TAG, FIRSTRES_TAG, SECRES_TAG, FINNISHED_TAG };
};

#endif // BRUTEFORCEALGORITHM_H
