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



CHTTPSession::CHTTPSession(int fd, CHTTPHandler* handler, int epoll) : fd(fd), handler(handler), epollfd(epoll),
                                                                       clientStatus(WANT_READ), flagEAGAIN(false){
}


CHTTPSession::~CHTTPSession() {
}


void CHTTPSession::mod(uint32_t flag) {
    epoll_event ev;
    ev.events = flag | EPOLLERR | EPOLLHUP | EPOLLET;
    ev.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev);
}


void CHTTPSession::Close() {
    request.clear();
    responce.clear();
    responceHeader.clear();
    filePath.clear();
    file.clear();
    close(fd);
}

void CHTTPSession::Read() {
    char buf[ BUFFER_SIZE];
    ssize_t n = recv(fd, buf, sizeof(buf), 0);
    if (n == -1 && errno != EAGAIN) {
        throw std::runtime_error("read: " + std::string(strerror(errno)));
    }
    if( errno != EAGAIN || errno != EWOULDBLOCK  ) {
        request.append(buf, (size_t) n);
        try {

            handler->RequestHandle(request, responceHeader, filePath, responce);
            leftData = responceHeader.size();
            sentData = 0;
            clientStatus = WANT_HEADER;
        } catch (std::runtime_error) {
            return;
        }
    }
}

void CHTTPSession::RecvHeader() {
    try {
        if( write(responceHeader.data(), responceHeader.size()) ) {
            if (!filePath.empty()) {
                file.open(filePath, std::ifstream::in);

                clientStatus = WANT_FILE;
            } else {
                if (!responce.empty()) {
                    leftData = responce.size();
                    sentData = 0;
                clientStatus = WANT_RESPONCE;
                } else {
                    clientStatus = WANT_CLOSE;
                }
            }
           // mod(EPOLLOUT);
        }
    } catch (std::runtime_error ex) {
        throw std::runtime_error("send: " + std::string(strerror(errno)));
    }
}

void CHTTPSession::RecvFile() {
    if (flagEAGAIN) {
        while (leftData > 0) {
            try {
                write(fileBuffer,fileBufferCount);
            } catch (std::runtime_error) {
                flagEAGAIN = true;
                return;
            }
        }
        flagEAGAIN = false;
    }
    if (file.is_open()) {
        while ((fileBufferCount = (size_t) file.readsome(fileBuffer, BUFFER_SIZE)) > 0) {
            leftData = fileBufferCount;
            sentData = 0;
            while (leftData > 0) {
                try {
                    write(fileBuffer,fileBufferCount);
                } catch (std::runtime_error) {
                    flagEAGAIN = true;
                    return;
                }
            }
        }
        file.close();
        clientStatus = WANT_CLOSE;
    }

}



bool CHTTPSession::write(const char *data, size_t size) {
    ssize_t sendResult;
    if (leftData > 0) {
        sendResult = send(fd, data + sentData, size - sentData, 0);
        if (sendResult == -1) {
            throw std::runtime_error("send: " + std::string(strerror(errno)));
        }
        sentData = sendResult;
        leftData -= sentData;
    }
    return leftData == 0;
}

/*bool CHTTPSession::write(const std::string &message) {
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
}*/

void CHTTPSession::RecvResponce() {
    try {
        if( write(responce.data(), responce.size()) ) {
            clientStatus = WANT_CLOSE;
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

