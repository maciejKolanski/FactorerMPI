#include <iostream>
#include <string.h>
#include <mpi.h>
#include <pthread.h>

#include "MPIAlgorithmHelper.h"
#include "CommSocket.h"

pthread_mutex_t	command_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t number_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t task_flag_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t ready_flag_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t result_flag_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t result_mutex = PTHREAD_MUTEX_INITIALIZER;

bool new_task_flag = false, ready_flag = true, result_flag = false;
char command[8]="", number[256]="";
std::vector <std::string> result;

void RunAlgorithm(MPIAlgorithm::AlgorithmsEnum algoEm, const char* valueStr);
void* main_comm(void*);

int main(int argc, char** argv)
{
     MPI_Init (&argc, &argv);
     int myrank;
     MPI_Comm_rank (MPI_COMM_WORLD, &myrank);

    if( myrank == 0 )
    {
//----------------------------------------------------------------------------------------------------
        pthread_t thread_comm;

        if(pthread_create(&thread_comm, NULL, main_comm, NULL))
            std::cout<<"Thread comm creation failed.";

        while(true)
        {

            if(new_task_flag)
            {
                std::cout<<"Rozpoczeto algorytm"<<std::endl;

                pthread_mutex_lock(&ready_flag_mutex);
                ready_flag = false;

                pthread_mutex_unlock(&ready_flag_mutex);

                pthread_mutex_lock(&task_flag_mutex);
                new_task_flag = false;
                pthread_mutex_unlock(&task_flag_mutex);

                pthread_mutex_lock(&command_mutex);
                std::cout<<"Command_MPI: "<<command<<std::endl;


                if(strcmp(command, "B_FORCE") == 0)
                {
                    pthread_mutex_unlock(&command_mutex);

                    pthread_mutex_lock(&number_mutex);
                    const char * nr = number;
                    pthread_mutex_unlock(&number_mutex);

                    RunAlgorithm(MPIAlgorithm::BruteForce, nr); // wystarczy wyowolac te metode
                    std::cout<<std::endl<<"Algorytm zakonczony"<<std::endl;

                    pthread_mutex_lock(&ready_flag_mutex);
                    ready_flag = true;
                    pthread_mutex_unlock(&ready_flag_mutex);
                }
                else
                {
                    pthread_mutex_unlock(&command_mutex);
                }
            }
            pthread_mutex_lock(&command_mutex);
            if(strcmp(command,"QUIT") == 0)
            {
                pthread_mutex_unlock(&command_mutex);
                break;
            }
            else
            {
                pthread_mutex_unlock(&command_mutex);
            }
        }



 //----------------------------------------------------------------------------------------------------
 /*     bool endFlag = false;
        std::string command, valueStr;
        do{
            getline(std::cin, command); //Te getline to pole do popisu dla komunikacji przez sockety

            if( command == "bruteforce" )
            {
                getline(std::cin, valueStr);
                RunAlgorithm(MPIAlgorithm::BruteForce,valueStr.c_str()); // wystarczy wyowolac te metode
            }
            else if( command == "quit" )
            {
                endFlag = true;
            }
        }while( false == endFlag );*/
//-----------------------------------------------------------------------------------------------------
        SendDieMessageToAll();
        pthread_join(thread_comm, NULL);
    }
    else
    {
        SlaveWait();
    }


    pthread_mutex_destroy(&command_mutex);
    pthread_mutex_destroy(&number_mutex);
    pthread_mutex_destroy(&task_flag_mutex);
    pthread_mutex_destroy(&ready_flag_mutex);

    MPI_Finalize();
    return 0;
}

void RunAlgorithm(MPIAlgorithm::AlgorithmsEnum algoE, const char* valueStr )
{
    MPIAlgorithm *algo = GetAlgorithm(algoE);

    pthread_mutex_lock(&result_mutex);
    result = algo->Master(valueStr);
    pthread_mutex_unlock(&result_mutex);

    pthread_mutex_lock(&result_flag_mutex);
    result_flag = true;
    pthread_mutex_unlock(&result_flag_mutex);

    delete algo;


/*    for( size_t i = 1; i < returned.size(); i += 2 )
        std::cout << "Returned: " << returned[i-1] << " * " << returned[i] << "\n";
    if( returned.size() == 0 )
        std::cout << "To jest liczba pierwsza!\n";*/
}

void* main_comm(void*)
{

    CommSocket *comm = new CommSocket(8060);


    char rcvd_command[8]="";
    char next[8]="NEXT";
    char busy[8]="BUSY";
    char no_res[8]="NO_RES";
    char in_res[8]="IN_RES";

    while(true)
    {

        if(comm->my_receive())
            break;

        pthread_mutex_lock(&ready_flag_mutex);
        if(!ready_flag)
        {
            pthread_mutex_unlock(&ready_flag_mutex);
            if(comm->my_send(busy))
                break;

            continue;
        }
        else
        {
            pthread_mutex_unlock(&ready_flag_mutex);
        }
        strncpy(rcvd_command, comm->get_recv_buffer(), strlen(comm->get_recv_buffer()) + 1);

        if(strcmp(rcvd_command, "B_FORCE") == 0)
        {

            pthread_mutex_lock(&command_mutex);
            strncpy(command, rcvd_command, sizeof(command));
            std::cout<<std::endl<<"Command: "<<command<<std::endl;
            pthread_mutex_unlock(&command_mutex);

            if(comm->my_send(next))
                break;

            if(comm->my_receive())
                break;

            pthread_mutex_lock(&number_mutex);
            strncpy(number, comm->get_recv_buffer(), strlen(comm->get_recv_buffer()) + 1);
            std::cout<<"Number: "<<number<<std::endl;
            pthread_mutex_unlock(&number_mutex);

            pthread_mutex_lock(&task_flag_mutex);
            new_task_flag = true;
            pthread_mutex_unlock(&task_flag_mutex);

            if(comm->my_send(next))
                break;

        }

        if(strcmp(rcvd_command, "RESULT") == 0)
        {
            pthread_mutex_lock(&result_flag_mutex);
            if(result_flag)
            {
                result_flag = false;
                pthread_mutex_unlock(&result_flag_mutex);

                if(comm->my_send(in_res))
                    break;

                if(comm->my_receive())
                    break;

                std::cout<<comm->get_recv_buffer() << std::endl;


                if(strcmp(comm->get_recv_buffer(), "START") == 0)
                {
                    std::cout<<"In start if" << std::endl;
                    pthread_mutex_lock(&result_mutex);
                    for(unsigned int i = 0; i<result.size(); i++)
                    {   std::cout<<"In for loop: "<< i << std::endl;
                        char buff[result[i].length() + 1];
                        strcpy(buff, result[i].c_str());

                        if(comm->my_send(buff))
                            break;

                        if(comm->my_receive())
                            break;

                        if(strcmp(comm->get_recv_buffer(), "NEXT") == 0)
                            continue;
                        else
                            break;
                    }
                    pthread_mutex_unlock(&result_mutex);

                    char res_end[8] ="RES_END";
                    if(comm->my_send(res_end))
                            break;

                }

            }
            else
            {
                pthread_mutex_unlock(&result_flag_mutex);
                if(comm->my_send(no_res))
                    break;
                continue;
            }

        }

        if(strcmp(rcvd_command, "QUIT") == 0)
        {
            pthread_mutex_lock(&command_mutex);
            strncpy(command, rcvd_command, sizeof(command));
            pthread_mutex_unlock(&command_mutex);
            break;
        }

    }

    delete comm;
    pthread_exit(NULL);
}

