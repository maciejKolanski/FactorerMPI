#ifndef COMMSOCKET_H
#define COMMSOCKET_H

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>

#include <sys/socket.h>
#include <arpa/inet.h>

class CommSocket
{
    private:

    int socket_desc , new_socket;
    struct sockaddr_in server , client;

    long int bytesSent, bytesReceived, recv_buffer_size;

    public:
         CommSocket(unsigned);
        ~CommSocket();

        void my_receive();
        void my_send(char*);

};

#endif // COMMSOCKET_H
