//
// Created by parfenkov on 03.03.18.
//

#include "threadpool.h"

CThreadPool::CThreadPool(size_t threadCount) : stop(false) {
    init(threadCount);
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


void CThreadPool::AddTask(std::function<void()> task) {
    std::unique_lock<std::mutex> lock(queueMutex);
    tasks.push(task);
}

void CThreadPool::worker() {
    while(!stop) {
        std::function<void()> task;
        pop(&task);
        if (task) {
            task();
        } else {
            std::this_thread::yield();
        }

    }
}

void CThreadPool::init(size_t threadCount) {
    try {
        for (size_t i = 0; i < threadCount; ++i) {
            threads.push_back(std::thread(&CThreadPool::worker, this));
        }
    } catch (...)
    {
        stop = true;
        throw;
    }
}

void CThreadPool::pop(std::function<void()>* task) {
    std::unique_lock<std::mutex> lock(queueMutex);
    if (!tasks.empty()) {
        *task = std::move(tasks.front());
        tasks.pop();
    }
}