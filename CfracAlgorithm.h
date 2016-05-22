#include <mpi/mpi.h>
#include <gmp.h>

#include "MPIAlgorithm.h"


class CfracAlgorithm : public MPIAlgorithm
{
    public:
        CfracAlgorithm(Logger &a_logger);
        ~CfracAlgorithm(){};

    private:
        //parametr do jakiej wartosci program zdecyduje sie wykonac BruteForce
        int betterbruteforce=1000000000;
        //ilosc iteracji wykonan algorytmu CFRAC dla jednej maszyny
        int n = 100000000;//dla wiekszej ilosci 'for' wywala bledy//alternatywnie mozna zrobic rekurencje
        int limit_k = 10; //ograniczenie resend'ow

        //Master zwraca liste faktorow
        std::vector<std::string> Master(const char* value) override;
        int Slave() override;
        void runSlaveAlgorithm(mpz_t left, mpz_t right);
        void initSlaves(std::vector<int>& freeSlaves, size_t numberOfSlaves );

        //algorytm CFRAC :
        //Argumenty (Liczba dla ktorej szukamy dzielnika,parametr K, Zmienna mpz,w ktorej zapisany bd wynik)
        //Parametr K - liczba calkowita od 1. Dla roznych K algorytm znajduje wyniki w innym czasie
        //CFRAC zwracana wartosc:
        // 0 - dzielnik znaleziony, nadpisany w mpz_t Result
        // 1 - dzielnik nie znaleziony - za mało iteracji lub liczba jest liczbą pierwsza
        //-1 -blad wykonania algorytmu (wystapienie wartosci ujemnej w kodzie)
        int AlgorithmCFRAC(mpz_t val,int k, mpz_t result);

        //Algorytm NWD Euklidesa
        void NWDeuklides(mpz_t a,mpz_t val);
        //Prosty Bruteforce - efektywniejszy dla malych liczb
        int SimpleBruteForceFact(int val);

        enum Tags{ SETVALUE_TAG=10, SETVALUESIZE_TAG,K_PARAM_TAG, FINNISHED_TAG };
};


