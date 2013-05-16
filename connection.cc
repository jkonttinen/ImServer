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
//    shutdown(connfd,SHUT_RDWR);
    close(connfd);
}

void Connection::receive()
{
    while(1)
    {
        int res,num;
        char size[32] = "", *buf = NULL;

        res = recv(connfd,size,sizeof(size),0);
        if (res != sizeof(size)) break;

        num = atoi(size);
        buf = new char[num+1];

        res = recv(connfd,buf,num,0);
        buf[num] = '\0';
        if (res < 1)
        {
            delete [] buf;
            break;
        }

        server.handle_msg(Message(std::string(buf)), *this);

        delete [] buf;
        usleep(5);
    }
    
    std::cout <<name<<" disconnected."<<std::endl;
    set_state(DISCONNECTED);
}

void Connection::send_to(const Message& msg)const
{
    int rv;
    std::stringstream ostr;
    char help[32] = "";
    ostr << msg.get_content(true);

    sprintf(help,"%u", ostr.str().size());

    rv = send(connfd, help, 32,MSG_NOSIGNAL);
    if (rv < 1){
        std::cout << "Error while trying to send" <<std::endl;
        return;
    }
    rv = send(connfd, ostr.str().c_str(), ostr.str().size(), MSG_NOSIGNAL);
    if (rv < 1){
        std::cout << "Error while trying to send" <<std::endl;
        return;
    }
}

void Connection::start()
{
    int res,rc1;
    char num[4], *buf;

    res = recv(connfd,num,sizeof(num),0);
    if (res != sizeof(num))
    {
        set_state(DISCONNECTED);
        return;
    }

    buf = new char[atoi(num) +1];
    res = recv(connfd,buf,atoi(num)+1,0);
    if (res < 1)
    {
        set_state(DISCONNECTED);
        delete [] buf;
        return;
    }
    buf[atoi(num)] = '\0';
    name = buf;
    std::cout<<name<<" connected."<<std::endl;
    //printf("%s connected.\n",name.c_str());
    delete [] buf;

    if (server.has_client(name)) {
        set_state(DISCONNECTED);
        return;
    }

    if ((rc1=pthread_create( &recv_thread, NULL, start_thread<Connection,
                             &Connection::receive>, this)))
        std::cout<<"Thread creation failed: "<<rc1<<std::endl;
    //printf("Thread creation failed: %d\n", rc1);
}

Connection::state Connection::get_state() const
{
    return State;
}

void Connection::set_state(const state& state)
{
    State = state;
}

std::string Connection::get_name() const
{
    return name;
}
