#include "CommSocket.h"
#include <iostream>

int main()
{
    CommSocket *comm = new CommSocket(8888);

    char data[64]="";

    while(true)
    {
        comm->my_receive();

        std::cin.getline(data, sizeof(data));

        if(data == "exit")
            return 0;

        comm->my_send(data);

    }


    return 0;

}
