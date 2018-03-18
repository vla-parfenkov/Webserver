//
// Created by parfenkov on 04.03.18.
//

#ifndef WEBSERVER_СLIENT_H
#define WEBSERVER_СLIENT_H

#define MAX_REQUEST_LEN 4096

#include <iostream>
#include "http_request.h"
#include "threadpool.h"
#include "client_buffer.h"


class CHTTPRequest;

class CClient {
private:
    CHTTPRequest* httpRequest;
    CThreadPool* threadPool;
    CClientsBuffer* clientsBuffer;

public:
    CClient(const std::string& root, CThreadPool* threadPoll, CClientsBuffer* cClientsBuffer);
    ~CClient();

    void Write(int fd, const std::string& message);
    void Read(int fd);
    void Close(int fd);

};





#endif //WEBSERVER_СLIENT_H
