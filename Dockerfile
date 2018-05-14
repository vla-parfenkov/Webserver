FROM ubuntu:16.04
MAINTAINER Parfenkov Vladislav
RUN apt-get update && apt-get install -y --no-install-recommends apt-utils
RUN apt-get -y install g++
RUN apt-get -y install cmake 
RUN apt-get -y install make

RUN ln -s /usr/include/x86_64-linux-gnu/zconf.h /usr/include

ADD . /app
WORKDIR /app

RUN cmake CMakeLists.txt
RUN make

CMD ./webserver 

EXPOSE 80
