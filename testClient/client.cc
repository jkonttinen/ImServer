#include "address.hh"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <cerrno>
#include <arpa/inet.h>

#include <iostream>
#include <sstream>
#include <string>
#include <pthread.h>

void* receive_thread(void* ptr){
    int rv = 0;
    int connfd = *((int*) ptr);
    char *buf, num[32] = "";

    while (1){
        rv = recv(connfd, num, 32, 0);
        if (rv < 1) {
            std::cout<<"Reading failed."<<std::endl;
            break;
        }
        buf = new char[atoi(num)+1];
        rv = recv(connfd, buf, atoi(num), 0);
        buf[atoi(num)] = '\0';
        if (rv < 1){
            std::cout <<"Reading failed."<<std::endl;
            delete [] buf;
            break;
        }
        std::cout <<buf<<std::endl;
        delete [] buf;
    }

    return 0;
}

int main()
{
    int sockfd = 0, res;
    char address[100];
    std::string sendStr;
    std::stringstream helpStr;
    struct sockaddr_in serv_addr;

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    }

    memset(&serv_addr, '\0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(12345);

    if (host_lookup("mip4.kyla.fi",address) != 0){
        printf("Host not found\n");
        return 1;
    }

    if(inet_pton(AF_INET, address, &serv_addr.sin_addr)<=0)
    {
        printf("\n inet_pton error occured\n");
        return 1;
    }

    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\n Error : Connect Failed \n");
        return 1;
    }

    std::cout <<"Anna nimimerkki: ";
    std::cin >> sendStr;
    helpStr << sendStr.size();
    res = send(sockfd,helpStr.str().c_str(),4,0);
    if (res < 1) std::cout << "Error while trying to send" <<std::endl;
    res = send(sockfd,sendStr.c_str(),sendStr.size(),0);
    if (res < 1) std::cout << "Error while trying to send" <<std::endl;
    std::cin.ignore();

    int rv;
    pthread_t recv_thread = 0;
    if ((rv = pthread_create(&recv_thread, NULL, receive_thread, &sockfd)))
        std::cout <<"Thread creation failed: "<<rv<<std::endl;

    while(1)
    {
        helpStr.clear();
        helpStr.str(std::string());

        //buffer = new char[len];
        std::getline(std::cin,sendStr);
        //sendStr = buffer;
        if (!sendStr.compare("exit")) break;

        helpStr << sendStr.size();
        std::cout << sendStr.size()<< " ";
        std::cout << helpStr.str() << std::endl;

        res = send(sockfd,helpStr.str().c_str(),32,0);
        if (res < 1) std::cout << "Error while trying to send" <<std::endl;
        res = send(sockfd,sendStr.c_str(),sendStr.size(),0);
        if (res < 1) std::cout << "Error while trying to send" <<std::endl;
    }

    pthread_cancel(recv_thread);
    pthread_join(recv_thread,NULL);

    close(sockfd);
    return 0;
}
