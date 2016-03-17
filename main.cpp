#include <iostream>
#include <string.h>
#include <mpi.h>

#include "BruteForceAlgorithm.h"

int main(int argc, char** argv)
{
  int myrank;

  MPI_Init (&argc, &argv);      /* starts MPI */
  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);        /* get current process id */

  MPIAlgorithm *algo = new BruteForceAlgorithm("21");
  std::vector<std::string> returned;

  if (myrank == 0 )
  {
    returned = algo->Master();
  }
  else
  {
    algo->Slave();
  }

  MPI_Finalize();
  delete algo;

  for( auto& s : returned )
    std::cout << "Returned: " << s << "\n";
  return 0;
}

