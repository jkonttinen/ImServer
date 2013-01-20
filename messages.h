#ifndef MESSAGE_H
#define MESSAGE_H

#include <queue>

class message
{
public:
    message();
    ~message();

    void parse_msg(char*);

private:
    enum msg_type {
        MESSAGE,
        LIST_REQUEST,
        START_CONV,
        SEND_FILE
    }msg_type;
};

#endif // message_H
