//
// Created by parfenkov on 03.03.18.
//

#include <cstring>
#include <sys/epoll.h>
#include <epollengine.h>
#include "threadpool.h"

CThreadPool::CThreadPool(size_t threadCount, int maxEpollEvents, int timeout, CHTTPHandler* handler) : stop(false)
        , maxEpollEvents(maxEpollEvents), timeout(timeout){
    init(threadCount);
    this->handler = handler;
}

CThreadPool::CThreadPool() : stop(false) {
    size_t threadCount = std::max((unsigned)MIN_THREADS_COUNT, std::thread::hardware_concurrency());
    init(threadCount);
}

CThreadPool::~CThreadPool() {
    stop = true;
    for (auto& thread : threads) {
        thread.join();
    }
}


void CThreadPool::AddTask(std::function<int(int)> task) {
    std::unique_lock<std::mutex> lock(queueMutex);
    tasks.push(task);
}



void CThreadPool::worker() {
    CEpollEngine* epollEngine = new CEpollEngine(maxEpollEvents, timeout);
    std::map<int, CHTTPSession*> sessionMap;

    while(!stop) {
        std::function<int(int)> task;
        pop(&task);
        if (task) {
            int clientfd = task(epollEngine->Epollfd());

            sessionMap.insert(std::pair<int, CHTTPSession*>(clientfd, new CHTTPSession(clientfd, handler,
                                                                                       epollEngine->Epollfd())));

        } else {
            epoll_event events[maxEpollEvents];
            ssize_t fdcount = epollEngine->Wait(events);
            for (uint32_t  i = 0; i < fdcount; ++i) {
                int fd = events[i].data.fd;
                CHTTPSession *session;
               session = sessionMap.at(fd);

                if (events[i].events & EPOLLIN) {
                    if (session->Status() == WANT_READ) {
                        session->Read();
                    }
                } else if (events[i].events & EPOLLOUT) {
                    switch (session->Status()) {
                        case WANT_HEADER : {
                            session->RecvHeader();
                            break;
                        }
                        case WANT_RESPONCE: {

                            session->RecvResponce();
                            break;
                        }
                        case WANT_FILE: {
                            session->RecvFile();
                            break;
                        }
                        case WANT_CLOSE: {
                            epoll_ctl(epollEngine->Epollfd(), EPOLL_CTL_DEL, fd, 0);
                            session->Close();
                            delete session;
                            sessionMap.erase(fd);
                        }
                        default:
                            break;
                    }
                } else if (events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) {
                    epoll_ctl(epollEngine->Epollfd(), EPOLL_CTL_DEL, fd, 0);
                    session->Close();
                    delete session;
                    sessionMap.erase(fd);
                }
            }
            }
        }

    delete  epollEngine;
}

void CThreadPool::init(size_t threadCount) {
    try {
        for (size_t i = 0; i < threadCount; ++i) {
            threads.push_back(std::thread(&CThreadPool::worker, this));
        }
    } catch (...)
    {
        stop = true;
        throw std::runtime_error("threads create" + std::string(strerror(errno)));;
    }
}

void CThreadPool::pop(std::function<int(int)>* task) {
    std::unique_lock<std::mutex> lock(queueMutex);
    if (!tasks.empty()) {
        *task = std::move(tasks.front());
        tasks.pop();
    }
}