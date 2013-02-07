#ifndef MESSAGE_H
#define MESSAGE_H

#include <sstream>
#include <iostream>

class Message
{
public:
    Message(std::string);

    std::string get_content(bool)const;

    enum MsgType
    {
        NONE,
        MESSAGE,
        LIST_INFO,
        INVITE,
        SEND_FILE,
        SERVER_GOING_DOWN
    };

    MsgType get_type()const;
private:
    MsgType type;

    std::stringstream content;
    std::string message;
};

std::ostream& operator<<(std::ostream &, const Message&);
#endif // message_H

