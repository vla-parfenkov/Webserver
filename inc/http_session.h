//
// Created by parfenkov on 29.03.18.
//

#ifndef WEBSERVER_HTTP_SESSION_H
#define WEBSERVER_HTTP_SESSION_H

#define  BUFFER_SIZE 4096

#include <fstream>
#include "http_handler.h"


enum ClientStatus{
    WANT_READ, WANT_HEADER, WANT_FILE, WANT_RESPONCE, WANT_CLOSE
};


class CHTTPSession {
private:

    int fd;
    std::string request;
    std::string responceHeader;
    std::string filePath;
    std::string responce;
    std::ifstream file;
    ClientStatus clientStatus;
    CHTTPHandler* handler;
    int epollfd;
    ssize_t leftData;
    ssize_t sentData;
    size_t fileBufferCount;
    char fileBuffer[BUFFER_SIZE];
    bool flagEAGAIN;

    //bool write(const std::string &message);
    bool write (const char *data, size_t size);
    void mod(uint32_t flag);

public:

    CHTTPSession(int fd, CHTTPHandler* handler, int epoll);
    ~CHTTPSession();

    void Init();
    void Read();
    void RecvHeader();
    void RecvFile();
    void RecvResponce();
    ClientStatus Status();
    void Close();
    void SetStatus(ClientStatus status);



};


#endif //WEBSERVER_HTTP_SESSION_H
