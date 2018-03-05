//
// Created by parfenkov on 03.03.18.
//

#include <sys/epoll.h>
#include "epollengine.h"


CEpollEngine::CEpollEngine(int maxEpollEvents, int timeout, CClient* client, CThreadPool* threadPool) :
        maxEpollEvents(maxEpollEvents), timeout(timeout), stop(false), client(client), threadPool(threadPool) {
    epollfd = epoll_create(maxEpollEvents);
}

void CEpollEngine::AddFd(int clientfd) {
    ClientBuf* clientBuf = new ClientBuf(clientfd);
    epoll_event ev;
    ev.events = EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLOUT | EPOLLET;
    ev.data.fd = clientfd;
    ev.data.ptr = clientBuf;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, clientfd, &ev);
}


void CEpollEngine::Run() {
    epoll_event events[maxEpollEvents];
    while (!stop) {
        ssize_t fdcount = epoll_wait(epollfd, events, maxEpollEvents, timeout);
        for (size_t i = 0; i < fdcount; ++i) {
            if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP)){
                ClientBuf* clientBuf = (ClientBuf*) events->data.ptr;
                delete clientBuf;
                client->Close(events->data.fd);
            }
            if (events[i].events & EPOLLIN) {
                ClientBuf* clientBuf = (ClientBuf*) events->data.ptr;
                if(clientBuf->status == WANT_READ) {
                    std::function<void()> onRead;
                    onRead = std::bind(&CClient::Read, &*client, clientBuf);
                    threadPool->AddTask(onRead);
                }
            }
            if (events[i].events & EPOLLOUT) {
                //client send
            }
        }
    }
}

void CEpollEngine::Stop() {
    stop = false;
}

CEpollEngine::~CEpollEngine() {}