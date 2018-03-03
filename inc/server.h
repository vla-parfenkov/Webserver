//
// Created by parfenkov on 24.02.18.
//

#ifndef WEBSERVER_SERVER_H
#define WEBSERVER_SERVER_H


#include <glob.h>
#include <netinet/in.h>
#include <string>

#include "threadpool.h"
#include "epollengine.h"

#define MAX_EPOLL_EVENT 300
#define EPOLL_TIMEOUT 50


class CServer {
private:

    int listenfd;
    sockaddr_in serveraddr;
    bool stop;
    CEpollEngine* epollEngine;
    CThreadPool* threadPool;

public:

    CServer(const std::string& addr, const std::uint16_t& port,const std::uint32_t& queueSize);
    ~CServer();

    void Listen();
    void Stop();

};


#endif //WEBSERVER_SERVER_H
