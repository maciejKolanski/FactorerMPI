#ifndef MPIALGORITHMHELPER_H
#define MPIALGORITHMHELPER_H

#include "BruteForceAlgorithm.h"

MPIAlgorithm* GetAlgorithm(MPIAlgorithm::AlgorithmsEnum algorithms, Logger &logger);
void SlaveWait(Logger &logger);
void SendDieMessageToAll();

#endif
