#ifndef CONNETION_H
#define CONNETION_H

#include "messages.hh"
#include "server.hh"

class Server;

class Connection
{
public:
    Connection(int,Server&);
    ~Connection();

    enum state
    {
        NORMAL,
        DISCONNECTED
    };

    int get_state() const;
    std::string get_name() const;

    void send_to(const Message& msg) const;
private:
    void start();
    void receive();
    void set_state(const state&);

    int connfd;
    Server& server;

    std::string name;
    state State;
    pthread_t recv_thread, send_thread;

    Connection(const Connection&);
    Connection operator= (const Connection&);
};

#endif // CONNETION_H
