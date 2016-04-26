#ifndef MPIALGORITHM_H
#define MPIALGORITHM_H

#include <gmp.h>
#include <string>
#include <vector>
#include <mpi.h>
#include "Logger.h"

class MPIAlgorithm
{
    public:
        enum AlgorithmsEnum {None, BruteForce = 2};
        enum Tags {EMPTYTAG,DIETAG,SETALGO_TAG};

        MPIAlgorithm(Logger &a_logger);
        virtual ~MPIAlgorithm(){};
        virtual std::vector<std::string> Master(const char* value) = 0;
        virtual int Slave() = 0;

    protected:
        Logger &logger;
        mpz_t _value;
        size_t base;
        static constexpr int DEFAULT_BASE = 10;

        inline size_t getMaxDigits() const { return maxDigits; };
        void SetValue(const char* number, int base = 10);
        bool getString(mpz_t number, char *ptr ) const;
        bool getString(mpz_t number, char* ptr, size_t ptrLen ) const;

    private:
        size_t maxDigits;
};

#endif // MPIALGORITHM_H
