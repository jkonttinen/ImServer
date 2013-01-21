#ifndef CONNETION_H
#define CONNETION_H

#include "messages.h"
#include <string>

class Connection
{
public:
    Connection(int);
    ~Connection();

    enum state
    {
        NORMAL,
        DISCONNECTED
    };

    void set_state(state);

    int get_state() const;
    std::string get_name() const;

    void sending(Message msg) const;
private:
    void start();
    void receive();

    int connfd;
    std::string name;
    state State;
    pthread_t recv_thread, send_thread;

    Connection(const Connection&);
    Connection operator= (const Connection&);
};

#endif // CONNETION_H
