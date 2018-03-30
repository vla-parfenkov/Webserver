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


CHTTPSession::CHTTPSession(int fd, CHTTPHandler* handler) : fd(fd), clientStatus(WANT_READ),  handler(handler) {
}

CHTTPSession::~CHTTPSession() {
}

void CHTTPSession::Read() {
    char buf[ BUFFER_SIZE];
    ssize_t n = recv(fd, buf, sizeof(buf), 0);
    if (n == -1 && errno != EAGAIN) {
        throw std::runtime_error("read: " + std::string(strerror(errno)));
    }
    request.append(buf, (size_t) n);
    try {

        handler->RequestHandle(request, responceHeader, filePath, responce);
        clientStatus = WANT_HEADER;
        RecvHeader();
    } catch (std::runtime_error) {
        return;
    }
}

void CHTTPSession::RecvHeader() {
    try {
        if( write(responceHeader) ) {
            if (!filePath.empty()) {
                file.open(filePath, std::ifstream::in);
                clientStatus = WANT_FILE;
                RecvFile();
            } else {
                if (!responce.empty()) {
                clientStatus = WANT_RESPONCE;
                    RecvResponce();
                }
            }
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
                write(std::string(buffer, bufferCount));
            }
        file.close();
        clientStatus = WANT_CLOSE;
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
        sent = send(fd, message.data() + sent, message.size() - sent, 0);
        if (sent == -1) {
            throw std::runtime_error("send: " + std::string(strerror(errno)));
        }
        left -= sent;
    }
}

void CHTTPSession::RecvResponce() {
    try {
        if( write(responce) ) {
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

