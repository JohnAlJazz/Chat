TARGET= ../ListenerMain.out
CC= gcc
CFLAGS= -ansi -g -pedantic
OBJS= ListenerMain.o

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LDFLAGS)

ListenerMain.o: ListenerMain.c
	$(CC) $(CFLAGS) -c ListenerMain.c

clean:
	rm -f $(TARGET) $(OBJS)
