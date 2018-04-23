//
// Created by parfenkov on 29.03.18.
//

#include <stdio.h>
#include <sys/socket.h>
#include <cerrno>
#include <stdexcept>
#include <cstring>
#include <bits/ios_base.h>
#include "http_session.h"
#include <unistd.h>

#include <sys/epoll.h>
#include <bits/signum.h>


CHTTPSession::CHTTPSession(int fd, CHTTPHandler* handler, int epoll) : fd(fd), handler(handler), epollfd(epoll),
                                                                       clientStatus(WANT_READ){
}
//CHTTPSession::CHTTPSession () {}

CHTTPSession::~CHTTPSession() {
}

/*void CHTTPSession::Init(int newfd, CHTTPHandler* newhandler, int epoll) {
    fd = newfd;
    handler = newhandler;
    epollfd
}*/

void CHTTPSession::mod(uint32_t flag) {
    epoll_event ev;
    ev.events = flag | EPOLLERR | EPOLLHUP | EPOLLET | EPOLLONESHOT;
    ev.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev);
}


void CHTTPSession::Close() {
    request.clear();
    responce.clear();
    responceHeader.clear();
    filePath.clear();
    file.clear();
    std::cout <<"close " << fd << std::endl;
    close(fd);
}

void CHTTPSession::Read() {
    char buf[ BUFFER_SIZE];
    std::cout <<"read " << fd << std::endl;
    ssize_t n = recv(fd, buf, sizeof(buf), 0);
    std::cout <<"read finish " << fd << std::endl;
    if (n == -1 && errno != EAGAIN) {
        throw std::runtime_error("read: " + std::string(strerror(errno)));
    }
    if( errno != EAGAIN || errno != EWOULDBLOCK  ) {
        request.append(buf, (size_t) n);
        try {

            handler->RequestHandle(request, responceHeader, filePath, responce);
            clientStatus = WANT_HEADER;
            mod(EPOLLOUT);
        } catch (std::runtime_error) {
            mod(EPOLLIN);
            return;
        }
    }
}

void CHTTPSession::RecvHeader() {
    try {
        if( write(responceHeader) ) {
            if (!filePath.empty()) {
                file.open(filePath, std::ifstream::in);
                clientStatus = WANT_FILE;
            } else {
                if (!responce.empty()) {
                clientStatus = WANT_RESPONCE;
                } else {
                    clientStatus = WANT_CLOSE;
                    //close(fd);
                }
            }
            mod(EPOLLOUT);
        }
    } catch (std::runtime_error ex) {
        throw std::runtime_error("send: " + std::string(strerror(errno)));
    }
}

void CHTTPSession::RecvFile() {
    char buffer[BUFFER_SIZE];
    size_t bufferCount;
    if (file.is_open()) {
           while ((bufferCount = (size_t) file.readsome(buffer, BUFFER_SIZE)) > 0) {
               try {
                   write(std::string(buffer, bufferCount));
               } catch (...) {

               }
            }
        file.close();
        clientStatus = WANT_CLOSE;
        //Close();
        mod(EPOLLOUT);
        }
    }


/*bool CHTTPSession::write(const char *data, size_t size) {
    if (leftData > 0) {
        std::cout << "write " << fd << std::endl;
        sentData = send(fd, data + sentData, size - sentData, 0);
        if (sentData == -1) {
            throw std::runtime_error("send: " + std::string(strerror(errno)));
        }
        leftData -= sentData;
    }
    return leftData == 0;
}*/

bool CHTTPSession::write(const std::string &message) {
    size_t left = message.size();
    ssize_t sent = 0;

    while (left > 0) {
        std::cout <<"write " << fd << std::endl;
        sent = send(fd, message.data() + sent, message.size() - sent, 0);
        if (sent == -1) {
            throw std::runtime_error("send: " + std::string(strerror(errno)));
        }
        left -= sent;
    }
    std::cout <<"write finish " << fd << std::endl;
}

void CHTTPSession::RecvResponce() {
    try {
        if( write(responce) ) {
            clientStatus = WANT_CLOSE;
            mod(EPOLLOUT);
        }
    } catch (std::runtime_error ex) {
        throw std::runtime_error("send: " + std::string(strerror(errno)));
    }
}

ClientStatus CHTTPSession::Status() {
    return clientStatus;
}

void CHTTPSession::SetStatus(ClientStatus status) {
    clientStatus = status;
}

