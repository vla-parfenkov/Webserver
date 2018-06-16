//
// Created by parfenkov on 24.02.18.
//

#include <arpa/inet.h>
#include <cstring>
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/epoll.h>
#include "server.h"

CServer::CServer(const std::string &addr, const std::uint16_t &port, const std::uint32_t& queueSize,
                 const std::string& root, size_t threadCount) : stop(false){
    std::cout << "Server Parameters: " << std::endl;
    std::cout << "root: " << root << std::endl;
    std::cout << "thread: " << threadCount << std::endl;
    std::cout << "address: " << addr << std::endl;
    std::cout << "port: " << port << std::endl;
    handler = new CHTTPHandler(root);
    threadPool = new CThreadPool(threadCount, MAX_EPOLL_EVENT, EPOLL_TIMEOUT, handler);
    epollEngine = new CEpollEngine(MAX_EPOLL_EVENT, EPOLL_TIMEOUT);
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

    epollEngine->AddFd(listenfd, epollEngine->Epollfd());
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
        epoll_event events[MAX_EPOLL_EVENT];
        ssize_t fdcount = epollEngine->Wait(events);
        for (uint32_t  i = 0; i < fdcount; ++i) {
            int fd = events[i].data.fd;
            if (fd == listenfd) {
                ssize_t coonectionfd = accept(listenfd, (sockaddr*)&clientaddr, &clientlen);
                if (coonectionfd >= 0) {
                    int flags = fcntl((int)coonectionfd, F_GETFL, 0);
                    if (fcntl((int)coonectionfd, F_SETFL, flags | O_NONBLOCK) != -1) {
                        std::function<int(int)> Add;
                        Add = std::bind(&CEpollEngine::AddFd, &*epollEngine, coonectionfd, std::placeholders::_1 );

                        threadPool->AddTask(Add);
                    }
                }
            }
        }
    }
}

void CServer::Stop() {
    stop = true;
}
