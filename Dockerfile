FROM ubuntu:16.04
MAINTAINER Parfenkov Vladislav
RUN apt-get update && apt-get install -y --no-install-recommends apt-utils
RUN apt-get -y install g++



ADD /webserver/ ./

RUN g++ src/*.cpp -std=c++14

EXPOSE 80
