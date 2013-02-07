#include "connection.hh"
#include "thread_starter.hh"
#include "messages.hh"
#include "server.hh"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <sstream>

Connection::Connection(int connfd, Server& server):connfd(connfd),server(server),
    name(""),State(NORMAL),recv_thread(0),send_thread(0)
{
    start();
}

Connection::~Connection()
{
    if (recv_thread)
    {
        pthread_cancel(recv_thread);
        pthread_join(recv_thread,NULL);
    }
    if (send_thread)
    {
        pthread_cancel(send_thread);
        pthread_join(send_thread,NULL);
    }

    close(connfd);
}

void Connection::receive()
{
    int res,num;
    char size[32] = "", *buf = NULL;
    Message *msg;

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

        //msg = new Message(std::string(buf));
        server.handle_msg(Message(std::string(buf)));
        //delete msg;
        delete [] buf;
        usleep(5);
    }
    std::cout <<name<<" disconnected."<<std::endl;
}

void Connection::sending(const Message& msg)const
{
    int rv;
    std::stringstream ostr;
    char help[32] = "";
    ostr << msg.get_content(true);
    sprintf(help,"%lu", ostr.str().size());

    rv = send(connfd, help, 32,0);
    if (rv < 1) std::cout << "Error while trying to send" <<std::endl;
    rv = send(connfd, ostr.str().c_str(), ostr.str().size(), 0);
    if (rv < 1) std::cout << "Error while trying to send" <<std::endl;
}

void Connection::start()
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

    if ((rc1=pthread_create( &recv_thread, NULL, start_thread<Connection,
                             &Connection::receive>, this)))
        std::cout<<"Thread creation failed: "<<rc1<<std::endl;
    //printf("Thread creation failed: %d\n", rc1);
}

int Connection::get_state() const
{
    return State;
}

std::string Connection::get_name() const
{
    return name;
}
