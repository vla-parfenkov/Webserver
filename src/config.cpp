//
// Created by parfenkov on 29.03.18.
//

#include <fstream>
#include "config.h"



void CConfig::parse() {
    std::ifstream conf;
    conf.open(path);
    std::string temp;

   while (!conf.eof()) {
       conf >> temp;
        if (temp == ADDR) {
            conf >> addr;
        }
        if (temp == ROOT) {
            conf >> root;
        }
        if (temp == PORT) {
            conf >> port;
        }
        if (temp == CPU) {
            conf >> cpu;
        }
        if (temp == THREAD) {
            conf >> thread;
        }
        if (temp == QUEUE) {
            conf >> queueSize;
        }
    }
    isValid = true;
    conf.close();
}

CConfig::CConfig(std::string path) : path(path + "/httpd.conf") {
    parse();
}

CConfig::~CConfig() {

}

std::uint16_t CConfig::Port() {
    return port;
}

std::uint16_t CConfig::Thread() {
    return thread;
}

std::string CConfig::Addr() {
    return addr;
}

bool CConfig::IsValid() {
    return isValid;
}

std::string CConfig::Root() {
    return root;
}

std::uint32_t CConfig::QueueSize() {
    return queueSize;
}
