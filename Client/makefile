#makefile for clientApp - Chat application
TARGET= ClientApp.out
CC= gcc
CFLAGS= -ansi -g -pedantic
OBJS= ClientAppMain.o ClientApp.o ClientNet.o UIChat.o Chat.o Protocol.o

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS)

ClientAppMain.o: ClientAppMain.c ClientApp.h
	$(CC) $(CFLAGS) -c ClientAppMain.c

ClientApp.o: ClientApp.c ClientApp.h ClientNet.h ../Protocol/Protocol.h UIChat.h
	$(CC) $(CFLAGS) -c ClientApp.c

ClientNet.o: ClientNet.c ClientNet.h
	$(CC) $(CFLAGS) -c ClientNet.c

UIChat.o: UIChat.c UIChat.h
	$(CC) $(CFLAGS) -c UIChat.c
    
Chat.o: ../Chat/Chat.c ../Chat/Chat.h
	$(CC) $(CFLAGS) -c ../Chat/Chat.c

Protocol.o: ../Protocol/Protocol.c ../Protocol/Protocol.h
	$(CC) $(CFLAGS) -c ../Protocol/Protocol.c

check:
	make && ./ClientApp.out
    
clean:
	rm -f $(TARGET) $(OBJS) *.txt