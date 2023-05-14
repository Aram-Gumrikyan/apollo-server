//
// Created by aram on 5/1/23.
//

#ifndef APOLLO_SERVER_INETSOCKET_H
#define APOLLO_SERVER_INETSOCKET_H

#include <netinet/in.h>

class InetSocket {
private:
    int socketFd;
    int port;
    struct sockaddr_in socketAddress{};
    unsigned char *iv;
    void createServer();
public:
    explicit InetSocket(int p = 3000);
    ~InetSocket();
    void startRequestAcceptingLoop();
    void sendRequest();
    void closeServer() const;

};


#endif //APOLLO_SERVER_INETSOCKET_H
