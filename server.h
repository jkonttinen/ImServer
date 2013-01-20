#ifndef SERVER_H
#define SERVER_H

#include "connection.h"
#include "messages.h"

class server
{
public:
    server();
    ~server();

    void run();

private:
    void poll_clients();
    void read_commands();

    std::list<connection*> clients;
    std::queue<message*> msg_queue;

    int listenfd, connfd;
    bool done;
    sockaddr_in serv_addr;

    pthread_t poll_thread, cmd_thread;
    pthread_mutex_t client_mutex,done_mutex;
};

#endif //SERVER_H
