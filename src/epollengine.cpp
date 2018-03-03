//
// Created by parfenkov on 03.03.18.
//

#include <sys/epoll.h>
#include "epollengine.h"


CEpollEngine::CEpollEngine(int maxEpollEvents, int timeout) :
        maxEpollEvents(maxEpollEvents), timeout(timeout), stop(false) {
    epollfd = epoll_create(maxEpollEvents);
}

void CEpollEngine::AddFd(int clientfd) {
    epoll_event ev;
    ev.events = EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLOUT;
    ev.data.fd = clientfd;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, clientfd, &ev);
}


void CEpollEngine::Run() {
    epoll_event events[maxEpollEvents];
    while (!stop) {
        int fdcount = epoll_wait(epollfd, events, maxEpollEvents, timeout);
        for (int i = 0; i < fdcount; ++i) {
            if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP)){
                //client dead
            }
            if (events[i].events & EPOLLIN) {
                //client recv
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