#ifndef SERVER_H
#define SERVER_H

#include "connection.hh"
#include "messages.hh"

#include <list>
#include <netinet/in.h>
#include <cstring>


class Connection;

class Server
{
public:
    Server();
    ~Server();

    void run();
    void handle_msg(const Message&, const Connection&);

private:
    void poll_clients();
    void read_commands();
    void check_connections();

    std::list<Connection*> clients;
    //std::queue<Message*> msg_queue;

    int listenfd, connfd;
    bool done;
    sockaddr_in serv_addr;

    pthread_t poll_thread, cmd_thread;
    pthread_mutex_t client_mutex, done_mutex;//, msg_mutex;

    Server(const Server&);
    Server operator= (const Server&);
};

#endif //SERVER_H
