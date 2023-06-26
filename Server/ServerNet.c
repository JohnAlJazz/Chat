#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h> /* printf() */
#include <fcntl.h> /* fcntl() */
#include <stdlib.h> /* malloc() */
#include <sys/select.h> /* fd_set, select() */
#include <signal.h> /* signal */
#include <sys/time.h> /* timeval */

#include "ServerNet.h"

#include "../DsLibrary/GenDLL.h"
#include "../DsLibrary/ListItr.h"
#include "../DsLibrary/GenEngineDLL.h"

#define MAX_NUM_OF_CLIENT 1000
#define INIT_FAIL 0
#define INIT_SUCCESS 1
#define NEW_CLIENT_FAIL 0
#define NEW_CLIENT_SUCCESS 1
#define SELECT_MAX 1024
#define NOT_EQUAL 0

struct Server
{
	List* m_list;
	NewClientFunc m_NewClient;
	GotMsgFunc m_GotMsg;
	CloseClientFunc m_CloseClient;
	FailFunc m_Fail;
	void* m_context;
	int* m_sock;
	int m_port;		
};

typedef struct ToClientsMsgFunc
{
	fd_set* m_temp;
	int m_activity;
	Server* m_server;

} ToClientsMsgFunc;

struct Koko {
	Server* m_server;
};

int g_flagToStopRun = 0;
int g_pipeFds[2];

static List* InitServer(int _port, int* _sock);
static int Init(int _port, int* _sock);
static int AcceptNewClients(Server* _svr, fd_set* _master);
static int GetClientsMessages(void* _node, void* _checkNode);
static void ElementDestroy(void* _node);
static void HandleExistingClients(Server* _server, fd_set* _master, fd_set* _tempFdSet, int _activity);
static void HandleSignal(int _signal);
static void HandleCleanUp();
static void HandleShutDown();

Server* CreateServer(NewClientFunc _NewClient, GotMsgFunc _GotMsg, CloseClientFunc _CloseClient, FailFunc _Fail, int _port,void* _context)
{
	Server* ptrServer;
	int* sock;
	if(!_GotMsg /* || _port <= 0 || _port > 1023*/)
	{
		return NULL;
	}
	ptrServer = (Server*)malloc(sizeof(Server));
	if(ptrServer == NULL)
	{
		return NULL;
	}
	sock = (int*)malloc(sizeof(int));
	if(sock == NULL)
	{
		return NULL;
	}
	ptrServer -> m_port = _port;
	ptrServer -> m_list = InitServer(ptrServer->m_port, sock);
	ptrServer -> m_NewClient = _NewClient;
	ptrServer -> m_GotMsg = _GotMsg;
	ptrServer -> m_CloseClient = _CloseClient;
	ptrServer -> m_Fail = _Fail;
	ptrServer -> m_context = _context;
	ptrServer -> m_sock = sock;		

	return ptrServer;
}

void RunServer(Server* _svr)
{		
	fd_set master, temp;
	int activity, isFail;			

	signal(SIGINT, HandleSignal);
	
	if(!_svr)
	{
		return;
	}

	FD_ZERO(&master);
	FD_SET(*(_svr->m_sock), &master);
	
	while(!g_flagToStopRun)
	{		
		temp = master;
		
		activity = select(SELECT_MAX, &temp, NULL, NULL, NULL);						
        
		if(activity < 0 && errno != EINTR)
		{
			perror("\nselect error");			
		}
		
		/* accept also blocks, so when a signal arrives,
		server sock is set, thus going to accept. that's why the flag check */
		if(FD_ISSET(*(_svr->m_sock), &temp) && !g_flagToStopRun)
		{			
			--activity;
			isFail = AcceptNewClients(_svr, &master);
			if(isFail = NEW_CLIENT_FAIL)
			{				
				return;
			}
		}		
			
		if(activity > 0 && !g_flagToStopRun)
		{			
			HandleExistingClients(_svr, &master, &temp, activity);
		}		
	}
	HandleShutDown();	
}

void SendMsg(Server* _svr, int _clientId, char* _buffer)
{
	ssize_t sendBytes;
	
	if(!_svr || _clientId < 1 || !_buffer) 
	{
		return;
	}	
	sendBytes = send(_clientId, _buffer, strlen(_buffer), 0);
	
	if(sendBytes < 0)
	{
		perror("send failed");
		close(_clientId);
		if(_svr -> m_Fail != NULL)
		{
			_svr -> m_Fail(_svr -> m_context, "send failed",  NULL);
		}
		return;
	}
}

void DestroyServer(Server* _svr)
{	
	if(_svr)
	{
		ListDestroy(&(_svr -> m_list), ElementDestroy);		
		close(*(_svr -> m_sock));
		free(_svr -> m_sock);
	}
	HandleCleanUp();	
}

void* GetContext(Server* _svr)
{
	return _svr -> m_context;
}

/************************************ static *****************************/

static void HandleSignal(int _signal)
{	
	if(_signal == SIGINT)
	{					
		g_flagToStopRun = 1; 		 
	}
}

static void HandleShutDown()
{
	system("clear");
	printf("Server is shutting down...\n");
	sleep(3);
}

static void HandleCleanUp()
{
	system("clear");
	printf("Server cleaned up successfuly...\n\n");
	sleep(3);
	system("clear");
}

static void HandleExistingClients(Server* _server, fd_set* _master, fd_set* _tempFdSet, int _activity)
{
	ToClientsMsgFunc* checkEachNode;
	ListItr begin, end, item, data;

	checkEachNode = (ToClientsMsgFunc*)malloc(sizeof(ToClientsMsgFunc));
	if(!checkEachNode)
	{
		return;
	}

	checkEachNode->m_temp = _tempFdSet;
	checkEachNode->m_activity = _activity;
	checkEachNode->m_server = _server;

	begin = ListItrBegin(_server->m_list);
	end = ListItrEnd(_server->m_list);
	item = begin;

	while(ListItrEquals(item, end) == NOT_EQUAL)
	{
		item = ListItrForEach(begin, end, GetClientsMessages, (void*)checkEachNode);
		begin = ListItrNext(item);

		/* GetClientsMessages broke iteration*/
		if(ListItrEquals(item, end) == NOT_EQUAL)
		{
			if(checkEachNode->m_activity != 0)
			{
				data = ListItrRemove(item);
				FD_CLR(*(int*)data, _master);
				printf("sock %d was removed\n", *(int*)data);
				--checkEachNode->m_activity;

				if(_server->m_CloseClient)
				{
					_server->m_CloseClient((*(int*)data), (_server->m_context), NULL);
				}
			}
			if(checkEachNode->m_activity == 0)
			{				
				break;
			}
		}
	} 
	free(checkEachNode);
}

static List* InitServer(int _port, int* _sock)
{
	List* ptrList;
	int isInit;
	
	if(_sock == NULL)
	{
		return NULL;
	}
	ptrList = ListCreate();
	if(ptrList == NULL)
	{
		return NULL;
	}
	isInit = Init(_port, _sock);
	if(isInit == INIT_FAIL)
	{
		ListDestroy(&ptrList, NULL);
		return NULL;
	}
	return ptrList;
}

static int Init(int _port, int* _sock)
{
	struct sockaddr_in sinS;
	int ibind, isetsockopt, ilisten, optval = 1;
	
	*_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(*_sock < 0)
	{
		perror("socket failed");
		return INIT_FAIL;
	}
	
	isetsockopt = setsockopt(*_sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
	if(isetsockopt < 0)
	{
		perror("reuse failed");
		close(*_sock);
		return INIT_FAIL;
	}
	
	memset(&sinS, 0, sizeof(sinS));
	sinS.sin_family = AF_INET;
	sinS.sin_addr.s_addr = INADDR_ANY;
	sinS.sin_port = htons(_port);
	
	ibind = bind(*_sock, (struct sockaddr*) &sinS, sizeof(sinS));
	if(ibind < 0)
	{
		perror("bind failed");
		close(*_sock);
		return INIT_FAIL;
	}
	
	ilisten = listen(*_sock, MAX_NUM_OF_CLIENT);
	if(ilisten < 0)
	{
		perror("listen failed");
		close(*_sock);
		return INIT_FAIL;
	}
    printf("Server is listening on port %d\n", _port);
	return INIT_SUCCESS;
}

static int AcceptNewClients(Server* _svr, fd_set* _master)
{
	int accept_sock, *client_sock, status;
	struct sockaddr_in sinC;
	socklen_t sinLength = sizeof(sinC);	
	
	accept_sock = accept(*(_svr -> m_sock), (struct sockaddr*) &sinC, &sinLength);
	
	if(accept_sock < 0)
	{
		perror("accept failed");
		return NEW_CLIENT_FAIL;
	}
	FD_SET(accept_sock, _master);
	printf("sock %d was created\n", accept_sock);
	client_sock = (int*)malloc(sizeof(int));
	if(client_sock == NULL)
	{
		perror("malloc failed\n");
		FD_CLR(accept_sock, _master);
		return NEW_CLIENT_FAIL;
	}
	*client_sock = accept_sock;
	status = ListPushHead(_svr->m_list, (void*)client_sock);
	if(status != LIST_SUCCESS)
	{
		printf("Insertion failed\n"); 
		FD_CLR(accept_sock, _master);
		free(client_sock);
		return NEW_CLIENT_FAIL;
	}
	if(_svr -> m_NewClient)
	{
		_svr -> m_NewClient(accept_sock, _svr -> m_context, NULL);
	}
	return NEW_CLIENT_SUCCESS;
}

static int GetClientsMessages(void* _node, void* _checkNode)
{
	ServerNetErr status;
	void* data;
	int clientFd;
	ssize_t readBytes;
	char buffer[4096];
	Server* server = ((ToClientsMsgFunc*)_checkNode)->m_server;	
	ToClientsMsgFunc* checkNode = (ToClientsMsgFunc*)_checkNode;

	data = ListItrGet(_node);
	clientFd = *(int*)data;

	/*in generic engine implementation,
	 break iteration if func returns 1, otherwise - 0*/	

	if(FD_ISSET(clientFd, checkNode->m_temp))
	{
		/* generic dll engine runs this func on each node, so vars are not deleted until end of iteration */
		memset(buffer, 0, sizeof(buffer));
		readBytes = recv(clientFd, buffer, sizeof(buffer), 0);
		if(readBytes < 0)
		{
			perror("recv failed");
			close(clientFd);
			return 1;
		}
		else if(readBytes == 0)
		{
			printf("client in socket %d terminated connection\n", clientFd);
			close(clientFd);
			return 1;
		}
		else
		{
			printf("msg from client in sock %d: %s\n", clientFd, buffer);
			
			status = server->m_GotMsg(clientFd, buffer, server->m_context, sizeof(buffer), server);
			if(status == CLOSE_AND_REMOVE_SOCK)
			{
				close(clientFd);
				return 1;
			}
		}
		--checkNode->m_activity;
		if(checkNode->m_activity == 0)
		{			
			return 1;
		}
	}
	return 0;
}

static void ElementDestroy(void* _node)
{
	void* data;
	int sock;
	data = ListItrGet(_node);
	sock = *(int*)data;
	close(sock);
	free(data);
}