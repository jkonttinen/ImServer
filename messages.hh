#ifndef MESSAGE_HH
#define MESSAGE_HH

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
        LIST_ALL,
        LIST_CHAT,
        PART_CHAT,
        EXIT
    };
    Message(const std::string&);
    Message(const std::string&, const MsgType&, const std::string& info = "");

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
#endif // message_HH

