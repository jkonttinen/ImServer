#ifndef SERVER_H
#define SERVER_H

#include "connection.hh"
#include "messages.hh"
#include "chat.hh"

#include <list>
#include <map>
#include <netinet/in.h>

class Connection;
class Chat;

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
    Connection* get_client(const std::string&);

    std::list<Connection*> clients;
    std::map<std::string, Chat*> chats;

    int listenfd, connfd;
    bool done;
    sockaddr_in serv_addr;

    pthread_t poll_thread, cmd_thread;
    pthread_mutex_t client_mutex, done_mutex, chat_mutex;

    Server(const Server&);
    Server operator= (const Server&);
};

#endif //SERVER_H
