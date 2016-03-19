#include <iostream>
#include <string.h>
#include <mpi.h>

#include "BruteForceAlgorithm.h"

int main(int argc, char** argv)
{
  int myrank;
  MPI_Init (&argc, &argv);      /* starts MPI */
  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);        /* get current process id */

  MPIAlgorithm *algo = new BruteForceAlgorithm(argv[1]);
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

  for( size_t i = 1; i < returned.size(); i += 2 )
    std::cout << "Returned: " << returned[i-1] << " * " << returned[i] << "\n";
  return 0;
}

