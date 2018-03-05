//
// Created by parfenkov on 04.03.18.
//

#ifndef WEBSERVER_СLIENT_H
#define WEBSERVER_СLIENT_H

#define MAX_REQUEST_LEN 4096

#include <iostream>
#include "http_request.h"
#include "threadpool.h"

enum ClientStatus{
    WANT_READ, WANT_WRITE, WANT_CLOSE
};

struct ClientBuf {
    int fd;
    std::string data;
    ClientStatus status;
    ClientBuf(int fd) : status(WANT_READ) {}
};


class CClient {
private:
    CHTTPRequest* httpRequest;
    CThreadPool* threadPool;

public:
    CClient(const std::string& root, CThreadPool* threadPoll);
    ~CClient();

    void Write(int fd, const std::string& message);
    void Read(ClientBuf* buf);
    void Close(int fd);

};





#endif //WEBSERVER_СLIENT_H
