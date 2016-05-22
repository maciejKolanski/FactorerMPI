#include "CmdFactorerCommunicator.h"

CmdFactorerCommunicator::CmdFactorerCommunicator()
{
    std::cout << "Uruchomiono w trybie cmd\n Dostepne komendy:\n\tbruteforce(ENTER)liczba\n\tcfrac(ENTER)liczba" <<
    "\n\tquit\n";
}


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
        else if(commandStr.compare("cfrac") == 0)
        {
            algorithm = MPIAlgorithm::Cfrac;
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
    std::cout << "Returned: ";
    for( size_t i = 1; i < result.size(); i += 2 )
        std::cout << result[i-1] << " * " << result[i] << " * " ;
    if( result.size() == 0 )
        std::cout << "To jest liczba pierwsza!\n";
}
