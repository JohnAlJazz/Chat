FROM ubuntu:20.04

RUN apt update && apt install -y gcc make \
    && apt clean

WORKDIR /app

COPY . /app

CMD make check_all && make run_server
