//
// Created by parfenkov on 24.02.18.
//

#ifndef WEBSERVER_SERVER_H
#define WEBSERVER_SERVER_H


#define MAX_EPOLL_EVENTS 1024
#define EPOLL_FD_COUNT 1024
#define LISTEN_QUEUE 1024
#define TIMEOUT 50

#include <glob.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <string>

class CServer {
private:

    int listenfd;
    sockaddr_in serveraddr;

    int epollfd;
    epoll_event events[MAX_EPOLL_EVENTS];

public:

    CServer(const std::string& addr, const std::uint16_t& port);
    ~CServer();

    void Listen();
    void Stop();

};


#endif //WEBSERVER_SERVER_H
