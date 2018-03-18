//
// Created by parfenkov on 03.03.18.
//

#include <sys/epoll.h>
#include "epollengine.h"




CEpollEngine::CEpollEngine(int maxEpollEvents, int timeout, CClient* client, CThreadPool* threadPool,
                           CClientsBuffer* clientsBuffer) :
        maxEpollEvents(maxEpollEvents), timeout(timeout), stop(false), client(client),
        threadPool(threadPool), clientsBuffer(clientsBuffer) {
    epollfd = epoll_create(maxEpollEvents);
}

void CEpollEngine::AddFd(int clientfd) {
    epoll_event ev;
    ev.events = EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLOUT | EPOLLET;
    ev.data.fd = clientfd;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, clientfd, &ev);
    clientsBuffer->AddClient(clientfd);
}


void CEpollEngine::Run() {
    epoll_event events[maxEpollEvents];
    while (!stop) {
        ssize_t fdcount = epoll_wait(epollfd, events, maxEpollEvents, timeout);
        for (uint32_t  i = 0; i < fdcount; ++i) {
            int fd = events[i].data.fd;
            if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP)){
                client->Close(fd);
            }
            if (events[i].events & EPOLLIN) {
                if (clientsBuffer->isWantToRead(fd)) {
                    std::function<void()> onRead;

                    onRead = std::bind(&CClient::Read, &*client, fd);
                    threadPool->AddTask(onRead);
                }
            }
        }
    }
}

void CEpollEngine::Stop() {
    stop = false;
}

CEpollEngine::~CEpollEngine() {}