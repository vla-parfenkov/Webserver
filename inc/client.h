//
// Created by parfenkov on 04.03.18.
//

#ifndef WEBSERVER_СLIENT_H
#define WEBSERVER_СLIENT_H

#define MAX_REQUEST_LEN 4096

#include <iostream>
#include <netinet/in.h>

class CClient {
private:
    int clientfd;

    std::string clientRequest;


public:
    CClient(int fd);
    ~CClient();

    void Write(const std::string& message);
    std::string Read();
    void Close();

};


#endif //WEBSERVER_СLIENT_H
