#write the export line in the terminal if you run only make 
export LD_LIBRARY_PATH=../DsLibrary/
LDFLAGS=-L../DsLibrary/ -lGenericStructs
TARGET= ServerApp.out
CC= gcc
CFLAGS= -ansi -g -pedantic
OBJS= user.o userMng.o groups.o grpMng.o Protocol.o ServerNet.o ServerApp.o ServerAppMain.o

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LDFLAGS)

user.o: ../User/user.c ../User/user.h ../DsLibrary/GenDLL.h ../DsLibrary/ListItr.h ../DsLibrary/GenEngineDLL.h
	$(CC) $(CFLAGS) -c ../User/user.c
	
userMng.o: ../UserManager/userMng.c ../UserManager/userMng.h ../DsLibrary/HashMap.h ../DsLibrary/ListItr.h
	$(CC) $(CFLAGS) -c ../UserManager/userMng.c
	
groups.o: ../Group/groups.c ../Group/groups.h ../DsLibrary/GenDLL.h ../DsLibrary/ListItr.h ../DsLibrary/GenEngineDLL.h
	$(CC) $(CFLAGS) -c ../Group/groups.c
	
grpMng.o: ../GroupManager/grpMng.c ../GroupManager/grpMng.h ../Group/groups.h ../DsLibrary/HashMap.h ../DsLibrary/ListItr.h ../DsLibrary/GenEngineDLL.h ../DsLibrary/GenQ.h
	$(CC) $(CFLAGS) -c ../GroupManager/grpMng.c

Protocol.o: ../Protocol/Protocol.c ../Protocol/Protocol.h
	$(CC) $(CFLAGS) -c ../Protocol/Protocol.c
	
ServerNet.o: ServerNet.c ServerNet.h ../DsLibrary/GenDLL.h ../DsLibrary/ListItr.h ../DsLibrary/GenEngineDLL.h
	$(CC) $(CFLAGS) -c ServerNet.c
	
ServerApp.o: ServerApp.c ServerApp.h ServerNet.h ../Protocol/Protocol.h ../GroupManager/grpMng.h ../DsLibrary/GenDLL.h ../DsLibrary/ListItr.h
	$(CC) $(CFLAGS) -c ServerApp.c
	
ServerAppMain.o: ServerAppMain.c ServerApp.h
	$(CC) $(CFLAGS) -c ServerAppMain.c

check:
	make && clear && ./ServerApp.out

clean:
	rm -f $(TARGET) $(OBJS)
