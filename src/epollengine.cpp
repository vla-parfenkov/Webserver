//
// Created by parfenkov on 03.03.18.
//


#include "epollengine.h"



#include <unistd.h>

CEpollEngine::CEpollEngine(int maxEpollEvents, int timeout) :
        maxEpollEvents(maxEpollEvents), timeout(timeout), stop(false) {
    epollfd = epoll_create(maxEpollEvents);

}

int CEpollEngine::AddFd(int clientfd, int epollfd) {
    epoll_event ev;
    ev.events = EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLOUT ;
    ev.data.fd = clientfd;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, clientfd, &ev);
    return clientfd;
}


ssize_t CEpollEngine::Wait(epoll_event* events) {
    return epoll_wait(epollfd, events, maxEpollEvents, timeout);
}


int CEpollEngine::Epollfd() {
    return epollfd;
}

CEpollEngine::~CEpollEngine() {}