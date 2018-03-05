//
// Created by parfenkov on 04.03.18.
//

#include "client.h"
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>


CClient::CClient(const std::string& root, CThreadPool* threadPoll ) : threadPool(threadPoll)   {
    httpRequest = new CHTTPRequest(root);
}

CClient::~CClient() {
    delete httpRequest;
}



void CClient::Write(int fd, const std::string &message) {
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

void CClient::Read(ClientBuf* clientBuf) {
    char buf[MAX_REQUEST_LEN];
        ssize_t n = recv(clientBuf->fd, buf, sizeof(buf), 0);
        if (n == -1 && errno != EAGAIN) {
            throw std::runtime_error("read: " + std::string(strerror(errno)));
        }
        if (n == 0 || n == -1) {
            clientBuf->data.append(buf, (unsigned long)n);
            try {
                std::string res = httpRequest->RequestHandle(clientBuf->data);
                clientBuf->status = WANT_WRITE;
                std::function<void()> onWrite;
                onWrite = std::bind(&CClient::Write, &*this, clientBuf->fd, res);

                threadPool->AddTask(onWrite);
            } catch (std::runtime_error) {
                return;
            }
        }
        clientBuf->data.append(buf, (unsigned long)n);
    try {
        std::string res = httpRequest->RequestHandle(clientBuf->data);
        clientBuf->status = WANT_WRITE;
        std::function<void()> onWrite;
        onWrite = std::bind(&CClient::Write, &*this, clientBuf->fd, res);

        threadPool->AddTask(onWrite);
    } catch (std::runtime_error) {
        return;
    }
}

void CClient::Close(int fd) {
    close(fd);
}
