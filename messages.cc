#include "messages.hh"

Message::Message(const std::string& msg):content(msg)
{
    size_t num;
    content >> num;
    type = (Message::MsgType)num;
    std::string help;
    if (type != NONE && type < LIST_ALL){
        do {
            content >> name;
        }while(help != "|");
    }
    do{
        content >> message;
    }while(help != "|");
    content >> chat;
}

Message::Message(const Message::MsgType& type, const std::string& name,
    const std::string& msg, const std::string& chat): name(name),message(msg),chat(chat)
{
    if (chat != "")
        content << name <<" | "<< msg <<" | "<< chat;
    else content << name <<" | "<< msg;
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

std::string Message::get_name()const
{
    return name;
}

std::string Message::get_chat()const
{
    return chat;
}

std::ostream& operator<< (std::ostream& out, const Message& msg)
{
    out << msg.get_content(false);
    return out;
}
