#include "connection.h"
#include "thread_starter.h"
#include "messages.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>

connection::connection(int connfd):connfd(connfd),name(""),State(NORMAL),
    recv_thread(0),send_thread(0)
{
    start();
}

connection::~connection()
{
    if (recv_thread){
        pthread_cancel(recv_thread);
        pthread_join(recv_thread,NULL);
    }
    if (send_thread){
        pthread_cancel(send_thread);
        pthread_join(send_thread,NULL);
    }

    close(connfd);
}

void connection::receive()
{
    int res,num;
    char size[32] = "", *buf = NULL;

    while(1)
    {
        res = recv(connfd,size,sizeof(size),0);
        if (res != sizeof(size))
        {
            State = DISCONNECTED;
            break;
        }
        num = atoi(size);
        buf = new char[num+1];

        res = recv(connfd,buf,num,0);
        buf[num] = '\0';
        if (res < 1)
        {
            State = DISCONNECTED;
            delete [] buf;
            break;
        }
        std::cout <<name<<": "<<buf<<std::endl;
        //printf("%s: %s\n",name.c_str(),buf);
        delete [] buf;
        usleep(5);
    }
    std::cout <<name<<" disconnected."<<std::endl;
    //printf("%s disconnected.\n", name.c_str());
}

void connection::start()
{
    int res,rc1;
    char num[4], *buf;

    res = recv(connfd,num,sizeof(num),0);
    if (res != sizeof(num))
    {
        State = DISCONNECTED;
        return;
    }

    buf = new char[atoi(num) +1];
    res = recv(connfd,buf,atoi(num)+1,0);
    if (res < 1)
    {
        State = DISCONNECTED;
        delete [] buf;
        return;
    }
    name = buf;
    std::cout<<name<<" connected."<<std::endl;
    //printf("%s connected.\n",name.c_str());
    delete [] buf;

    if ((rc1=pthread_create( &recv_thread, NULL, start_thread<connection,
                            &connection::receive>, this)))
        std::cout<<"Thread creation failed: "<<rc1<<std::endl;
        //printf("Thread creation failed: %d\n", rc1);
}

int connection::get_state() const
{
    return State;
}

std::string connection::get_name() const
{
    return name;
}
