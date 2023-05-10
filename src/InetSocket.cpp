//
// Created by aram on 5/1/23.
//

#include <iostream>
#include <sys/socket.h>
#include <thread>
#include <arpa/inet.h>
#include "InetSocket.h"
#include "UserInput.h"

#define BUFFER_SIZE 1024

InetSocket::InetSocket(int p) : port(p), socketFd(-1) {
    socketAddress.sin_family = AF_INET;
    socketAddress.sin_addr.s_addr = INADDR_ANY;
    socketAddress.sin_port = htons(port);

    this->createServer();
}

InetSocket::~InetSocket() {
    closeServer();
}

void InetSocket::createServer() {
    socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFd == -1) {
        std::cerr << "Error can't create socket";
        exit(-1);
    }
}

void InetSocket::closeServer() const {
    close(socketFd);
}

void InetSocket::startRequestAcceptingLoop() {
    /**
     * binding our socket to address
     */
    if (bind(socketFd, (const sockaddr *) &socketAddress, sizeof(socketAddress)) == -1) {
        std::cerr << "Error can't bind address to socket";
        exit(-2);
    }

    /**
     * starting to listen port.
     * this means that we can accept incoming requests
     * 1 request at time. all other requests is queued
     */
    if (listen(socketFd, 1) == -1) {
        std::cerr << "Error listening to socket is failed";
        exit(-3);
    }

    while (true) {
        int clientSocketFd = accept(socketFd, nullptr, nullptr);
        if (clientSocketFd == -1) {
            perror("accept: ");
            exit(-4);
        }

        char buffer[BUFFER_SIZE] = {0};
        read(clientSocketFd, buffer, BUFFER_SIZE);
        std::cout << "Message received: " << buffer << std::endl;

        char response[BUFFER_SIZE] = {};
        UserInput::prompt(&response, "Submit answer");

        if (response[0] == 'q' && response[1] == ' ') {
            break;
        }

        send(clientSocketFd, response, BUFFER_SIZE, 0);

//        close(clientSocketFd);
    }
}

void InetSocket::sendRequest() {
    if (connect(socketFd, (const sockaddr *) &socketAddress, sizeof(socketAddress)) < 0) {
        std::cerr << "Failed to connect to server";
        exit(-5);
    }

    while (true) {
        char request[BUFFER_SIZE] = {};
        UserInput::prompt(&request, "Enter message");

        if (request[0] == 'q' && request[1] == ' ') {
            break;
        }

        //    implement rsa

        send(socketFd, request, BUFFER_SIZE, 0);


        std::string response;
        char buffer[BUFFER_SIZE] = {0};
        long bytes_received;
        while ((bytes_received = recv(socketFd, buffer, BUFFER_SIZE, 0)) > 0) {
            response.append(buffer, bytes_received);
        }
        if (bytes_received == -1) {
            std::cerr << "Failed to receive HTTP response";
            exit(-7);
        }

        std::cout << "response: " << response << std::endl;

//        closeServer();
//        createServer();
    }
}

