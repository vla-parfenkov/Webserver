//
// Created by parfenkov on 03.03.18.
//

#ifndef WEBSERVER_EPOLLENGINE_H
#define WEBSERVER_EPOLLENGINE_H

#include <atomic>

class CEpollEngine {
private:
    int epollfd;
    int maxEpollEvents;
    int timeout;
    std::atomic_bool stop;

public:
    CEpollEngine(int maxEpollEvents, int timeout);
    ~CEpollEngine();

    void Run();
    void AddFd(int clientfd);
    void Stop();
};


#endif //WEBSERVER_EPOLLENGINE_H
