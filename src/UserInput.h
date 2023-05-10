//
// Created by aram on 5/9/23.
//

#ifndef APOLLO_SERVER_USERINPUT_H
#define APOLLO_SERVER_USERINPUT_H

#include "iostream"

class UserInput {
public:
    template<class T>
    static void prompt(T *outputEndpoint, const std::string &msg, const std::string &separator = ": ");
};

template<class T>
void UserInput::prompt(T *outputEndpoint, const std::string &msg, const std::string &separator) {
    std::cout << msg << separator;
    std::cin >> *outputEndpoint;
}

#endif //APOLLO_SERVER_USERINPUT_H
