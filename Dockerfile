FROM ubuntu:16.04
MAINTAINER Parfenkov Vladislav
RUN apt-get update && apt-get install -y --no-install-recommends apt-utils
RUN apt-get -y install g++
RUN apt-get -y install cmake 3.6
RUN apt-get -y install make

ADD . /app
WORKDIR /app

RUN cmake CMakeLists.txt
RUN make

CMD ./webserver 

EXPOSE 80
