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

#include <cstdint>
#include <iostream>

class CConfig {
private:
    std::string addr;
    std::uint16_t port;
    std::uint16_t cpu;
    std::uint16_t thread;
    bool isValid;
    std::string root;
    std::uint32_t queueSize;

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
