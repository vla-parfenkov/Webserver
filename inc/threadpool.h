//
// Created by parfenkov on 03.03.18.
//

#ifndef WEBSERVER_THREADPOOL_H
#define WEBSERVER_THREADPOOL_H


#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <atomic>
#include <map>
#include "http_session.h"


#define MIN_THREADS_COUNT 1

class CThreadPool {

private:

    std::vector<std::thread> threads;
    std::queue<std::function<int(int)>> tasks;
    std::mutex queueMutex;
    std::mutex mapMutex;
    std::atomic_bool stop;
    int maxEpollEvents;
    int timeout;

    CHTTPHandler* handler;

    void worker();
    void init(size_t threadCount);
    void pop(std::function<int(int)>* task);


public:

    CThreadPool(size_t threadCount, int maxEpollEvents, int timeout, CHTTPHandler* handler );
    CThreadPool();
    ~CThreadPool();

    void AddTask(std::function<int(int)> task);


};


#endif //WEBSERVER_THREADPOOL_H
