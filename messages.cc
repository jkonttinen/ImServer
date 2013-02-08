#include "messages.hh"

Message::Message(const std::string& msg):content(msg)
{
    size_t num;
    content >> num;
    type = (Message::MsgType)num;
    if (type != NONE && type < LIST_INFO)
        content >> destination;
    content.ignore();
    std::getline(content, message);
}

Message::Message(const std::string& msg, const Message::MsgType& type,
            const std::string& dest): type(type),message(msg),destination(dest)
{
    if (destination != "")
        content << type <<" "<< destination <<" "<< msg;
    else content << type <<" "<< msg;
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

std::string Message::get_destination()const
{
    return destination;
}

std::ostream& operator<< (std::ostream& out, const Message& msg)
{
    out << msg.get_content(false);
    return out;
}
