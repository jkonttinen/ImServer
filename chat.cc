#include "chat.hh"
//#include <sstream>

Chat::Chat(Connection* client, const std::string& name):name(name)
{
    clients.push_back(client);
}

Chat::Chat(const std::list<Connection*>& clients, const std::string& name):
    clients(clients), name(name) {}

Chat::~Chat()
{
    clients.clear();
}

void Chat::send_all(const Message& msg, const Connection& sender) const
{
    std::stringstream ss;
    ss << sender.get_name() <<": "<<msg.get_content(false);

    for (auto it = clients.begin(); it != clients.end(); it++)
        if (sender.get_name() != (*it)->get_name())
            (*it)->send_to(Message(ss.str(),msg.get_type(),msg.get_info()));
}

void Chat::add_client(Connection* client)
{
    clients.push_back(client);
}

std::string Chat::get_name()const
{
    return name;
}
