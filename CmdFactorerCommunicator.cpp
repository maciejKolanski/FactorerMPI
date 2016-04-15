#include "CmdFactorerCommunicator.h"

CommunicatorCommand CmdFactorerCommunicator::getCommand(MPIAlgorithm::AlgorithmsEnum &algorithm, std::string &number)
{
    std::string commandStr;
    CommunicatorCommand communicatorCommand = Quit;
    bool readState = false;

    do{
        readState = true;
        getline(std::cin, commandStr);

        if(commandStr.compare("bruteforce") == 0)
        {
            algorithm = MPIAlgorithm::BruteForce;
            getline(std::cin, number);
            communicatorCommand = Algorithm;
        }
        else if(commandStr.compare("quit") == 0)
        {
            communicatorCommand = Quit;
        }
        else
        {
            readState = false;
        }
    }while(readState == false);

    return communicatorCommand;
}

void CmdFactorerCommunicator::algorithmFinnished(const std::vector<std::string> &result)
{
    for( size_t i = 1; i < result.size(); i += 2 )
        std::cout << "Returned: " << result[i-1] << " * " << result[i] << "\n";
    if( result.size() == 0 )
        std::cout << "To jest liczba pierwsza!\n";
}
