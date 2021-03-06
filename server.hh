#ifndef SERVER_HH
#define SERVER_HH

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
    bool has_client(const std::string&) const;
private:
    void poll_clients();
    void read_commands();
    void check_connections();
    Connection* get_client(const std::string&);
    void send_lists();

    std::list<Connection*> clients;
    std::map<std::string, Chat*> chats;

    int listenfd;
    bool done;
    sockaddr_in serv_addr;

    pthread_t poll_thread, cmd_thread;
    pthread_mutex_t client_mutex, done_mutex, chat_mutex;

    Server(const Server&);
    Server operator= (const Server&);
};

#endif //SERVER_HH
