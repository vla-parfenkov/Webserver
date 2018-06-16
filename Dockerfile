FROM fedora

MAINTAINER Parfenkov Vladislav

RUN yum install -y gcc-c++
RUN yum install -y cmake
RUN yum install -y make

RUN ln -s /usr/include/x86_64-linux-gnu/zconf.h /usr/include

ADD . /app
WORKDIR /app

RUN cmake CMakeLists.txt
RUN make

CMD ./webserver 80 

EXPOSE 80
