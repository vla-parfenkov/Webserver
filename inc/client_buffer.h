//
// Created by parfenkov on 18.03.18.
//

#ifndef WEBSERVER_CCLIENTSBUFFER_H
#define WEBSERVER_CCLIENTSBUFFER_H

#include <map>
#include <mutex>

enum ClientStatus{
    WANT_READ, WANT_WRITE, WANT_CLOSE
};

class CClientsBuffer {
 private:
    std::map<int, std::string> clientBufferMap;
    std::map<int, ClientStatus> clientStatusMap;
    std::mutex bufferMutex;



public:
    void AddClient(int fd);
    void DelClient(int fd);
    void AddData(int fd, const char* data, size_t size);
    bool isWantToRead(int fd);
    bool isWantToWrite(int fd);
    bool isWantToClose(int fd);
    void SetWantToRead(int fd);
    void SetWantToWrite(int fd);
    void SetWantToClose(int fd);
    std::string GetData(int fd);

    CClientsBuffer();
    ~CClientsBuffer();

};


#endif //WEBSERVER_CCLIENTSBUFFER_H
