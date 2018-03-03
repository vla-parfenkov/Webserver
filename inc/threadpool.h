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

#define MIN_THREADS_COUNT 2

class CThreadPool {

private:

    std::vector<std::thread> threads;
    std::queue<std::function<void()>> tasks;
    std::mutex queueMutex;
    std::atomic_bool stop;

    void worker();
    void init(size_t threadCount);
    void pop(std::function<void()>* task);

public:

    CThreadPool(size_t threadCount);
    CThreadPool();
    ~CThreadPool();

    void AddTask(std::function<void()> task);

};


#endif //WEBSERVER_THREADPOOL_H
