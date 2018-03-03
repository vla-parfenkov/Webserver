//
// Created by parfenkov on 24.02.18.
//

#include <arpa/inet.h>
#include <cstring>
#include "server.h"

CServer::CServer(const std::string &addr, const std::uint16_t &port, const std::uint32_t& queueSize) : stop(false){
    threadPool = new CThreadPool;
    epollEngine = new CEpollEngine(MAX_EPOLL_EVENT, EPOLL_TIMEOUT);

    listenfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenfd < 0 ) {
        throw std::runtime_error("exception socket create");
    }

    serveraddr.sin_family = AF_INET;
    inet_pton(AF_INET, addr.data(), &(serveraddr.sin_addr));
    serveraddr.sin_port  = htons(port);

    if (bind(listenfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0) {
        throw std::runtime_error("bind: " + std::string(strerror(errno)));
    }

    if ( listen(listenfd, queueSize) < 0) {
        throw std::runtime_error("listen: " + std::string(strerror(errno)));
    }

    std::function<void()> runEpoll;
    runEpoll = std::bind(&CEpollEngine::Run, &*epollEngine);

    threadPool->AddTask(runEpoll);
}

CServer::~CServer() {
    delete epollEngine;
    delete threadPool;
}

void CServer::Listen() {
    sockaddr_in clientaddr;
    socklen_t clientlen;

    while(!stop) {
        int coonectionfd = accept(listenfd, (sockaddr*)&clientaddr, &clientlen);
        if (coonectionfd >= 0) {
            epollEngine->AddFd(coonectionfd);
        }
    }
}

void CServer::Stop() {
    stop = true;
}
