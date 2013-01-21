#ifndef CONNECTION_H
#define CONNECTION_H

#include "messages.h"
#include <string>

class connection
{
public:
    connection(int);
    ~connection();

    enum state
    {
        NORMAL,
        DISCONNECTED
    };

    void set_state(state);

    int get_state() const;
    std::string get_name() const;

    void send(message msg);
private:
    connection(&connection);
    connection operator= (&connection); 

    void start();
    void receive();

    int connfd;
    std::string name;
    state State;
    pthread_t recv_thread, send_thread;

};

#endif // CONNECTION_H
