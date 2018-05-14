//
// Created by parfenkov on 04.03.18.
//

#include <sstream>
#include <cstring>
#include <fstream>
#include <sys/stat.h>
#include <ctime>
#include <chrono>
#include "http_handler.h"


void CHTTPHandler::RequestHandle(std::string request, std::string& header, std::string& path, std::string& responce)  {

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
        GET(url, protocol, header, path, responce);
    } else if (method == "HEAD") {
        HEAD(url, protocol, header, responce);
    } else if (method == "POST" || method == "PUT" || method == "PATCH" ||
               method == "DELETE" || method == "TRACE" || method == "CONNECT" ||
               method == "OPTIONS") {
        NotAllowed(protocol, header, responce);
    } else {
        NotImplemented(protocol, header, responce);
    }


    /*std::function<void()> onClose;
    onClose = std::bind(&CClient::Close, &*client, fd);
    threadPool->AddTask(onClose);*/
}



CHTTPHandler::CHTTPHandler(const std::string &root) : rootDir(root) {}

std::string CHTTPHandler::urldecode(const std::string& url) {
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

bool CHTTPHandler::readReq(std::string &str, std::istringstream &stream) {

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

void CHTTPHandler::GET(const std::string &url, const std::string &protocol, std::string& header, std::string& path,
                       std::string& responce) {
    std::string pathTemp = rootDir + url;
    int code;
    struct stat s;
    bool dir = ((stat(pathTemp.c_str(), &s) == 0) && (s.st_mode & S_IFDIR));
    if (dir) {
        if (pathTemp.back() != '/') {
            pathTemp += '/';
        }
        pathTemp += "index.html";
    }


    if (fileExists(pathTemp)) {
        code = 200;
        std::string type = dir ? "html" : getFileType(url);
        struct stat file;
        stat(pathTemp.c_str(), &file);
        header = buidHeader((size_t )file.st_size, type, protocol, getCode(code));
        header.append("\r\n");
        path = pathTemp;
        std::ifstream in(path);

    } else {
        code = (dir) ? 403 : 404;
        header  = buidHeader(strlen((dir) ? forbidden : notFound), "html", protocol, getCode(code));
        header.append("\r\n");
        responce = (dir) ? forbidden : notFound;

    }



}

void CHTTPHandler::HEAD(const std::string &url, const std::string &protocol, std::string& header, std::string& responce) {
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
        header = buidHeader((size_t )file.st_size, type, protocol, getCode(code));
        header.append("\r\n");

    } else {
        code = (dir) ? 403 : 404;

       header = buidHeader(strlen((dir) ? forbidden : notFound), "html", protocol, getCode(code));
        header.append("\r\n");
    }

}

void CHTTPHandler::NotAllowed(const std::string &protocol, std::string& header, std::string& responce) {
    const int code = 405;
    header = buidHeader(strlen(notAllowed), "html", protocol, getCode(code));
    header.append("\r\n");
    responce = notAllowed;
}

void CHTTPHandler::NotImplemented(const std::string &protocol, std::string& header, std::string& responce) {
    const int code = 501;
    header = buidHeader(strlen(notImplemented), "html", protocol, getCode(code));
    header.append("\r\n");
    responce = notImplemented;

}

bool CHTTPHandler::fileExists(const std::string &path) {
    bool isExist = false;
    std::ifstream fin(path.c_str());

    if(fin.is_open())
        isExist = true;

    fin.close();
    return isExist;
}

std::string CHTTPHandler::getFileType(const std::string &path) {
    size_t pos = path.find_last_of('.');
    return (pos != std::string::npos) ? path.substr(pos + 1) : "txt";
}

std::string CHTTPHandler::getCode(int code) {
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


std::string CHTTPHandler::buidHeader(size_t fileLength, const std::string &fileType, const std::string &protocol,
                                     const std::string &code) {
    std::ostringstream response;
    std::chrono::system_clock::time_point p = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(p);

    response << protocol
             << ' ' << code << "\r\n"
             << "Server: WebServer \r\n"
             << "Date: " << std::ctime(&t)
             << "Connection: close\r\n"
             << "Content-Length: " << fileLength << "\r\n"
             << "Content-Type: " << mime_map[fileType] << "\r\n";

    return response.str();
}
