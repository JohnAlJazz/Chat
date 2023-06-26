TARGET= ../SenderMain.out
CC= gcc
CFLAGS= -ansi -g -pedantic
OBJS= SenderMain.o

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LDFLAGS)

SenderMain.o: SenderMain.c
	$(CC) $(CFLAGS) -c SenderMain.c

clean:
	rm -f $(TARGET) $(OBJS)
