#include <iostream>

#include "server.h"
#include "config.h"

int main(int argc, char* argv[]) {
    std::string rootDir = argv[0];
    rootDir = rootDir.substr(0, rootDir.find_last_of('/'));
    CConfig* config = new CConfig("/etc");

    CServer* server = new CServer(config->Addr(), config->Port(), config->QueueSize(), config->Root(), config->Thread());

    server->Listen();

    delete server;
    delete  config;

    return 0;
}
