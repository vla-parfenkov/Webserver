//
// Created by parfenkov on 04.03.18.
//

#ifndef WEBSERVER_HTTP_REQUEST_H
#define WEBSERVER_HTTP_REQUEST_H

#include <iostream>
#include <unordered_map>
#include <functional>
#include "threadpool.h"

#define BUFFER_SIZE 4096


class CClient;

class CHTTPHandler {
private:
    std::string rootDir;


    std::unordered_map <std::string, std::string> mime_map =
            {
                    { "txt", "text/plain" },
                    { "html", "text/html" },
                    { "css", "text/css" },
                    { "js", "application/javascript" },
                    { "jpg", "image/jpeg" },
                    { "jpeg", "image/jpeg" },
                    { "png", "image/png" },
                    { "gif", "image/gif" },
                    { "swf", "application/x-shockwave-flash" }
            };

    const char* notFound =
            "<html> "
                    "<head><title>Not Found</title></head> "
                    "<body><h1>404 Not Found</h1></body> "
                    "</html>";
    const char* forbidden =
            "<html>"
                    "<head><title>Forbidden</title></head>"
                    "<body><h1>403 Forbidden</h1></body>"
                    "</html>";
    const char* notAllowed =
            "<html>"
                    "<head><title>Not Allowed</title></head>"
                    "<body><h1>404 Method Not Allowed</h1></body>"
                    "</html>";
    const char* notImplemented =
            "<html>"
                    "<head><title>Not Implemented</title></head>"
                    "<body><h1>501 Not Implemented</h1></body>"
                    "</html>";

    //void addBuffer(char *newBuff);
    //void saveStr(std::string& str, char* begin, char* end);
    bool readReq(std::string& str, std::istringstream& stream);
    std::string urldecode(const std::string& url);
    bool fileExists(const std::string& path);
    std::string getCode(int code);
    std::string getFileType(const std::string& path);
    std::string buidHeader(size_t fileLength, const std::string& fileType,
                           const std::string &protocol,  const std::string &code);
    void GET(const std::string& url, const std::string& protocol, std::string& header, std::string& path,
             std::string& responce);
    void HEAD(const std::string& url, const std::string& protocol, std::string& header, std::string& responce);
    void NotAllowed(const std::string& protocol, std::string& header, std::string& responce);
    void NotImplemented(const std::string& protocol, std::string& header, std::string& responce);



public:
    void RequestHandle(std::string request, std::string& header, std::string& path, std::string& responce);
    CHTTPHandler(const std::string& root);
    //void RequestAdd(std::string request, CClientData* clientData);

    CHTTPHandler();
    ~CHTTPHandler(){};


};


#endif //WEBSERVER_HTTP_REQUEST_H
