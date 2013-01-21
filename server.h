#ifndef SERVER_H
#define SERVER_H

#include "connection.h"
#include "messages.h"

class Server
{
public:
    Server();
    ~Server();

    void run();

private:
    void poll_clients();
    void read_commands();

    std::list<Connection*> clients;
    std::queue<Message*> msg_queue;

    int listenfd, connfd;
    bool done;
    sockaddr_in serv_addr;

    pthread_t poll_thread, cmd_thread;
    pthread_mutex_t client_mutex,done_mutex;

    Server(const Server&);
    Server operator= (const Server&);
};

#endif //SERVER_H
