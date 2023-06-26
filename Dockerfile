FROM ubuntu:latest

RUN apt-get update && apt-get install -y gcc

WORKDIR /app

COPY chatProject /app/chatProject
COPY project /app/project
COPY ../GenericStructs /app/GenericStructs

RUN gcc -o myprogram myprogram.c

CMD ["./myprogram"]
