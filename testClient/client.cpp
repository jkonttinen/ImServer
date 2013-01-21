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

int main(int argc, char *argv[])
{
    int sockfd = 0, len = 256,res;
    char *buffer;
    std::string sendStr;
    std::stringstream helpStr;
    struct addrinfo hints, *serv;
    struct sockaddr_in serv_addr;

    if(argc != 2)
    {
        printf("\n Usage: %s <ip of server> \n",argv[0]);
        return 1;
    }
/*
    memset(&hints,0,sizeof(hints));
    hints.ai_family = AF_UNSPEC; // use AF_INET6 to force IPv6
    hints.ai_socktype = SOCK_STREAM;

    if ((res = getaddrinfo("mip4.kyla.fi","12345",&hints,&serv)) != 0){
        std::cout<<"Error "<<res<<std::endl;
	return 1;
    }

    if ((sockfd = socket(serv->ai_family, serv->ai_socktype, serv->ai_protocol)) == -1) {
	std::cout <<"Socket error "<<sockfd<<std::endl;
	return 1;
    }

    if (connect(sockfd, serv->ai_addr, serv->ai_addrlen)==-1){
	std::cout <<"Couldn't connect to server."<<std::endl;
	return 1;
    }
*/
    //memset(recvBuff, '0',sizeof(recvBuff));
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    }

    memset(&serv_addr, '\0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(12345);

    if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0)
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
    while(1)
    {
        helpStr.clear();
        helpStr.str(std::string());

        buffer = new char[len];
        std::cin.getline(buffer,len);
        sendStr = buffer;
        if (!sendStr.compare("exit")) break;

        helpStr << sendStr.size();
        std::cout << sendStr.size()<< " ";
        std::cout << helpStr.str() << std::endl;

        res = send(sockfd,helpStr.str().c_str(),32,0);
        if (res < 1) std::cout << "Error while trying to send" <<std::endl;
        res = send(sockfd,sendStr.c_str(),sendStr.size(),0);
        if (res < 1) std::cout << "Error while trying to send" <<std::endl;

        delete [] buffer;
    }
    //freeaddrinfo(serv);
    delete [] buffer;
    close(sockfd);
    return 0;
}
