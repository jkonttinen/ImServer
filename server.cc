#include "server.hh"
#include "thread_starter.hh"

#include <pthread.h>
#include <cstring>
#include <ctime>

int main(void)
{
    Server s;
    s.run();

    return 0;
}

Server::Server():listenfd(0),done(false),poll_thread(0),cmd_thread(0),
    client_mutex(PTHREAD_MUTEX_INITIALIZER),done_mutex(PTHREAD_MUTEX_INITIALIZER),
    chat_mutex(PTHREAD_MUTEX_INITIALIZER)
{
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        std::cout<<"Couldn't create socket\n"<<std::endl;
        exit(1);
    }
    memset(&serv_addr, '\0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(12345);

    if(bind(listenfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) != 0)
    {
        std::cout<<"Couldn't bind the socket\n"<<std::endl;
        exit(1);
    }

    if ((listen(listenfd, 10)) != 0)
    {
        std::cout<<"Listening failed\n"<<std::endl;
        exit(1);
    }
}

Server::~Server()
{
    pthread_mutex_lock(&chat_mutex);
    for (auto it = chats.begin(); it != chats.end(); it++)
        delete((*it).second);
    pthread_mutex_unlock(&chat_mutex);

    pthread_mutex_lock(&client_mutex);
    std::stringstream ss;
    ss << Message::EXIT;
    Message msg(ss.str());

    for(auto it = clients.begin(); it != clients.end(); it++)
    {
        (*it)->send_to(msg);
        usleep(1000000);
        delete (*it);
    }
    pthread_mutex_unlock(&client_mutex);

    if (poll_thread)
    {
        pthread_cancel(poll_thread);
        pthread_join(poll_thread,NULL);
    }
    if (cmd_thread)
    {
        pthread_cancel(cmd_thread);
        pthread_join(cmd_thread,NULL);
    }
    close(listenfd);
}

void Server::poll_clients()
{
    while(!done)
    {
        int connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
        pthread_mutex_lock( &client_mutex );
        clients.push_back(new Connection(connfd, *this));
        if (clients.back()->get_state() == Connection::DISCONNECTED)
            clients.back()->send_to(Message(Message::EXIT,clients.back()->get_name(),"Duplicate name"));
        else {
            clients.back()->send_to(Message(Message::NONE,clients.back()->get_name(),"Terve!"));
            pthread_mutex_unlock(&client_mutex);
            send_lists();
            pthread_mutex_lock(&client_mutex);
        }
        pthread_mutex_unlock( &client_mutex );
        usleep(5);
    }
}

void Server::read_commands()
{
    std::string command;
    while(!done)
    {
        std::cin >> command;
        pthread_mutex_lock(&done_mutex);
        if (!command.compare("exit")) done = true;
        pthread_mutex_unlock(&done_mutex);

        pthread_mutex_lock(&client_mutex);
        if (!command.compare("list"))
        {
            size_t i = 0;
            for (auto it = clients.begin(); it != clients.end(); it++)
            {
                std::cout << (*it)->get_name() << std::endl;
                i++;
            }
            std::cout << i <<" clients total."<<std::endl;
        }
        pthread_mutex_unlock(&client_mutex);
        usleep(5);
    }
}

void Server::check_connections()
{
    bool help = false;
    pthread_mutex_lock(&client_mutex);
    pthread_mutex_lock(&chat_mutex);
    for(auto it = clients.begin(); it != clients.end();)
    {
        if ((*it)->get_state() == Connection::DISCONNECTED)
        {
            usleep(1000000);
            for (auto it1 = chats.begin();it1 != chats.end();){
                it1->second->remove_client((*it)->get_name());
                if (it1->second->empty()) {
                   delete (it1->second);
                   it1 = chats.erase(it1);
                }
                else it1++;
            }

            delete(*it);
            *it = NULL;
            it = clients.erase(it);
            help = true;
        }
        else it++;
    }
    pthread_mutex_unlock(&chat_mutex);
    pthread_mutex_unlock(&client_mutex);
    if (help) send_lists();
}

Connection* Server::get_client(const std::string& name)
{
    for (auto it = clients.begin();it != clients.end(); it++)
        if ((*it)->get_name() == name) return (*it);
    return NULL;
}

void Server::send_lists()
{
    for (auto it = clients.begin();it != clients.end();it++)
        if (*it) handle_msg(Message(Message::LIST_ALL,(*it)->get_name()),**it);
}

void Server::handle_msg(const Message &msg, const Connection &client)
{
    pthread_mutex_lock(&client_mutex);
    pthread_mutex_lock(&chat_mutex);

    switch (msg.get_type())
    {
    case Message::MESSAGE:
    {
        for (auto it = clients.begin(); it != clients.end(); it++)
        {
            if ((*it)->get_name() == msg.get_name())
            {
                (*it)->send_to(Message(msg.get_type(),client.get_name(),msg.get_content(false)));
                break;
            }
        }
        break;
    }

    case Message::CHAT_MESSAGE:
    {
        if (chats.find(msg.get_chat()) == chats.end()) break;
        if (msg.get_name() != client.get_name()) break;
        if (!chats[msg.get_chat()]->has_client(client.get_name())) break;
        chats[msg.get_chat()]->send_all(msg);
        break;
    }
    case Message::INVITE:
    {
        if (msg.get_name() == client.get_name()) break;

        std::stringstream chat_name;
        Connection* cptr = NULL;

        if (msg.get_content(false) == std::string("new"))
        {
            do {
                chat_name << "Chat" << random();
            }while(!(chats.find(chat_name.str()) == chats.end()));

            cptr = get_client(client.get_name());
            if (cptr == NULL) break;
            chats[chat_name.str()] = new Chat(cptr, chat_name.str());

            cptr = get_client(msg.get_name());
            if (cptr == NULL) break;
            chats[chat_name.str()]->add_client(cptr);

            cptr->send_to(Message(Message::INVITE, client.get_name(), "invited you to a chat.",  chat_name.str()));
            client.send_to(Message(Message::INVITE, client.get_name(), "Sent invitation.", chat_name.str()));
        }
        else
        {
            chat_name << msg.get_chat();
            if (chats.find(chat_name.str()) == chats.end()) break;
            if (chats.find(chat_name.str())->second->has_client(msg.get_name())) break;
            cptr = get_client(msg.get_name());
            if (cptr == NULL) break;
            chats[chat_name.str()]->add_client(cptr);
            cptr->send_to(Message(Message::INVITE, client.get_name(), "invited you to a chat.", chat_name.str()));
            client.send_to(Message(Message::INVITE, client.get_name(),"Sent invitation.", chat_name.str()));
        }
        break;
    }
    case Message::PART_CHAT:
    {
        std::stringstream chat_name,ss;
        chat_name << msg.get_chat();
        if (chats.find(chat_name.str()) == chats.end()) break;

        //chats[chat_name.str()]->send_all(Message("quit the chat.",msg.get_type(),""),client);
        chats[chat_name.str()]->remove_client(client.get_name());

        client.send_to(Message(msg.get_type(),msg.get_name(), "You quit the chat."));
        break;
    }
    case Message::LIST_CHAT:
    {
        std::string help("");
        if (chats.find(msg.get_chat()) == chats.end()) break;
        help = chats[msg.get_chat()]->get_namelist();
        client.send_to(Message(msg.get_type(), msg.get_name(),help,msg.get_chat()));
        break;
    }
    case Message::LIST_ALL:
    {
        std::string help("");
        for (auto it = clients.begin(); it != clients.end(); it++)
        {
            if (client.get_name() != (*it)->get_name())
                help += (*it)->get_name() +"~";
        }

        client.send_to(Message(msg.get_type(),msg.get_name(),help));
        break;
    }
    default: break;
    }
    pthread_mutex_unlock(&chat_mutex);
    pthread_mutex_unlock(&client_mutex);
}

bool Server::has_client(const std::string& name) const
{
    for (auto it = clients.begin(); it != clients.end(); it++)
        if (name == (*it)->get_name()) return true;
    return false;
}

void Server::run()
{
    int res;

    if ((res = pthread_create(&poll_thread,NULL,start_thread<Server,&Server::poll_clients>,this)))
        std::cout <<"Thread creation failed: "<<res<<std::endl;
    //printf("Thread creation failed: %d\n", res);
    if ((res = pthread_create(&cmd_thread,NULL,start_thread<Server,&Server::read_commands>,this)))
        std::cout <<"Thread creation failed: "<<res<<std::endl;
    //printf("Thread creation failed: %d\n", res);
    time_t tStart, tEnd;
    time(&tStart);

    while(!done)
    {
        time(&tEnd);

        if (difftime(tEnd,tStart) >= 240) {
            send_lists();
            time(&tStart);
        }
        check_connections();
        //check_messages();
        usleep(5000);
    }
}

