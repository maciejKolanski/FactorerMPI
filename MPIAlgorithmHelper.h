#ifndef MPIALGORITHMHELPER_H
#define MPIALGORITHMHELPER_H

#include "BruteForceAlgorithm.h"

MPIAlgorithm* GetAlgorithm(MPIAlgorithm::AlgorithmsEnum algorithms);
void SlaveWait();
void SendDieMessageToAll();

#endif
