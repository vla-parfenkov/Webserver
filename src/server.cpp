//
// Created by parfenkov on 24.02.18.
//

#include <arpa/inet.h>
#include "server.h"

CServer::CServer(const std::string &addr, const std::uint16_t &port) {

    epoll_event event;
    epollfd = epoll_create(EPOLL_FD_COUNT);
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    event.data.fd = epollfd;
    event.events = EPOLLIN;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &event);

    serveraddr.sin_family = AF_INET;
    inet_pton(AF_INET, addr.data(), &(serveraddr.sin_addr));
    serveraddr.sin_port  = htons(port);

    bind(listenfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));

    listen(listenfd, LISTEN_QUEUE);
}

CServer::~CServer() {

}

void CServer::Listen() {
    /*for(;;) {
        int fdcount = epoll_wait(epollfd, events, LISTEN_QUEUE, TIMEOUT);



    }*/
}

void CServer::Stop() {

}
