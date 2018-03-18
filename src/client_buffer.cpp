//
// Created by parfenkov on 18.03.18.
//

#include <cstring>
#include "client_buffer.h"


CClientsBuffer::CClientsBuffer() {

}

CClientsBuffer::~CClientsBuffer() {

}

void CClientsBuffer::AddClient(int fd) {
    std::unique_lock<std::mutex> lock(bufferMutex);
    std::string newClientBuf;
    clientBufferMap.insert(std::pair<int, std::string> (fd, newClientBuf));
    clientStatusMap.insert(std::pair<int, ClientStatus > (fd, WANT_READ));
}

void CClientsBuffer::DelClient(int fd) {
    std::unique_lock<std::mutex> lock(bufferMutex);
    clientBufferMap.erase(fd);
    clientStatusMap.erase(fd);
}

void CClientsBuffer::AddData(int fd, const char *data, size_t size) {
    std::unique_lock<std::mutex> lock(bufferMutex);
    try {
        clientBufferMap.at(fd).append(data, (unsigned long) size)  ;
    } catch ( std::out_of_range ex) {
        AddClient(fd);
    }
}

bool CClientsBuffer::isWantToRead(int fd) {
    try {
        return (clientStatusMap.at(fd) == WANT_READ);
    } catch ( std::out_of_range ex) {
        return false;
    }


}

bool CClientsBuffer::isWantToWrite(int fd) {
    try {
        return (clientStatusMap.at(fd) == WANT_WRITE);
    } catch ( std::out_of_range ex) {
        return false;
    }
}

bool CClientsBuffer::isWantToClose(int fd) {
    try {
        return (clientStatusMap.at(fd) == WANT_CLOSE);
    } catch ( std::out_of_range ex) {
        return false;
    }
}

void CClientsBuffer::SetWantToRead(int fd) {
    std::unique_lock<std::mutex> lock(bufferMutex);
    try {
        clientStatusMap.at(fd) = WANT_READ  ;
    } catch ( std::out_of_range ex) {
        throw std::runtime_error("setWantToRead: " + std::string(strerror(errno)));
    }
}

void CClientsBuffer::SetWantToWrite(int fd) {
    std::unique_lock<std::mutex> lock(bufferMutex);
    try {
        clientStatusMap.at(fd) = WANT_WRITE  ;
    } catch ( std::out_of_range ex) {
        throw std::runtime_error("setWantToWrite: " + std::string(strerror(errno)));
    }
}

void CClientsBuffer::SetWantToClose(int fd) {
    std::unique_lock<std::mutex> lock(bufferMutex);
    try {
        clientStatusMap.at(fd) = WANT_CLOSE  ;
    } catch ( std::out_of_range ex) {
        throw std::runtime_error("setWantToClose: " + std::string(strerror(errno)));
    }
}

std::string CClientsBuffer::GetData(int fd) {
    try {
        return clientBufferMap.at(fd) ;
    } catch ( std::out_of_range ex) {
        throw  ex;
    }
}
