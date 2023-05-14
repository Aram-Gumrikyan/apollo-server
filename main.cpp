#include <iostream>
#include <thread>
#include "src/InetSocket.h"
#include "src/UserInput.h"

using namespace std;

enum app_behavior {
    server = 0,
    client = 1,
};

void handleAppBehavior() {
    app_behavior applicationBehavior = app_behavior::server;
    UserInput::prompt((int *) &applicationBehavior, "Please enter app behavior");

    int port = 0;
    UserInput::prompt(&port, "Enter port number");

    InetSocket inetSocket(port);

    switch (applicationBehavior) {
        case app_behavior::server: {
            inetSocket.startRequestAcceptingLoop();
            break;
        }
        case app_behavior::client: {
            inetSocket.sendRequest();
            break;
        }
    }
}

int main() {
    std::cout << "\033[1;32m APOLLO \033[0m\n" << std::endl;

    std::thread networkThread(handleAppBehavior);
    networkThread.join();

    return 0;
}
