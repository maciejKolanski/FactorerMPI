#include "CommSocket.h"
#include <iostream>

int main()
{
    CommSocket *comm = new CommSocket(8050);

    char data[64]="";
    char exit[64]="exit";

    while(true)
    {
        if(comm->my_receive())
            break;

        std::cin.getline(data, sizeof(data));

        if(*data == *exit)
            break;

        if(comm->my_send(data))
            break;

    }

    delete comm;

    return 0;

}
