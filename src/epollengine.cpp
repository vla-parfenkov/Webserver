//
// Created by parfenkov on 03.03.18.
//

#include <sys/epoll.h>
#include "epollengine.h"
#include <unistd.h>





CEpollEngine::CEpollEngine(int maxEpollEvents, int timeout, CHTTPHandler* handler, CThreadPool* threadPool) :
        maxEpollEvents(maxEpollEvents), timeout(timeout), stop(false), handler(handler),
        threadPool(threadPool) {
    epollfd = epoll_create(maxEpollEvents);
}

void CEpollEngine::AddFd(int clientfd) {
    sessionMap.insert(std::pair<int, CHTTPSession*>(clientfd, new CHTTPSession(clientfd,handler)));
    epoll_event ev;
    ev.events = EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLOUT | EPOLLET ;
    ev.data.fd = clientfd;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, clientfd, &ev);
}


void CEpollEngine::Run() {
    epoll_event events[maxEpollEvents];
    while (!stop) {
        ssize_t fdcount = epoll_wait(epollfd, events, maxEpollEvents, timeout);
        for (uint32_t  i = 0; i < fdcount; ++i) {
            std::cout <<"read";
            int fd = events[i].data.fd;
            CHTTPSession* session = sessionMap.at(fd);
            if (events[i].events & EPOLLIN) {
                if (session->Status() == WANT_READ) {
                    std::function<void()> onRead;

                    onRead = std::bind(&CHTTPSession::Read, &*session);
                    threadPool->AddTask(onRead);
                }
            }
           /*if (events[i].events & EPOLLOUT) {
                switch (session->Status()) {
                    case WANT_HEADER : {
                        std::cout << "header " << fd << std::endl;
                        std::function<void()> onWriteHeader;
                        onWriteHeader = std::bind(&CHTTPSession::RecvHeader, &*session);
                        session->SetStatus(USED);
                        threadPool->AddTask(onWriteHeader);
                        break;
                    }
                    case WANT_RESPONCE: {
                        std::cout << "responce " << fd << std::endl;
                        std::function<void()> onWriteResponce;
                        onWriteResponce = std::bind(&CHTTPSession::RecvResponce, &*session);
                        session->SetStatus(USED);
                        threadPool->AddTask(onWriteResponce);
                        break;
                    }
                    case WANT_FILE: {
                        std::cout << "file " << fd << std::endl;
                        std::function<void()> onWriteFile;
                        onWriteFile = std::bind(&CHTTPSession::RecvFile, &*session);
                        session->SetStatus(USED);
                        threadPool->AddTask(onWriteFile);
                        break;
                    }
                  case WANT_CLOSE: {
                        delete session;
                        //sessionMap.erase(fd);
                        //close(fd);
                        break;
                    }
                    default:
                        break;

                }
            }*/
        }
    }
}

void CEpollEngine::Stop() {
    stop = false;
}

CEpollEngine::~CEpollEngine() {}