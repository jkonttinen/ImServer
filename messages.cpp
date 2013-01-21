 #include "messages.h"
 #include <iostream>

Message::Message():content("")
{
}

void Message::in(char* msg){
    content = msg;
}

std::string Message::out()const{
    return get_content();
}

std::string Message::get_content()const{
    return content;
}

