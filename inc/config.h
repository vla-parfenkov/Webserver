//
// Created by parfenkov on 29.03.18.
//

#ifndef WEBSERVER_CONFIG_H
#define WEBSERVER_CONFIG_H

#define ADDR "addr"
#define PORT "port"
#define CPU "cpu_limit"
#define THREAD "thread"
#define ROOT "document_root"
#define QUEUE "queue_size"

#define DEFAULT_ADDR "0.0.0.0"
#define DEFAULT_PORT 80
#define DEFAULT_CPU 1
#define DEFAULT_THREAD 1
#define DEFAULT_ROOT "/"
#define DEFAULT_QUEUE 1024

#include <cstdint>
#include <iostream>

class CConfig {
private:
    std::string addr = DEFAULT_ADDR;
    std::uint16_t port = DEFAULT_PORT;
    std::uint16_t cpu = DEFAULT_CPU;
    std::uint16_t thread = DEFAULT_THREAD;
    bool isValid;
    std::string root = DEFAULT_ROOT;
    std::uint32_t queueSize = DEFAULT_QUEUE;

    std::string path;

    void parse();


public:
    CConfig(std::string path);
    ~CConfig();

    std::uint16_t Port();
    std::uint16_t Thread();
    std::string Addr();
    std::string Root();
    std::uint32_t QueueSize();
    std::uint16_t Cpu();
    bool IsValid();



};


#endif //WEBSERVER_CONFIG_H
