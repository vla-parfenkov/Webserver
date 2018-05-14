//
// Created by parfenkov on 24.02.18.
//

#ifndef WEBSERVER_SERVER_H
#define WEBSERVER_SERVER_H


#include <glob.h>
#include <string>

#include "threadpool.h"
#include "epollengine.h"

#define MAX_EPOLL_EVENT 2000
#define EPOLL_TIMEOUT 50

#include "http_handler.h"

class CServer {
private:

    int listenfd;
    bool stop;
    CEpollEngine* epollEngine;
    CThreadPool* threadPool;
    CHTTPHandler* handler;

public:

    CServer(const std::string& addr, const std::uint16_t& port,const std::uint32_t& queueSize,
            const std::string& root, size_t threadCount);
    ~CServer();

    void Listen();
    void Stop();

};


#endif //WEBSERVER_SERVER_H
