//
// Created by parfenkov on 04.03.18.
//

#include "сlient.h"
#include <unistd.h>
#include <cstring>

CClient::CClient(int fd) : clientfd(fd) {
}

CClient::~CClient() {
    if (clientfd > 0) {
        Close();
    }
    clientfd = -1;
}



void CClient::Write(const std::string &message) {
    size_t left = message.size();
    ssize_t sent = 0;

    while (left > 0) {
        sent = send(clientfd, message.data() + sent, message.size() - sent, 0);
        if (sent == -1) {
            throw std::runtime_error("send: " + std::string(strerror(errno)));
        }
        left -= sent;
    }
}

std::string CClient::Read() {
    char buf[MAX_REQUEST_LEN];
        ssize_t n = recv(clientfd, buf, sizeof(buf), 0);
        if (n == -1 && errno != EAGAIN) {
            throw std::runtime_error("read: " + std::string(strerror(errno)));
        }
        if (n == 0 || n == -1) {
            //hanle
        }
        clientRequest.append(buf, (unsigned long)n);

}

void CClient::Close() {
    close(clientfd);
}
