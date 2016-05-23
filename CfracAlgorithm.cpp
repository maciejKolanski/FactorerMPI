#include "CfracAlgorithm.h"
#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <limits.h>

CfracAlgorithm::CfracAlgorithm(Logger &a_logger)
    :MPIAlgorithm(a_logger)
    {}

std::vector<std::string> CfracAlgorithm::Master(const char* value)
{

    SetValue(value,DEFAULT_BASE); //zmienna _value

    mpz_t result,result2,rest,current;
    mpz_init(result);
    mpz_init(result2);
    mpz_init(rest);
    mpz_init(current);

    std::vector<std::string> ret; // wektor na wynik

    //tablica stringow jako kolejka na faktory
    //vector<mpz_t> numberQue; impossibru
    std::vector<std::string> numberQue;

    int tasksNumber;//liczba procesow

    char *buff = new char[getMaxDigits()];
    MPI_Comm_size(MPI_COMM_WORLD,&tasksNumber);
    MPI_Status status;

    size_t vSize;
    char* valueBuff;
    int k = 1;
    int setValueSize;
    int scalar = 1;
    int odbior;
    bool found = false;
    bool lastprimefound = false;
    int primes;

    logger << "Uruchomiono Algorytm Cfrac na " << tasksNumber <<" watkach" << "\n";


    //inicjalizacja Slaves
    std::vector<int> freeSlaves;

    //Poczatkowa wartosc do kolejki
    mpz_get_str(buff,10,_value);
    numberQue.push_back(buff);

    while (numberQue.size() != 0)
    {
        while(!lastprimefound)
        {
            //podglad kolejek
            /*printf("Zawartosc NumberQUE: ");
            for( size_t i = 0; i < numberQue.size(); i ++ )
                std::cout<<" "<<numberQue[i];
            printf("|\n");

            printf("Zawartosc return: ");
            for( size_t i = 0; i < ret.size(); i ++ )
                std::cout<<" "<<ret[i];
            printf("|\n");*/
            vSize = getMaxDigits();
            valueBuff = new char[vSize];
            valueBuff = (char*)numberQue.back().c_str();
            mpz_set_str(current,(char*)numberQue.back().c_str(),10);

            if (mpz_cmp_ui(current,betterbruteforce) > 0) // dla duzych wartosci CFRAC
            {
                initSlaves(freeSlaves,tasksNumber);
                while(freeSlaves.size() != 0)
                {
                    //Rozeslij zadania dla kazdego Slave'a

                    // - - - - - - - - - - - Wysylanie danych - - - - - - - - - - - -//
                    //Wyslij rozmiar liczby
                    MPI_Send(&vSize,1,MPI_INT,*(freeSlaves.end()-1),SETVALUESIZE_TAG,MPI_COMM_WORLD);
                    //Wyslij wartosc liczby
                    MPI_Send(valueBuff,getMaxDigits(),MPI_CHAR,*(freeSlaves.end()-1),SETVALUE_TAG,MPI_COMM_WORLD);
                    //Wyslij parametr K
                    MPI_Send(&k,1,MPI_INT,*(freeSlaves.end()-1),K_PARAM_TAG,MPI_COMM_WORLD);
                    k++;
                    //Usun z kolejki
                    freeSlaves.pop_back();
                    ///////////////////////////////////////////////////////////////////
                }
                numberQue.pop_back();
                k=1;
                odbior=0;
                found = false;
                if(scalar>limit_k)
                    primes = 99999;
                else
                    primes =0;

                while(odbior !=tasksNumber-1)
                {
                    odbior++;

                    //odbierz wynik
                    MPI_Recv(&setValueSize,1,MPI_INT,MPI_ANY_SOURCE, SETVALUESIZE_TAG,MPI_COMM_WORLD, &status);
                    buff = new char[setValueSize];
                    MPI_Recv(buff,setValueSize+2,MPI_CHAR,status.MPI_SOURCE, SETVALUE_TAG,MPI_COMM_WORLD, &status);

                    if(found) continue;

                    mpz_set_str(result,buff,10);//string to mpz

                    if (mpz_cmp_ui(result,1) == 0 || primes == 99999)//jezeli liczba pierwsza
                    {
                        primes++;
                        if(primes>tasksNumber)
                        {
                            //printf("Wszystkie wyniki zwrocily 1 - Wrzucam jako liczba pierwsza\n");
                            mpz_get_str(buff,10,current);
                            ret.push_back(buff);
                            found = true;
                            if(numberQue.size() == 0) lastprimefound=true;
                        }
                    }

                    else if(mpz_cmp_ui(result,1) > 0)//prawdlowy wynik
                    {
                        mpz_cdiv_qr(result2,rest,current,result);
                        if(mpz_cmp_ui(rest,0) == 0 && mpz_cmp(result,current)!=0)//sprawdzenie poprawnosci
                        {
                            scalar=1;
                            mpz_get_str(buff,10,result);
                            numberQue.push_back(buff);
                            mpz_get_str(buff,10,result2);
                            numberQue.push_back(buff);
                            found = true;
                        }
                        else
                        {

                        }
                    }

                    //jezeli zadna maszyna nie znalazla poprawnego wyniku
                    if(odbior == tasksNumber-1 && !found)
                    {
                        mpz_get_str(buff,10,current);
                        numberQue.push_back(buff);
                        //zmiana parametru
                        k=(tasksNumber+1) * scalar;
                        scalar++;
                    }

                }
            }
            else //dla malych wartosci uzyj brutforc
            {
                vSize = getMaxDigits();
                valueBuff = new char[vSize];
                valueBuff = (char*)numberQue.back().c_str();
                numberQue.pop_back();

                int val = atoi(valueBuff);
                int _val=val;
                val = SimpleBruteForceFact(val);

                if( val != -1)// dzielnik znaleziony
                {
                    char u1[(((sizeof _val) * CHAR_BIT) + 2)/3 + 2];
                    _val = _val/val;
                    sprintf(u1,"%i",_val);
                    numberQue.push_back(std::string(u1));

                    char u2[(((sizeof val) * CHAR_BIT) + 2)/3 + 2];
                    sprintf(u2,"%i",val);
                    numberQue.push_back(std::string(u2));

                    break;
                }
                else //liczba pierwsza
                {
                    char u1[(((sizeof _val) * CHAR_BIT) + 2)/3 + 2];
                    sprintf(u1,"%d",_val);
                    ret.push_back(u1);
                    //numberQue.pop_back();
                    break;
                }
            }
        }
    }

    for( size_t i = 0; i < ret.size(); i ++ )
        std::cout << "Returned: " << ret[i] << " * \n";


    for( int rank = 1; rank < tasksNumber; ++rank )
        MPI_Send(0, 0, MPI_INT, rank, EMPTYTAG, MPI_COMM_WORLD);


    delete buff;
    return ret;

    mpz_clear(result);
    mpz_clear(result2);
    mpz_clear(rest);
    mpz_clear(current);
}

int CfracAlgorithm::Slave()
{

    MPI_Status status;
    char *buff;
    int myrank, setValueSize;
    MPI_Comm_rank (MPI_COMM_WORLD, &myrank);        /* get current process id*/
    mpz_t startVal,result;
    mpz_init(startVal);
    mpz_init(result);
    int k;
    size_t vSize;
    char* valueBuff;


    //rozmiar liczby
    MPI_Recv(&setValueSize,1,MPI_INT,0, SETVALUESIZE_TAG,MPI_COMM_WORLD, &status);
    buff = new char[setValueSize];
    //wartosc liczby
    MPI_Recv(buff,setValueSize,MPI_CHAR,0, SETVALUE_TAG,MPI_COMM_WORLD, &status);
    //SetValue(buff,DEFAULT_BASE);
    //parametr K
    MPI_Recv(&k,1,MPI_INT,0, K_PARAM_TAG,MPI_COMM_WORLD, &status);
    //wartosc do mpz startVal
    mpz_set_str(startVal, buff, base);

    //wykonaj algorytm
    int _result = AlgorithmCFRAC(startVal, k, result);


    //gmp_printf("Jestem slave %d :::  wartosc: %Zd, dla k: %d \n",myrank,result,k);
    //while(1); //test


    vSize = mpz_sizeinbase(result, 10)+2;

    valueBuff = new char[vSize];

    //gmp_printf("SLAVE_NR:_%d_Result : %Zd, intRes=%d",myrank,result,_result);

    switch(_result)
    {
    //znaleziony wynik
    case 0:
        mpz_get_str(valueBuff,base,result);
        break;

    //przekroczony czas lub liczba pierwsza
    case 1:
        valueBuff = "1";
        break;

    //blad wykonania algorytmu
    case -1:
        valueBuff = "-1";
        break;
    }


    //printf("Slave %d : Vsize: %d \n",myrank,vSize);
    vSize = mpz_sizeinbase(result, 10)+2;

    //
    MPI_Send(&vSize,1,MPI_INT,0,SETVALUESIZE_TAG,MPI_COMM_WORLD);
    MPI_Send(valueBuff,vSize,MPI_CHAR,0,SETVALUE_TAG,MPI_COMM_WORLD);


    //czyscimy
    delete buff;
    delete valueBuff;
    mpz_clear(startVal);
    mpz_clear(result);

    return status.MPI_TAG;
}


void CfracAlgorithm::initSlaves(std::vector<int>& freeSlaves, size_t numberOfSlaves )
{
    AlgorithmsEnum algoE = Cfrac;

    for(size_t i = 1; i < numberOfSlaves; ++i )
    {
        MPI_Send(&algoE,1,MPI_INT,i,MPIAlgorithm::SETALGO_TAG,MPI_COMM_WORLD);
        freeSlaves.push_back(i);
    }
}

int CfracAlgorithm::AlgorithmCFRAC(mpz_t StartValue, int k, mpz_t result)
{
    mpz_mul_ui(StartValue,StartValue,k);

    // 0 < Q < 2*sqrt(k * MaxVal)
    // 0 < r < 2*sqrt(k * MaxVal)
    // 0 < A < MaxVal

    mpz_t *Q,*r,*A,*G,temp_mpz,*fractions;
    mpf_t temp,tempINT;
    Q = new mpz_t[n];
    A = new mpz_t[n];
    r = new mpz_t[n];
    G = new mpz_t[n];
    fractions = new mpz_t[n];
    mpz_init(temp_mpz);
    mpf_init2(temp,1000);
    mpf_init(tempINT);


    mpf_t FloatValue,IntValue;


    for (int i=0; i<n ; i++)
    {
        //mpz_inits(A[i],Q[i],r[i]); //wywala Segment fault
        mpz_init(A[i]);
        mpz_init(Q[i]);
        mpz_init(r[i]);
        mpz_init(G[i]);
        mpz_init(fractions[i]);
        // inicjalizacja zmiennych dla pierwszego obiegu petli
        if ( i == 0 )
        {
            //Q0 = 1
            mpz_set_ui(Q[0],1);

            //A0 = fraction[0] = [sqrt(N)]
            mpf_init2(FloatValue,10000); //duza precyzja dla poprawnych wynikow ulamkow
            mpf_init(IntValue);
            mpf_set_z(FloatValue,StartValue); //zapis wartosci calkowtiej do zmiennej zmnp.
            mpf_sqrt(FloatValue,FloatValue);  //Pierwiastek z FloatValue zapisany do FloatValue
            mpf_floor(IntValue,FloatValue); //rozdzielenie liczby na czesc ulamkowa i calkowita
            mpz_set_f(fractions[0],IntValue);

            mpz_set_f(A[0],IntValue);

            //G[0] = 2*frac[0] - r[-1] = frac[0]
            mpz_set(G[0],fractions[0]);


            //r0 = G[0] - frac[0]*Q[0]
            mpz_mul(r[0],Q[0],fractions[0]);
            mpz_sub(r[0],G[0],r[0]);
        }
        else //dla i >0
        {
            //G[i] = 2[sqrt(N)] - r[i-1]
            mpz_add(G[i],G[i],fractions[0]);
            mpz_add(G[i],G[i],fractions[0]);
            mpz_sub(G[i],G[i],r[i-1]);


            //Qi = Q[i-2] + frac[i-1]*(r[i-1] - r[i-2)
            if (i>1)
                mpz_sub(Q[i],r[i-1],r[i-2]);
            else
                mpz_sub(Q[i],r[i-1],fractions[0]);//dla i=1
            mpz_mul(Q[i],Q[i],fractions[i-1]);
            if (i>1)
                mpz_add(Q[i],Q[i],Q[i-2]);
            else
                mpz_add(Q[i],Q[i],StartValue);//dla i=1

            //frac[i] = [G[i]/Q[i]]
            if(mpz_cmp_ui(Q[i],0) != 0)
                mpz_fdiv_q(fractions[i],G[i],Q[i]);
            else
                mpz_set_ui(Q[i],0);
            //fractions[i] = mpz_get_ui(temp_mpz);


            //r[i] =G[i] - frac[i]*Q[i]
            mpz_mul(r[i],Q[i],fractions[i]);
            mpz_sub(r[i],G[i],r[i]);

            //Ai = MOD(frac[i]*A[-1] + A[-2] ; number_to_factor)
            mpz_mul(A[i],A[i-1],fractions[i]);
            if (i>1)
                mpz_add(A[i],A[i],A[i-2]);
            else
                mpz_add_ui(A[i],A[i],1);
            mpz_mod(A[i],A[i],StartValue);
        }

        if (i>1)
        {
            //biezace czyszczenie
            mpz_clear(A[i-2]);
            mpz_clear(Q[i-2]);
            mpz_clear(r[i-2]);
            mpz_clear(G[i-2]);
            if (i-2 != 0)
                mpz_clear(fractions[i-2]);
        }

        // podglad :)
        //gmp_printf(" !!!index : %d :::  \nQ[%d] = %Zd   | \nA[%d] = %Zd   | \nr[%d] = %Zd   \nG[%d] = %Zd   \nfrac[%d] = %Zd   \n",i,i,Q[i],i,A[i],i,r[i],i,G[i],i,fractions[i]);



        //wartosc ujemna nie powinna nigdy wystapic
        if (mpz_sgn(Q[i]) < 0)
        {
            //printf("wartosc ujemna ! blad \n");
            //mpz_neg(Q[i],Q[i]);
            return -1;
            //break;
        }


        if ( i > 0 && mpz_sgn(Q[i]) >= 0 )
        {
            //jezeli mozliwe sqrt(Q[i]) to drukuj prawdopodobny wynik
            mpf_set_z(temp,Q[i]);
            //gmp_printf("%d :      FULL :  %.*Ff \n" , i , 5,temp);
            mpf_sqrt(temp,temp);
            //gmp_printf("SQRT:  %.*Ff " , 3,temp);
            mpf_floor(tempINT,temp); //rozdzielenie liczby na czesc ulamkowa i calkowita
            //gmp_printf("INT:  %.*Ff " , 3,temp);
            mpf_sub(temp,temp,tempINT);
            //gmp_printf("Float:  %.*Ff \n\n " , 5,temp);
            if ( mpf_cmp_ui(temp,0) == 0 && mpz_cmp(A[i-1],Q[i]) > 0)
            {
                mpz_sqrt(temp_mpz,Q[i]);
                mpz_sub(temp_mpz,A[i-1],temp_mpz);
                //gmp_printf("index %d : numbah : %Zd \n", i,temp_mpz);
                //getchar();
                NWDeuklides(temp_mpz,StartValue);
                //gmp_printf("dzielnik : %Zd \n , K: %d", temp_mpz,k);
                mpz_set(result,temp_mpz);
                return 0;
                //getchar();
            }
        }
    }
    return 1;
}

void CfracAlgorithm::NWDeuklides(mpz_t a, mpz_t val) // a = result
{
    mpz_t c,b;//temps
    mpz_init(c);
    mpz_init(b);
    mpz_set(b,val);
    while (mpz_cmp_ui(b,0) != 0)
    {
        mpz_mod(c,a,b);
        mpz_set(a,b);
        mpz_set(b,c);
    }
    mpz_clear(c);
    mpz_clear(b);
}

int CfracAlgorithm::SimpleBruteForceFact(int val)
{
    for(int i=2; i<val; i++)
    {
        if (val%i == 0)
            return i;
    }
    return -1;
}

