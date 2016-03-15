#include <iostream>
#include <mpi/mpi.h>
#include <gmp.h>
#include <string.h>

using namespace std;

#define INITVALUETAG 1
#define DIETAG 2

constexpr int MAX_DIGITS = 64;

static void master();
static void slave();

int main(int argc, char** argv)
{
  int myrank;

  MPI_Init (&argc, &argv);      /* starts MPI */
  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);        /* get current process id */

  if (myrank == 0 )
  {
    master();
  }
  else
  {
    slave();
  }


  MPI_Finalize();
  return 0;
}

void master()
{
    int ntasks,buff;
    MPI_Comm_size(MPI_COMM_WORLD,&ntasks);

    char valueStr[] = "124\0";
    mpz_t value,step;
    mpz_init(value);
    mpz_init(step);
    mpz_set_str(value,valueStr,10);

    mpz_cdiv_q_ui(step,value,ntasks);
    gmp_printf("MASTER value/tasksk = step %Zd/%d = %Zd \n",value,ntasks,step);


    for( int rank = 1; rank < ntasks; ++rank )
    {
        MPI_Send(valueStr,strlen(valueStr),MPI_CHAR,rank,INITVALUETAG,MPI_COMM_WORLD);
    }

    for( int rank = 1; rank < ntasks; ++rank )
    {
        MPI_Send(&buff,0,MPI_INT,rank, DIETAG, MPI_COMM_WORLD);
    }
}

void slave()
{
    MPI_Status status;
    int myrank;
    MPI_Comm_rank (MPI_COMM_WORLD, &myrank);        /* get current process id */

    char valueStr[MAX_DIGITS];
    mpz_t value;
    mpz_init(value);

    MPI_Recv(valueStr,MAX_DIGITS,MPI_CHAR,0, INITVALUETAG ,
        MPI_COMM_WORLD, &status);
    mpz_set_str(value,valueStr,10);
    gmp_printf("Odebrano %Zd w procesie %d\n",value,myrank);

    for(;;)
    {
        MPI_Recv(valueStr,MAX_DIGITS,MPI_CHAR,0, MPI_ANY_TAG,
            MPI_COMM_WORLD, &status);
        if( status.MPI_TAG == DIETAG )
        {
            printf("Wysłano DIETAG do procesu %d\n",myrank);
            return;
        }
    }
}
