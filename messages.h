#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>

class message
{
public:
    message();

    void in(char*);
    std::string out()const;
    std::string get_content()const;

private:
    enum msg_type {
        MESSAGE,
        LIST_REQUEST,
        START_CONV,
        SEND_FILE
    }msg_type;

    std::string content;
};

#endif // message_H
