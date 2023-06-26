#ifndef __CLIENT_NET_H__
#define __CLIENT_NET_H__

#define BUFFER 1500

typedef struct Client Client;

typedef enum{
	INIT_FAIL,
	SUCKET_FAILED,
	INIT_SUCCESS,
	CONNECTION_FAILED,
	CONNECTION_SUCCEDED,
	RUN_CLIENT_FAILED,
	SEND_FAILED,
	SEND_SUCCEEDED,
	RECEIVE_FAILED,
	CONNECTION_IS_CLOSED,
	RECEIVE_SUCCEEDED
} CNetErr;

typedef void(*GotMsgFunc)(int _clientID, char* _buffer, void* _context, int  _msgSize, void* _serverNetContext);
typedef void(*FailFunc)(void* _context, CNetErr _error, void* _serverNetContext);

/*------------------------------------------------------------*/

/*
Description: creates comunucation with ServerNet, opens the port to communicate
Input: GotMsgFunc - function to send to app the answer that received fron server, cannot be NULL
		FailFunc - if need to have the error message in case of failure
		_port, context.
Output: pointer to struct that holds the information to use
Errors: NULL
*/
Client* CreateClient(FailFunc _Fail, int _port, void* _context);

void DestroyClient(Client* _clt);

/*
Description: send a message to ServerNet
Input: pointer to struct for comunication, message to send
Output: status
Errors:according to CNetErr errors
*/
CNetErr MsgSend(Client* _clt, char* _msgToSend);

/*
Description: recieve a message from ServerNet
Input: pointer to struct for comunication, array to recieve a message
Output: status
Errors:according to CNetErr errors
*/
CNetErr MsgRecv(Client* _clt, char _msgRecv[BUFFER]);

#endif /* __CLIENT_NET_H__ */
