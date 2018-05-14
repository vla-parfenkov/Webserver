//
// Created by parfenkov on 03.03.18.
//

#ifndef WEBSERVER_EPOLLENGINE_H
#define WEBSERVER_EPOLLENGINE_H

#include <atomic>
#include "threadpool.h"
#include <sys/epoll.h>





class CEpollEngine {
private:
    int epollfd;
    int maxEpollEvents;
    int timeout;
    std::atomic_bool stop;


public:
    CEpollEngine(int maxEpollEvents, int timeout);
    ~CEpollEngine();

    //void Run();
    int AddFd(int clientfd, int epollfd);
    //void Stop();
    ssize_t Wait(epoll_event* events);
    int Epollfd();
};


#endif //WEBSERVER_EPOLLENGINE_H
