#ifndef MESSAGES_HH
#define MESSAGES_HH

#include <iostream>
#include <sstream>

class Message
{
public:
    enum MsgType {
        NONE,
        MESSAGE,
        CHAT_MESSAGE,
        INVITE,
        SEND_FILE,
        LIST_ALL,
        LIST_CHAT,
        PART_CHAT,
        EXIT
    };
    Message(const std::string&);
    Message(const MsgType&, const std::string&, const std::string& msg="", const std::string& chat="");

    std::string get_content(bool)const;
    MsgType get_type()const;
    std::string get_name()const;
    std::string get_chat()const;
private:
    MsgType type;

    std::stringstream content;
    std::string name;
    std::string message;
    std::string chat;
};

std::ostream& operator<<(std::ostream &, const Message&);
#endif // MESSAGES_HH

