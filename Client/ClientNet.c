#include <stdio.h>			/*printf*/
#include <stdlib.h>			/*malloc*/
#include <string.h>			/*memset*/
#include <sys/socket.h>		/*socket*/
#include <netinet/in.h>		/*socket*/

#include "ClientNet.h"

#define PORT 3000
#define BUFFER 1500

struct Client{
	GotMsgFunc m_GotMsg;
	FailFunc m_Fail;
	void *m_context;
	int m_port;
	int *m_sock;
	int flagForConnect;
};

static CNetErr InitClient(int* _sock);

/*---------------------------API---------------------------*/

Client* CreateClient(FailFunc _Fail, int _port, void* _context)
{
	Client* ptrClient;
	CNetErr status;
	int* sock;

	if((ptrClient = (Client*)malloc(sizeof(Client))) == NULL)
	{
		return NULL;
	}
	ptrClient->m_Fail = _Fail;
	ptrClient->m_context;
	if((sock = (int*)malloc(sizeof(int))) == NULL)
	{
		return NULL;
	}
	if((status = InitClient(sock)) != INIT_SUCCESS)
	{
		if(ptrClient->m_Fail != NULL){
			ptrClient->m_Fail(ptrClient->m_context, status, NULL);
		}
		return NULL;
	}

	ptrClient->m_sock = sock;
	ptrClient->m_port = _port;
	ptrClient->flagForConnect = 0;
	return ptrClient;
}

void DestroyClient(Client* _clt)
{
	if(_clt != NULL)
	{
		close(*(_clt->m_sock));
		free(_clt->m_sock);
		free(_clt);
	}
}

CNetErr MsgSend(Client* _clt, char* _msgToSend)
{
	ssize_t sentBytes;

	if(!_clt || !_msgToSend)
	{
		return SEND_FAILED;
	}
	
	sentBytes = send((*(_clt->m_sock)), _msgToSend, strlen(_msgToSend), 0);

	if(sentBytes < 0)
	{
		close(*(_clt->m_sock));

		return SEND_FAILED;
	}	

	return SEND_SUCCEEDED;
}

CNetErr MsgRecv(Client* _clt, char _msgRecv[BUFFER])
{
	ssize_t readBytes;

	if(!_clt)
	{
		return RECEIVE_FAILED;
	}

	readBytes = recv((*(_clt->m_sock)), _msgRecv, BUFFER, 0);

	if(readBytes < 0)
	{		
		close(*(_clt->m_sock));

		return RECEIVE_FAILED;
	}	
	if(!readBytes)
	{		
		close(*(_clt->m_sock));
		
		return CONNECTION_IS_CLOSED;
	}

	return RECEIVE_SUCCEEDED;
}

/*-------------------------static-------------------------*/

static CNetErr InitClient(int* _sock)
{	
	struct sockaddr_in sinC;
	int iConnect;
	if(_sock == NULL){
		return SUCKET_FAILED;
	}
	*_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(*_sock < 0)
	{
		return SUCKET_FAILED;
	}
	memset(&sinC, 0, sizeof(sinC));
	sinC.sin_family = AF_INET;
	sinC.sin_addr.s_addr = INADDR_ANY;
	sinC.sin_port = htons(PORT);
	iConnect = connect(*_sock, (struct sockaddr *) &sinC, sizeof(sinC));
	if(iConnect < 0)
	{
		return CONNECTION_FAILED;
	}
	return INIT_SUCCESS;
}