#ifndef LOGGER_H
#define LOGGER_H
#include <string>
#include <iostream>

class Logger
{
    public:
        enum MessageType{Info, Fatal};

        void write(const std::string &message, MessageType messageType = MessageType::Info);
        Logger& operator<<(const char *text);
        Logger& operator<<(int value);

    private:
};

#endif // LOGGER_H
