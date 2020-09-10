FROM ubuntu:18.04 AS release
MAINTAINER Alex Sirmais

RUN  apt-get update
RUN  yes | apt-get install libboost-all-dev
RUN  yes | apt-get install cmake

WORKDIR /app

COPY . .

COPY ./httptest ./var/www/html/httptest

RUN mkdir ./mybuild && cd ./mybuild
RUN cmake -DCMAKE_BUILD_TYPE=Release ./
RUN make




CMD echo "\n\n" && ./server