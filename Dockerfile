FROM fedora

MAINTAINER Parfenkov Vlad

RUN yum install -y gcc-c++
RUN yum install -y cmake 3.6
RUN yum install -y make

ADD . /app
WORKDIR /app

RUN cmake CMakeLists.txt
RUN make

CMD ./webserver 80 

EXPOSE 80
