cmake_minimum_required(VERSION 3.5)
project(webserver)

set(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_FLAGS -pthread)


include_directories(inc)

set(SOURCE_FILES src/main.cpp src/server.cpp src/threadpool.cpp src/epollengine.cpp src/http_handler.cpp src/http_session.cpp src/config.cpp)
add_executable(webserver ${SOURCE_FILES})
