#include "messages.hh"

Message::Message(const std::string& msg):content(msg)
{
    size_t num,c=0;
    std::string help;
    content >> num;
    type = (Message::MsgType)num;
    for (size_t i = 2; i < msg.size(); i++){
        if (msg[i] != '|') help += msg[i];
        else {
            c++;
            if (c == 1) name = help;
            else if (c == 2) message = help;
            else chat = help;
            help = std::string("");
        }
    }
}

Message::Message(const Message::MsgType& type, const std::string& name,
    const std::string& msg, const std::string& chat): type(type),name(name),message(msg),chat(chat)
{
    if (chat != "")
        content << type << "|" << name <<"|"<< msg <<"|"<< chat << "|";
    else content << type << "|" << name <<"|"<< msg << "|";
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
