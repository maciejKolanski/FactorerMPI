#ifndef FACTORERCOMMUNICATORINTERFACE_H
#define FACTORERCOMMUNICATORINTERFACE_H
#include <string>
#include <vector>
#include <exception>
#include <string>

#include "MPIAlgorithm.h"

enum CommunicatorCommand{Quit, Algorithm};

class FactorerCommunicatorInterface
{
public:
    virtual ~FactorerCommunicatorInterface() {};

    virtual CommunicatorCommand getCommand(MPIAlgorithm::AlgorithmsEnum &algorithm, std::string &number ) = 0;
    virtual void algorithmFinnished(const std::vector<std::string> &result) = 0;

    static std::string commandAsString(CommunicatorCommand command)
    {
        std::string ret;

        switch( command )
        {
            case Quit:
                ret = "Quit";
                break;
            case Algorithm:
                ret = "Algorithm";
                break;
        }

    return ret;
}
};

class FactorerCommunicatorException : public std::exception
{
public:
    FactorerCommunicatorException(const std::string &what) noexcept : m_what(what){}
    const char* what() const noexcept{ return m_what.c_str();}

private:
    std::string m_what;
};

#endif // FACTORERCOMMUNICATORINTERFACE_H
