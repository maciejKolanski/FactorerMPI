#include "CommSocket.h"

CommSocket::CommSocket(unsigned port_nr)
{
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);

    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }

    int yes = 1;
    if ( setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1 )
    {
        perror("setsockopt");
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( port_nr );

    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        puts("ERROR - Bind failed");
        exit(1);
    }
    else
        puts("bind done");

    listen(socket_desc , 3);

    puts("Waiting for incoming connections...");
    //ctor

    int c = sizeof(struct sockaddr_in);
    new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);

    if (new_socket<0)
    {
        perror("accept failed");
    }
    else
        puts("Connection accepted");

    socket_desc = new_socket;


    recv_buffer = new char[1];
}

CommSocket::~CommSocket()
{
    delete[] recv_buffer;
    close(socket_desc);
}

char* CommSocket::get_recv_buffer()
{
    return recv_buffer;
}

bool CommSocket::my_receive()
{
    unsigned char size_buffer[4];

    bytesReceived = recv(socket_desc, (char*)size_buffer , 4 , 0);

    if(bytesReceived == 0)
        {
            close(socket_desc);
            return 1;
        }

    recv_buffer_size = (size_buffer[0] << 24) | (size_buffer[1] << 16) | (size_buffer[2] << 8 ) | size_buffer[3];

    if(bytesReceived < 0)
    {
        //puts("recv failed");
        //printf("%ld", bytesReceived);
        return 1;
    }
    else
    {
        /*printf("Bytes received %ld \n", bytesReceived);
        printf("Size buffer: %d \n", size_buffer[0]);
        printf("Size buffer: %d \n", size_buffer[1]);
        printf("Size buffer: %d \n", size_buffer[2]);
        printf("Size buffer: %d \n", size_buffer[3]);
        printf("Packet size received : %ld \n", recv_buffer_size);*/
    }

    delete[] recv_buffer;

    recv_buffer = new char[recv_buffer_size];

    char command[8] = "ACCEPTD";

    bytesSent = send(socket_desc, command, 8, 0);

    if(bytesSent < 0)
    {
        //puts("Command send failed");
        return 1;
    }
    else
    {
        //puts("Command send success");
    }

    bytesReceived = recv(socket_desc, recv_buffer, recv_buffer_size, 0);

    if(bytesReceived < 0)
    {
        //puts("recv failed");
        //printf("%ld", bytesReceived);
        return 1;
    }
    else
    {
        //printf("Packet received : %s \n\n", recv_buffer);
    }
    return 0;
}

bool CommSocket::my_send(char* data)
{
    long int packet_length = strlen(data);
    char size_buffer[4];

    size_buffer[3] = packet_length & 0xff;
    size_buffer[2] = (packet_length >> 8)  & 0xff;
    size_buffer[1] = (packet_length >> 16) & 0xff;
    size_buffer[0] = (packet_length >> 24) & 0xff;

    char *buffer =  new char[packet_length];
    strncpy(buffer, data, strlen(data));

    bytesSent = send(socket_desc, size_buffer, 4, 0);

    if(bytesSent < 0)
    {
        //puts("Packet size send failed");
        return 1;
    }
    else
    {
        //printf("Packet size send success\n");
    }

    char command[8];
    bytesReceived = recv(socket_desc, command, 8, 0);

    if(bytesReceived < 0)
    {
        //puts("Command receive failed");
        return 1;
    }
    else
    {
        //printf("Command receive success: %s\n", command);
    }

    bytesSent=send(socket_desc, buffer, packet_length, 0);

    if(bytesSent < 0)
    {
        //puts("Packet send failed");
        return 1;
    }
    else
    {
        //printf("Packet send success\n\n");
    }

    return 0;
}
