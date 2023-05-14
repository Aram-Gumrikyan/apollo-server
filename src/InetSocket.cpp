//
// Created by aram on 5/1/23.
//

#include <iostream>
#include <sys/socket.h>
#include <thread>
#include <arpa/inet.h>
#include "InetSocket.h"
#include "UserInput.h"
#include "../utils/State.h"
#include "../cryptography/rsa.h"
#include "../cryptography/aes.h"

#define BUFFER_SIZE 1024

using std::cout;
using std::endl;
using std::stoi;

InetSocket::InetSocket(int p) : port(p), socketFd(-1) {
    socketAddress.sin_family = AF_INET;
    socketAddress.sin_addr.s_addr = INADDR_ANY;
    socketAddress.sin_port = htons(port);

    iv = (unsigned char *) "0123456789012345";

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

    State serverRunning(true);

    while (serverRunning.getState()) {
        int clientSocketFd = accept(socketFd, nullptr, nullptr);
        if (clientSocketFd == -1) {
            perror("accept: ");
            exit(-4);
        }

        /**
         * server sends public key to client
         */
        Keypair keys{};
        generateKeys(keys);
        string publicKeyPair = std::to_string(keys.pub.n) + "@" + std::to_string(keys.pub.e);
        const char *publicKeyPairBuffer = publicKeyPair.c_str();
        send(clientSocketFd, publicKeyPairBuffer, BUFFER_SIZE, 0);

        /**
         * server sends encryption key for aes
         */
        vector<int> secret{};
        char encryptionKayBuffer[BUFFER_SIZE] = {0};
        read(clientSocketFd, encryptionKayBuffer, BUFFER_SIZE);
        string encryptionKayPlain(encryptionKayBuffer);
        string delimiter = "@";
        size_t pos = 0;
        while ((pos = encryptionKayPlain.find(delimiter)) != string::npos) {
            secret.push_back(stoi(encryptionKayPlain.substr(0, pos)));
            encryptionKayPlain.erase(0, pos + delimiter.length());
        }
        string key = decrypt(keys.pri, secret);

        while (true) {
            unsigned char cipherText[128];
            unsigned char decryptedText[128];
            int decryptedText_len, ciphertext_len;

            unsigned char request[BUFFER_SIZE] = {0};
            read(clientSocketFd, request, BUFFER_SIZE);
            unsigned char requestCipherTextLen[10] = {0};
            read(clientSocketFd, requestCipherTextLen, 10);
            decryptedText_len = aes::decrypt(
                    request,
                    stoi(string(reinterpret_cast<const char *>(requestCipherTextLen))),
                    (unsigned char *) key.c_str(),
                    iv,
                    decryptedText);
            decryptedText[decryptedText_len] = '\0';
            std::cout << "Encrypted Message received: " << request << std::endl;
            std::cout << "Message received: " << decryptedText << std::endl;

            unsigned char response[BUFFER_SIZE] = {};
            UserInput::prompt(&response, "Submit answer");

            if (response[0] == 'q' && response[1] == 0) {
                serverRunning.setState(false);
                break;
            }

            ciphertext_len = aes::encrypt(
                    response,
                    BUFFER_SIZE,
                    (unsigned char *) key.c_str(),
                    iv,
                    cipherText);
            send(clientSocketFd, cipherText, BUFFER_SIZE, 0);
            string ciphertext_len_str = std::to_string(ciphertext_len);
            send(clientSocketFd, ciphertext_len_str.c_str(), ciphertext_len_str.size(), 0);
        }

        close(clientSocketFd);
    }
}

void InetSocket::sendRequest() {
    if (connect(socketFd, (const sockaddr *) &socketAddress, sizeof(socketAddress)) < 0) {
        std::cerr << "Failed to connect to server";
        exit(-5);
    }

    PublicKey serverPublicKey{};
    char serverPublicKeyPairBuffer[BUFFER_SIZE] = {0};
    read(socketFd, serverPublicKeyPairBuffer, BUFFER_SIZE);
    string serverPublicKeyPair(serverPublicKeyPairBuffer);
    size_t delimiter_pos = serverPublicKeyPair.find('@');
    serverPublicKey.n = stoi(serverPublicKeyPair.substr(0, delimiter_pos));
    serverPublicKey.e = stoi(serverPublicKeyPair.substr(delimiter_pos + 1));

    string key = "01234567890123456789012345678901";
    vector<int> encryptedKay = encrypt(serverPublicKey, key);
    string joinedEncryptedKey;
    for (auto i: encryptedKay) {
        joinedEncryptedKey += std::to_string(i) + "@";
    }
    const char *joinedEncryptedKeyBuffer = joinedEncryptedKey.c_str();
    send(socketFd, joinedEncryptedKeyBuffer, BUFFER_SIZE, 0);

    State clientRunning(true);

    while (clientRunning.getState()) {
        unsigned char cipherText[128];
        unsigned char decryptedText[128];
        int decryptedText_len, ciphertext_len;

        unsigned char request[BUFFER_SIZE] = {};
        UserInput::prompt(&request, "Enter message");

        if (request[0] == 'q' && request[1] == ' ') {
            clientRunning.setState(false);
            break;
        }

        ciphertext_len = aes::encrypt(
                request,
                BUFFER_SIZE,
                (unsigned char *) key.c_str(),
                iv,
                cipherText);
        send(socketFd, cipherText, BUFFER_SIZE, 0);
        string ciphertext_len_str = std::to_string(ciphertext_len);
        send(socketFd, ciphertext_len_str.c_str(), ciphertext_len_str.size(), 0);

        unsigned char response[BUFFER_SIZE] = {0};
        read(socketFd, response, BUFFER_SIZE);
        unsigned char responseCipherTextLen[10] = {0};
        read(socketFd, responseCipherTextLen, 10);
        decryptedText_len = aes::decrypt(
                response,
                stoi(string(reinterpret_cast<const char *>(responseCipherTextLen))),
                (unsigned char *) key.c_str(),
                iv,
                decryptedText);
        decryptedText[decryptedText_len] = '\0';
        std::cout << "encrypted response: " << response << std::endl;
        std::cout << "response: " << decryptedText << std::endl;
    }
}

