#makefile to run all app with makefiles
SERVER_TARGET= $(MAKE) -C ./Server -f makefile
SERVER_CLEAN= $(MAKE) -C ./Server -f makefile clean
CLIENT_TARGET= $(MAKE) -C ./Client -f makefile
CLIENT_CLEAN= $(MAKE) -C ./Client -f makefile clean
LISTENER_TARGET= $(MAKE) -C ./Parameters/Listener -f makefile
LISTENER_CLEAN= $(MAKE) -C ./Parameters/Listener -f makefile clean
SENDER_TARGET= $(MAKE) -C ./Parameters/Sender -f makefile
SENDER_CLEAN= $(MAKE) -C ./Parameters/Sender -f makefile clean
SHARED_LIBRARY_TARGET= $(MAKE) -C ../GenericStructs/Library -f makefile
SHARED_LIBRARY_TARGET_CLEAN= $(MAKE) -C ../GenericStructs/Library -f makefile clean
SERVER_RUN= $(MAKE) -C ./Server -f makefile check

check_all:
	$(SERVER_TARGET) && $(CLIENT_TARGET) && $(LISTENER_TARGET) && $(SENDER_TARGET)

clean_all:
	$(SERVER_CLEAN) && $(CLIENT_CLEAN) && $(LISTENER_CLEAN) && $(SENDER_CLEAN)

check_library:
	$(SHARED_LIBRARY_TARGET)

clean_library:
	$(SHARED_LIBRARY_TARGET_CLEAN)

run_server:
	$(SERVER_RUN)