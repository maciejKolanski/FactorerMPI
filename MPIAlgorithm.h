#ifndef MPIALGORITHM_H
#define MPIALGORITHM_H

#include <gmp.h>
#include <string>
#include <vector>

class MPIAlgorithm
{
    public:
        MPIAlgorithm(const char* number, int base = 10);

        virtual ~MPIAlgorithm(){};
        virtual std::vector<std::string> Master() = 0;
        virtual void Slave() = 0;
    protected:
        mpz_t _value;

        inline size_t getMaxDigits() const { return maxDigits; };
        bool getString(mpz_t number, char *ptr ) const;
        bool getString(mpz_t number, char* ptr, size_t ptrLen ) const;
    private:
        size_t maxDigits;
};

#endif // MPIALGORITHM_H
