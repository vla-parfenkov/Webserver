//
// Created by parfenkov on 04.03.18.
//

#include <sstream>
#include <cstring>
#include <fstream>
#include <sys/stat.h>
#include <zconf.h>
#include <ctime>
#include <chrono>
#include "http_request.h"

std::string CHTTPRequest::RequestHandle(std::string request)  {

    std::string method, url, protocol = "HTTP/1.1";
    std::istringstream istringstream(request);

    if(!readReq(method, istringstream)) {
        throw std::runtime_error("bad request");
    }

    if(!readReq(url, istringstream)) {
        throw std::runtime_error("bad request");
    }

    if(!readReq(protocol, istringstream)) {
        throw std::runtime_error("bad request");
    }


    url = urldecode(url);
    size_t  pos;

    while ((pos = url.find("/../")) != std::string::npos) {
        url.erase(pos, 4);
    }

    if ((pos = url.find('?')) != std::string::npos) {
        url = url.substr(0, pos);
    }

    if (method == "GET") {
        return GET(url, protocol);
    } else if (method == "HEAD") {
        return HEAD(url, protocol);
    } else if (method == "POST" || method == "PUT" || method == "PATCH" ||
               method == "DELETE" || method == "TRACE" || method == "CONNECT" ||
               method == "OPTIONS") {
        return NotAllowed(protocol);
    } else {
        return NotImplemented(protocol);
    }
}

/*void CHTTPRequest::RequestAdd() {
    addBuffer(request);

    char * oldPos = requestBuff;
    char * newPos = requestBuff;
    newPos = strchr(newPos, ' ');
    if(newPos == NULL) {
        isValid = false;
        return;
    }
    if (method.empty()) {
        saveStr(method, oldPos, newPos);
    }

    newPos++;
    oldPos = newPos;
    newPos = strchr(newPos, ' ');
    if(newPos == NULL) {
        isValid = false;
        return;
    }

    if (url.empty()) {
        saveStr(url, oldPos, newPos);
    }

    newPos++;
    oldPos = newPos;
    newPos = strchr(newPos, '\r');
    if(newPos == NULL) {
        isValid = false;
        return;
    }

    if(protocol.empty()) {
        saveStr(protocol, oldPos, newPos);
    }
    if (!url.empty()) {
        url = urldecode(url);
    }
    isValid = true;

}*/

/*void CHTTPRequest::addBuffer(char *newBuff) {
    size_t i = 0;
    while (newBuff != NULL) {
        requestBuff[bufferSize] = *(newBuff + i);
        i++;
        bufferSize++;
    }
}*/

/*void CHTTPRequest::saveStr(std::string &str, char *begin, char* end) {
    std::string res(begin, 0, end-begin);
    str = res;
}*/

CHTTPRequest::CHTTPRequest(const std::string &root) : rootDir(root) {}

std::string CHTTPRequest::urldecode(const std::string& url) {
    std::string res;
    char ch;
    for (size_t i=0; i<url.length(); i++) {
        if (url[i] == '%') {
            int ii;
            sscanf(url.substr(i + 1,2).c_str(), "%x", &ii);
            ch = static_cast<char>(ii);
            res += ch;
            i += 2;
        } else if (url[i] == '+') {
            res += ' ';
        } else {
            res += url[i];
        }
    }

    return res;
}

bool CHTTPRequest::readReq(std::string &str, std::istringstream &stream) {

    while (stream.peek() != EOF) {
        char c = (char)stream.get();
        if (c == ' ' || c == '\r' || c == '\n' ) {
            return true;
        } else {
            str.push_back(c);
        }
    }

    return false;
}

std::string CHTTPRequest::GET(const std::string &url, const std::string &protocol) {
    std::string path = rootDir + url;
    int code;
    struct stat s;
    bool dir = ((stat(path.c_str(), &s) == 0) && (s.st_mode & S_IFDIR));
    if (dir) {
        if (path.back() != '/') {
            path += '/';
        }
        path += "index.html";
    }


    if (fileExists(path)) {
        code = 200;
        std::string type = dir ? "html" : getFileType(url);
        struct stat file;
        stat(path.c_str(), &file);
        return buidHeader((size_t )file.st_size, type, protocol, getCode(code));
         //sent
    } else {
        code = (dir) ? 403 : 404;
        return buidHeader(strlen((dir) ? forbidden : notFound), "html", protocol, getCode(code));
        //sent
    }

}

std::string CHTTPRequest::HEAD(const std::string &url, const std::string &protocol) {
    std::string path = rootDir + url;
    int code;
    struct stat s;
    bool dir = ((stat(path.c_str(), &s) == 0) && (s.st_mode & S_IFDIR));
    if (dir) {
        if (path.back() != '/') {
            path += '/';
        }
        path += "index.html";
    }


    if (fileExists(path)) {
        code = 200;
        std::string type = dir ? "html" : getFileType(url);
        struct stat file;
        stat(path.c_str(), &file);
        return buidHeader((size_t )file.st_size, type, protocol, getCode(code));
        //sent
    } else {
        code = (dir) ? 403 : 404;
        return buidHeader(strlen((dir) ? forbidden : notFound), "html", protocol, getCode(code));
        //sent
    }

}

std::string CHTTPRequest::NotAllowed(const std::string &protocol) {
    const int code = 405;
    return buidHeader(strlen(notAllowed), "html", protocol, getCode(code));
}

std::string CHTTPRequest::NotImplemented(const std::string &protocol) {
    const int code = 501;
    return buidHeader(strlen(notImplemented), "html", protocol, getCode(code));
}

bool CHTTPRequest::fileExists(const std::string &path) {
    return access(path.c_str(), 0) != -1;
}

std::string CHTTPRequest::getFileType(const std::string &path) {
    size_t pos = path.find_last_of('.');
    return (pos != std::string::npos) ? path.substr(pos + 1) : "txt";
}

std::string CHTTPRequest::getCode(int code) {
    switch (code) {
        case 200:
            return "200 OK";
        case 403:
            return "403 Forbidden";
        case 404:
            return "404 Not Found";
        case 405:
            return "405 Method Not Allowed";
        case 501:
            return "501 Not Implemented";
        default:
            return "";
    }
}


std::string CHTTPRequest::buidHeader(size_t fileLength, const std::string &fileType, const std::string &protocol,
                                     const std::string &code) {
    std::ostringstream response;
    std::chrono::system_clock::time_point p = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(p);

    response << protocol
             << ' ' << code << "\r\n"
             << "Server: WebServer \r\n"
             << "Date: " << std::ctime(&t) << "\r\n"
             << "Connection: close\r\n"
             << "Content-Length: " << fileLength << "\r\n"
             << "Content-Type: " << mime_map[fileType] << "\r\n";

    return response.str();
}