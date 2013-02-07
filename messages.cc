#include "messages.hh"

Message::Message(const std::string& msg):content(msg)
{
    size_t num;
    content >> num;
    content.ignore();
    std::getline(content, message);
    type = (Message::MsgType)num;
}

Message::Message(const std::string& msg, const Message::MsgType& type):
    type(type),message(msg)
{
    content << type <<" "<< msg;
}

std::string Message::get_content(bool all)const
{
    if (all) return content.str();
    else return message;
}

Message::MsgType Message::get_type()const
{
    return type;
}

std::ostream& operator<< (std::ostream& out, const Message& msg)
{
    out << msg.get_content(false);
    return out;
}
