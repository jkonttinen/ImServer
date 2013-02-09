#ifndef MESSAGE_H
#define MESSAGE_H

#include <sstream>
#include <iostream>

class Message
{
public:
    enum MsgType
    {
        NONE,
        MESSAGE,
        CHAT_MESSAGE,
        INVITE,
        SEND_FILE,
        LIST_INFO,
        SERVER_GOING_DOWN
    };
    Message(const std::string&);
    Message(const std::string&, const MsgType&, const std::string&);

    std::string get_content(bool)const;
    MsgType get_type()const;
    std::string get_info()const;
private:
    MsgType type;

    std::stringstream content;
    std::string message;
    std::string info;
};

std::ostream& operator<<(std::ostream &, const Message&);
#endif // message_H

