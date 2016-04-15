#ifndef CMDFACTORERCOMMUNICATOR_H
#define CMDFACTORERCOMMUNICATOR_H

#include <iostream>
#include "FactorerCommunicatorInterface.h"


class CmdFactorerCommunicator : public FactorerCommunicatorInterface
{
    public:
        virtual ~CmdFactorerCommunicator() {};
    CommunicatorCommand getCommand(MPIAlgorithm::AlgorithmsEnum &algorithm, std::string &number );
    virtual void algorithmFinnished(const std::vector<std::string> &result);
    private:
};

#endif // CMDFACTORERCOMMUNICATOR_H
