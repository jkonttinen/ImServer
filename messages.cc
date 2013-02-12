#include "messages.hh"

Message::Message(const std::string& msg):content(msg)
{
    size_t num;
    content >> num;
    type = (Message::MsgType)num;
    if (type != NONE && type < LIST_ALL)
        content >> info;
    content.ignore();
    std::getline(content, message);
}

Message::Message(const std::string& msg, const Message::MsgType& type,
            const std::string& info): type(type),message(msg),info(info)
{
    if (info != "")
        content << type <<" "<< info <<" "<< msg;
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

std::string Message::get_info()const
{
    return info;
}

std::ostream& operator<< (std::ostream& out, const Message& msg)
{
    out << msg.get_content(false);
    return out;
}
