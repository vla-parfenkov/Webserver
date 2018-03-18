#include <iostream>

#include "server.h"

int main(int argc, char* argv[]) {
    std::string rootDir = argv[0];
    rootDir = rootDir.substr(0, rootDir.find_last_of('/'));

    CServer* server = new CServer("0.0.0.0", 5001, 40, rootDir, 10);

    server->Listen();

    delete server;

    return 0;
}