#include "Logger.h"


void Logger::write(const std::string& message, MessageType messageType)
{
    if( messageType == MessageType::Fatal )
    {
        std::cout << "Fatal error: ";
    }
    std::cout << message << std::endl;
}

Logger& Logger::operator<<(const char *text)
{
    std::cout << text;
    return *this;
}

Logger& Logger::operator<<(int value)
{
    std::cout << std::to_string(value);
    return *this;
}
