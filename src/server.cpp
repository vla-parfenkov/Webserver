//
// Created by parfenkov on 24.02.18.
//

#include <arpa/inet.h>
#include <cstring>
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>
#include "server.h"

CServer::CServer(const std::string &addr, const std::uint16_t &port, const std::uint32_t& queueSize,
                 const std::string& root, size_t threadCount) : stop(false){
    threadPool = new CThreadPool(threadCount);
    handler = new CHTTPHandler(root);
    epollEngine = new CEpollEngine(MAX_EPOLL_EVENT, EPOLL_TIMEOUT, handler, threadPool);
    sockaddr_in serveraddr;

    listenfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenfd < 0 ) {
        throw std::runtime_error("exception socket create" + std::string(strerror(errno)));
    }

    int flags = fcntl(listenfd, F_GETFL, 0);
    if (fcntl(listenfd, F_SETFL, flags | O_NONBLOCK) == -1) {
        throw std::runtime_error("nonblocked: " + std::string(strerror(errno)));
    }

    serveraddr.sin_family = AF_INET;
    inet_pton(AF_INET, addr.data(), &(serveraddr.sin_addr));
    serveraddr.sin_port  = htons(port);
    //serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

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
    delete handler;
    close(listenfd);
}

void CServer::Listen() {
    sockaddr_in clientaddr;
    socklen_t clientlen;

    while(!stop) {
        ssize_t coonectionfd = accept(listenfd, (sockaddr*)&clientaddr, &clientlen);
        if (coonectionfd >= 0) {
            int flags = fcntl((int)coonectionfd, F_GETFL, 0);
            if (fcntl((int)coonectionfd, F_SETFL, flags | O_NONBLOCK) != -1) {
                epollEngine->AddFd((int)coonectionfd);
            }
        }
    }
}

void CServer::Stop() {
    stop = true;
}
