#ifndef CHAT_HH
#define CHAT_HH

#include "connection.hh"
#include "messages.hh"

#include <list>
#include <string>

class Connection;

class Chat
{
public:
    Chat(Connection*, const std::string&);
    Chat(const std::list<Connection*>&, const std::string&);

    ~Chat();

    void add_client(Connection*);
    void remove_client(const std::string&);
    void send_all(const Message&, const Connection&)const;

    bool has_client(const std::string&)const;

    std::string get_name()const;
private:
    Chat(const Chat&);
    Chat operator= (const Chat&);

    std::list<Connection*> clients;
    std::string name;
};

#endif // CHAT_HH
