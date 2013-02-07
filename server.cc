#include "server.hh"
#include "thread_starter.hh"

#include <pthread.h>

int main(void)
{
    Server s;
    s.run();

    return 0;
}

Server::Server():listenfd(0),connfd(0),done(false),poll_thread(0),cmd_thread(0),
    client_mutex(PTHREAD_MUTEX_INITIALIZER),done_mutex(PTHREAD_MUTEX_INITIALIZER)
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
    pthread_mutex_lock(&client_mutex);
    Message msg("5");

    for(auto it = clients.begin(); it != clients.end();)
    {
        (*it)->sending(msg);
        delete (*it);
        it = clients.erase(it);
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
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
        pthread_mutex_lock( &client_mutex );
        clients.push_back(new Connection(connfd, *this));
        clients.back()->sending(Message("Terve!"));
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
        usleep(5);
    }
}

void Server::check_connections()
{
    pthread_mutex_lock(&client_mutex);
    for(auto it = clients.begin(); it != clients.end();)
    {
        if ((*it)->get_state() == Connection::DISCONNECTED)
        {
            delete(*it);
            it = clients.erase(it);
        }
        else it++;
    }
    pthread_mutex_unlock(&client_mutex);
}

void Server::handle_msg(const Message &msg, const Connection &client)
{
    std::string help("");
    switch (msg.get_type())
    {
    case Message::MESSAGE:
        std::cout << client.get_name()<<": "<<msg << std::endl;
        //std::cout <<msg->get_content(true)<<std::endl;
        break;
    case Message::LIST_INFO:
        pthread_mutex_lock(&client_mutex);
        for (auto it = clients.begin();it != clients.end();it++){
            if (client.get_name() != (*it)->get_name())
                help += (*it)->get_name() +" ";
        }
        pthread_mutex_unlock(&client_mutex);

        if (help.size() > 0) help.erase(help.end()-1);
        client.sending(Message(help, msg.get_type()));
        break;
    default:
        break;
    }
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

    while(!done)
    {
        check_connections();
        //check_messages();
        usleep(5);
    }
}
