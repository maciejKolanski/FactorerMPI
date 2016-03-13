#include <iostream>
#include <mpi/mpi.h>

using namespace std;

#define WORKTAG 1
#define DIETAG 2

constexpr int BUFFSIZE = 64;
int buff[BUFFSIZE];

static void master();
static void slave();

int main(int argc, char** argv)
{
  int myrank;
<<<<<<< HEAD
  //MPI_Status status;
=======
  MPI_Status status;
>>>>>>> origin/master

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

static void master()
{
    int ntasks,buff;

    MPI_Comm_size(MPI_COMM_WORLD,&ntasks);
    for( int rank = 1; rank < ntasks; ++rank )
    {
        MPI_Send(&buff,1,MPI_INT,rank,WORKTAG,MPI_COMM_WORLD);
    }

    for( int rank = 1; rank < ntasks; ++rank )
    {
        MPI_Send(&buff,0,MPI_INT,rank, DIETAG, MPI_COMM_WORLD);
    }
}

static void slave()
{
    int work,myrank;
    MPI_Comm_rank (MPI_COMM_WORLD, &myrank);        /* get current process id */

    MPI_Status status;
    for(;;)
    {
        MPI_Recv(&work,1,MPI_INT,0, MPI_ANY_TAG,
            MPI_COMM_WORLD, &status);
        if( status.MPI_TAG == DIETAG )
        {
            printf("Wysłano DIETAG do procesu %d\n",myrank);
            return;
        }
        printf("Wysłano %d do procesu %d\n",work,myrank);

    }
}
