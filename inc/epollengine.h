//
// Created by parfenkov on 03.03.18.
//

#ifndef WEBSERVER_EPOLLENGINE_H
#define WEBSERVER_EPOLLENGINE_H

#include <atomic>
#include "threadpool.h"
#include "http_session.h"
#include "http_handler.h"
#include <map>




class CEpollEngine {
private:
    int epollfd;
    int maxEpollEvents;
    int timeout;
    std::atomic_bool stop;
    CHTTPHandler* handler;
    CThreadPool* threadPool;
    CHTTPSession* sessions;
    std::map<int, CHTTPSession*> sessionMap;

public:
    CEpollEngine(int maxEpollEvents, int timeout, CHTTPHandler* handler, CThreadPool* threadPool);
    ~CEpollEngine();

    void Run();
    void AddFd(int clientfd);
    void Stop();
};


#endif //WEBSERVER_EPOLLENGINE_H
