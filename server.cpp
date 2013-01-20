//#include <sys/socket.h>
//#include <sys/types.h>
#include <netinet/in.h>
//#include <arpa/inet.h>
//#include <cstdio>
//#include <cstdlib>
#include <cstring>

#include <iostream>
#include <string>
#include <list>
#include <queue>

#include <pthread.h>

#include "server.h"
#include "thread_starter.h"

int main(void)
{
    server s;
    s.run();

    return 0;
}

server::server():listenfd(0),connfd(0),done(false),poll_thread(0),cmd_thread(0),
   client_mutex(PTHREAD_MUTEX_INITIALIZER),done_mutex(PTHREAD_MUTEX_INITIALIZER)
{
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        std::cout<<"Couldn't create socket\n"<<std::endl;
        exit(1);
    }
    memset(&serv_addr, '\0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(12345);

    if(bind(listenfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) != 0)
    {
        std::cout<<"Couldn't bind the socket\n"<<std::endl;
        exit(1);
    }

    if ((listen(listenfd, 10)) != 0)
    {
        std::cout<<"Listening failed\n"<<std::endl;
        exit(1);
    }
}

server::~server(){
    pthread_mutex_lock(&client_mutex);
    for(auto it = clients.begin(); it != clients.end();){
        delete (*it);
        it = clients.erase(it);
    }

    pthread_mutex_unlock(&client_mutex);

    if (poll_thread){
        pthread_cancel(poll_thread);
        pthread_join(poll_thread,NULL);
    }
    if (cmd_thread){
        pthread_cancel(cmd_thread);
        pthread_join(cmd_thread,NULL);
    }
}

void server::poll_clients()
{
    while(!done)
    {
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
        pthread_mutex_lock( &client_mutex );
        clients.push_back(new connection(connfd));
        pthread_mutex_unlock( &client_mutex );
        usleep(5);
    }
}

void server::read_commands()
{
    std::string command;
    while(!done)
    {
        std::cin >> command;
        pthread_mutex_lock(&done_mutex);
        if (!command.compare("exit")) done = true;
        pthread_mutex_unlock(&done_mutex);
        usleep(5);
    }
}

void server::run()
{
    int res;

    if ((res = pthread_create(&poll_thread,NULL,start_thread<server,&server::poll_clients>,this)))
        std::cout <<"Thread creation failed: "<<res<<std::endl;
        //printf("Thread creation failed: %d\n", res);
    if ((res = pthread_create(&cmd_thread,NULL,start_thread<server,&server::read_commands>,this)))
        std::cout <<"Thread creation failed: "<<res<<std::endl;
        //printf("Thread creation failed: %d\n", res);
    while(!done)
    {
        pthread_mutex_lock(&client_mutex);
        for(auto it = clients.begin(); it != clients.end();)
        {
            if ((*it)->get_state() == connection::DISCONNECTED)
            {
                delete(*it);
                it = clients.erase(it);
            }
            else it++;
        }
        pthread_mutex_unlock(&client_mutex);
        usleep(5);
    }
}