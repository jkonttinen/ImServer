 #include "messages.h"
 #include <iostream>

message::message():content("")
{
}

void in(char* msg){
    content = msg;
}

std::string out()const{
    return get_content();
}

std::string get_content()const{
    return content;
}

