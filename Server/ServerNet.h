#ifndef __SERVER_NET_H__
#define __SERVER_NET_H__

#include <sys/select.h> /* fd_set, select() */

#include "../DsLibrary/GenDLL.h"

typedef enum ServerNetErr
{
    SERVER_NET_SUCCESS,
    GOT_MSG_SUCCESS,
    CLOSE_AND_REMOVE_SOCK
}ServerNetErr;

typedef struct Server Server;

typedef void(*NewClientFunc)(int _clientID, void* _context, void* _serverNetContext);
typedef ServerNetErr(*GotMsgFunc)(int _clientID, char* _buffer, void* _context, int  _msgSize, Server* _serverNetContext);
typedef void(*CloseClientFunc)(int _clientID, void* _context, void* _serverNetContext);
typedef void(*FailFunc)(void* _context, char* _error, void* _serverNetContext);

Server* CreateServer(NewClientFunc _NewClient, GotMsgFunc _GotMsg, CloseClientFunc _CloseClient, FailFunc _Fail, int _port,void* _context);

void DestroyServer(Server* _svr);

void RunServer(Server* _svr);

void SendMsg(Server* _svr, int _clientSock, char* _buffer);

void* GetContext(Server* _svr);


#endif /* __SERVER_NET_H__ */
