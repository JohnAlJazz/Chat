#include <stdio.h> /* files */
#include <string.h> 
#include <stdlib.h> /* malloc() */

#include "ClientApp.h"
#include "ClientNet.h"
#include "../Protocol/Protocol.h"
#include "UIChat.h"
#include "../Chat/Chat.h"

#define MAGIC 483607104
#define PORT 3000
#define BUFFER 1500
#define MAX_VALUE_SIZE 50
#define DEBUG 0
#define IP_SIZE 15
#define MAX_NUM_OF_GROUPS 3
#define EXIT 3
#define PROCESS_LENGTH 10
#define PORT_LENGTH 5

typedef struct Group Group;

struct ClientApp {
	char m_userName[MAX_VALUE_SIZE];
	Client* m_client;
	Group** m_groups;
	int m_numOfGroupsActive;
	int m_magic;
	int m_isConnected;
	int m_isNameInitialized;	
};

struct Group {
	char m_IP[IP_SIZE];
	char m_groupName [MAX_VALUE_SIZE];
	char m_port[PORT_LENGTH];
	char m_processA[PROCESS_LENGTH];
	char m_processB[PROCESS_LENGTH];	
};

static FailFunc Fail(void* _context, CNetErr _error, void* _serverNetContext);
static Respond RunGroupScreen(ClientApp* _clt);
static Respond RunMainScreen(ClientApp* _clt);
static CNetErr SendRecv (Client* _cltNet, char* _msgToSend, char *_msgRecv);
static Respond ClientRegister(Client* _cltNet, char* _msgToSend);
static Respond ClientLogin(Client* _cltNet, char* _msgToSend);
static Respond ClientExit(Client* _cltNet, char* _msgToSend);
static Respond ClientJoinGroup(ClientApp* _clt, char* _msgToSend, char* _groupName);
static Respond ClientCreateGroup(ClientApp* _clt, char* _msgToSend, char* _groupName);
static Respond ClientLeaveGroup(ClientApp* _clt, char* _msgToSend, char* _groupName);
static Respond ClientLogOut(ClientApp* _clt, char* _msgToSend);
static int OpenGroup(ClientApp* _clt, char* _groupName, char* _ipAdress, char* _port);
static Respond RunDebug (Client* _cltNet, char* _msgToSend, char *_msgRecv);
static void StartChat(ClientApp* _clt, char* _groupName);
static void CloseChat(ClientApp* _clt, char* _groupName);

ClientApp* CreateClientApp()
{
	ClientApp* clientAppPtr;
	Group** groupPtr;
	
	if((clientAppPtr = (ClientApp*)malloc(sizeof(ClientApp))) == NULL)
	{
		return NULL;
	}
	clientAppPtr->m_client = CreateClient(NULL, PORT, NULL);
	if(clientAppPtr->m_client == NULL)
	{
		PrintMessage("Connection initialization failure, please try again");
		return NULL;
	}
	if((groupPtr = (Group**)malloc(sizeof(Group*) * MAX_NUM_OF_GROUPS)) == NULL)
	{
		DestroyClient(clientAppPtr->m_client);
		free(clientAppPtr);
		return NULL;
	}
	clientAppPtr->m_groups = groupPtr;
	clientAppPtr->m_numOfGroupsActive = 0;
	clientAppPtr->m_magic = MAGIC;
	clientAppPtr->m_isConnected = 0;
	clientAppPtr->m_isNameInitialized = 0;
	
	return clientAppPtr;
}

static void HandleExit()
{
	system("clear");
	PrintMessage("Bye bye...\n");
	sleep(2);
}

static void HandleLogout(ClientApp* _clt)
{
	_clt->m_isConnected = 0;	
	system("clear");
	PrintMessage("You have logged out");
	sleep(2);	
}

static void HandleAllreadyConnected()
{
	system("clear");
	PrintMessage("You are already connected");
	sleep(2);
}

Respond RunClientApp(ClientApp* _clt)
{
	Respond status;
 
	if(((_clt == NULL) || (_clt->m_client == NULL)) && (DEBUG == 0))
	{
		PrintMessage("Please connect first");
		return EXIT_EXECUTED;
	}
	while(1)
	{	
		status = RunMainScreen(_clt);

		if(status == LOGIN_SUCCESS || status == REGISTRATION_SUCCEEDED)
		{	
			_clt->m_isConnected = 1;
			_clt->m_isNameInitialized = 1;
								
			status = RunGroupScreen(_clt);

			while(status != LOG_OUT_SUCCEEDED)
			{
				status = RunGroupScreen(_clt);
			}
			HandleLogout(_clt);				
			continue;			
		}
		if(status == EXIT_EXECUTED)
		{	
			HandleExit();		
			return status;
		}
		if(status == ALLREADY_CONNECTED)
		{
			HandleAllreadyConnected();	
			continue;
		}
	}
}

void DestroyClientApp(ClientApp* _clt)
{
	int i;	
	if((_clt == NULL) || (_clt->m_magic != MAGIC))
	{
		return;
	}
	for(i = 0; i < _clt->m_numOfGroupsActive; ++i)
	{
		free(_clt->m_groups[i]);
	}
	free(_clt->m_groups);
	DestroyClient(_clt->m_client);
	_clt->m_magic = 0;
	free(_clt);
}

/*----------------------static----------------------*/

static Respond RunMainScreen(ClientApp* _clt)
{
	int tag, msgSize;
	char name[MAX_VALUE_SIZE], password[MAX_VALUE_SIZE], msgToSend[BUFFER];
		
	PrintMainScreen(&tag, name, password);

	if(!_clt->m_isNameInitialized && tag != EXIT)
	{
		strncpy(_clt->m_userName, name, MAX_VALUE_SIZE);
	}
	else if(!_clt->m_isNameInitialized && tag == EXIT)
	{
		strncpy(_clt->m_userName, "NAME", MAX_VALUE_SIZE);
	}

	if(_clt->m_isConnected && tag != EXIT) 
	{
		return ALLREADY_CONNECTED;
	}

	switch(tag) 
	{
		case 1:			
			msgSize = PackReg(name, password, msgToSend);
			return ClientRegister(_clt->m_client, msgToSend);			

		case 2:			
			msgSize = PackLogin(name, password, msgToSend);
			return ClientLogin(_clt->m_client, msgToSend);			
			
		case 3:			
			msgSize = PackExit(_clt->m_userName, msgToSend);
			return ClientExit(_clt->m_client, msgToSend);

		default:
			break;
	}		
}

static Respond RunGroupScreen(ClientApp* _clt)
{
	int tag, msgSize;
	char name[MAX_VALUE_SIZE], group[MAX_VALUE_SIZE], msgToSend[BUFFER];
	Respond status;
	
	PrintGroupScreen(&tag, group);
	strncpy(name, _clt->m_userName, MAX_VALUE_SIZE);

	switch (tag)
	{
		case 1:
			msgSize = PackJoinGroup(name, group, msgToSend);
			if((status = ClientJoinGroup(_clt, msgToSend, group)) == JOIN_GROUP_SUCCEEDED)
			{
				StartChat(_clt, group);
			}			
			return status;

		case 2:
			msgSize = PackCreateGroup(name, group, msgToSend);
			if((status = ClientCreateGroup(_clt, msgToSend, group)) == GROUP_CREATED)
			{
				StartChat(_clt, group);
			}
			if(status == GROUPS_FULL_CAPACITY)
			{
				PrintMessage("Youv'e reached full groups capacity");
				EnterToContinue();
			}
			return status;

		case 3:
			msgSize = PackLeaveGroup(name, group, msgToSend);
			if((status = ClientLeaveGroup(_clt, msgToSend, group)) == LEAVING_GROUP_SUCCEEDED)
			{
				CloseChat(_clt, group);
			}			
			return status;

		case 4:
			msgSize = PackLogOut(_clt->m_userName, msgToSend);
			if((status = ClientLogOut(_clt, msgToSend)) == LOG_OUT_SUCCEEDED)
			{				
				return status;
			}

		default:
			break;
	}	
}

static void CloseChat(ClientApp* _clt, char* _groupName)
{
	int i;
	for(i = 0; i < _clt->m_numOfGroupsActive; ++i)
	{
		if((strcmp(_groupName, _clt->m_groups[i]->m_groupName)) == 0)
		{
			break;
		}
	}
	
	CloseWindows(_clt->m_groups[i]->m_processA, _clt->m_groups[i]->m_processB);	
	free(_clt->m_groups[i]);

	/* shift Group* to the left */
	while(i < _clt->m_numOfGroupsActive - 1)
	{
		_clt->m_groups[i] = _clt->m_groups[i + 1];		
		++i;
	}
	_clt->m_numOfGroupsActive -= 1;	
}

static void StartChat(ClientApp* _clt, char* _groupName)
{
	FILE *listenerFile, *senderFile;
	char listener[MAX_VALUE_SIZE] = "pIdListener.txt";
	char sender[MAX_VALUE_SIZE] = "pIdSender.txt";

	int groupsIdx = _clt->m_numOfGroupsActive - 1;
	
	OpenWindows(_clt->m_groups[groupsIdx]->m_IP, _clt->m_groups[groupsIdx]->m_port, _clt->m_userName, _groupName);

	listenerFile = fopen(listener, "r");
	senderFile = fopen(sender, "r");

	if(!listenerFile || !senderFile)
	{
		printf("couldn't open listener or sender\n");			
		sleep(5);	
		return;
	}	
	
	fgets(_clt->m_groups[groupsIdx]->m_processA, IP_SIZE, listenerFile);
	fgets(_clt->m_groups[groupsIdx]->m_processB, IP_SIZE, senderFile);	
	fclose(listenerFile);
	fclose(senderFile);
}

static CNetErr SendRecv(Client* _cltNet, char* _msgToSend, char *_msgRecv)
{
	CNetErr status;
	if(DEBUG)
	{
		char recvDebug[50];
		RunDebug(_cltNet, _msgToSend, recvDebug);
		strncpy(_msgRecv, recvDebug, 3);
		return RECEIVE_SUCCEEDED;
	}
	if((status = MsgSend(_cltNet, _msgToSend)) != SEND_SUCCEEDED)
	{
		PrintMessage("Sending your request failed, please try again");
		EnterToContinue();
		return status;
	}
	if((status = MsgRecv(_cltNet, _msgRecv)) != RECEIVE_SUCCEEDED)
	{		
		PrintMessage("couldn't receive respond, please try again");
		EnterToContinue();
	}	
	return status;
}

static Respond ClientRegister(Client* _cltNet, char* _msgToSend)
{
	CNetErr status;
	Respond respond;

	char msgRecv[BUFFER];
	
	if((status = SendRecv(_cltNet, _msgToSend, msgRecv)) != RECEIVE_SUCCEEDED)
	{ 
		return status;
	}
	respond = unpackMainRespond(msgRecv);
	if(respond == USERNAME_ALREADY_EXISTS)
	{
		PrintMessage("Username already exists");
		EnterToContinue();
		return respond;
	}
	if(respond == REGISTRATION_SUCCEEDED)
	{		
		PrintMessage("Registration succeeded");
		EnterToContinue();
		return respond;
	}	
	return BAD_RESPOND;
}

static Respond ClientLogin(Client* _cltNet, char* _msgToSend)
{
	CNetErr status;
	Respond respond;
	char msgRecv[BUFFER];

	if((status = SendRecv(_cltNet, _msgToSend, msgRecv)) != RECEIVE_SUCCEEDED)
	{
		return status;
	}
	respond = unpackMainRespond(msgRecv);
	if(respond == LOGIN_FAILED)
	{
		PrintMessage("One of the details you entered is wrong");
		EnterToContinue();
		return respond;
	}
	if(respond == LOGIN_SUCCESS)
	{
		PrintMessage("Login succeeded");
		EnterToContinue();
		return respond;
	}
	return BAD_RESPOND;
}

static Respond ClientExit(Client* _cltNet, char* _msgToSend)
{	
	CNetErr status = MsgSend(_cltNet, _msgToSend);
	if(status == SEND_FAILED)
	{
		PrintMessage("Couldn't send exit message to server");
		EnterToContinue();
	}
	return EXIT_EXECUTED;
}

static int OpenGroup(ClientApp* _clt, char* _groupName, char* _IP, char* _port)
{
	Group* groupPtr;

	if(_clt->m_numOfGroupsActive == MAX_NUM_OF_GROUPS)
	{		
		return GROUPS_FULL_CAPACITY;
	}
	
	if((groupPtr = (Group*)malloc(sizeof(Group))) == NULL)
	{
		return -1;
	}

	strncpy(groupPtr->m_IP, _IP, IP_SIZE);
	strncpy(groupPtr->m_port, _port, IP_SIZE);
	strncpy(groupPtr->m_groupName, _groupName, MAX_VALUE_SIZE);
	
	_clt->m_groups[_clt->m_numOfGroupsActive] = groupPtr;
	_clt->m_numOfGroupsActive += 1;	
	
	return 1;
}

static Respond ClientJoinGroup(ClientApp* _clt, char* _msgToSend, char* _groupName)
{
	CNetErr status;
	Respond respond;
	char port[IP_SIZE];
	char msgRecv[BUFFER], ipAdress[MAX_VALUE_SIZE];	
	
	if ((status = SendRecv(_clt->m_client, _msgToSend, msgRecv)) != RECEIVE_SUCCEEDED)
	{
		return status;
	}
	respond = UnpackJoinGroupRespond(msgRecv, ipAdress, port);
	if(respond == JOIN_GROUP_FAILED)
	{
		PrintMessage("Couldn't join to group");
		EnterToContinue();
		return respond;
	}
	if(respond == JOIN_GROUP_SUCCEEDED)
	{
		OpenGroup(_clt, _groupName, ipAdress, port);
		PrintMessage("Join group succeeded");
		EnterToContinue();
		return respond;
	}
	return BAD_RESPOND;
}

static Respond ClientCreateGroup(ClientApp* _clt, char* _msgToSend, char* _groupName)
{
	CNetErr status;
	Respond respond;	
	char port[IP_SIZE];
	char msgRecv[BUFFER], IP[MAX_VALUE_SIZE];

	if(_clt->m_numOfGroupsActive == MAX_NUM_OF_GROUPS)
	{
		return GROUPS_FULL_CAPACITY;
	}

	if((status = SendRecv(_clt->m_client, _msgToSend, msgRecv)) != RECEIVE_SUCCEEDED)
	{
		return status;
	}
	respond = UnpackCreateGroupRespond(msgRecv, IP, port);
	if(respond == CREATE_GROUP_FAILED)
	{
		PrintMessage("create group failed");
		EnterToContinue();
		return respond;
	}
	if(respond == GROUP_CREATED)
	{		
		OpenGroup(_clt, _groupName, IP, port);
		PrintMessage("create group succeeded");
		EnterToContinue();

		return respond;
	}
	return BAD_RESPOND;
}

static Respond ClientLeaveGroup(ClientApp* _clt, char* _msgToSend, char* _groupName)
{
	CNetErr status;
	Respond respond;
	char msgRecv[BUFFER];

	if ((status = SendRecv(_clt->m_client, _msgToSend, msgRecv)) != RECEIVE_SUCCEEDED)
	{
		return status;
	}
	respond = unpackGroupRespond(msgRecv);
	if(respond == LEAVING_GROUP_FAILED)
	{
		PrintMessage("leaving group failed");
		EnterToContinue();
		return respond;
	}
	if(respond == LEAVING_GROUP_SUCCEEDED)
	{
		PrintMessage("leaving group succeeded");
		EnterToContinue();
		return respond;
	}
	return BAD_RESPOND;
}

static Respond ClientLogOut(ClientApp* _clt, char* _msgToSend)
{
	int i;
	CNetErr status;
	Respond respond;
	char msgRecv[BUFFER];

	if((status = SendRecv(_clt->m_client, _msgToSend, msgRecv)) != RECEIVE_SUCCEEDED)
	{
		return status;
	}
	respond = unpackGroupRespond(msgRecv);
	if(respond == LOG_OUT_FAILED)
	{
		PrintMessage("Logging out failed, please try again");
		EnterToContinue();
		return respond;
	}
	if(respond == LOG_OUT_SUCCEEDED)
	{					
		for(i = 0; i < _clt->m_numOfGroupsActive; ++i)
		{			
			CloseWindows(_clt->m_groups[i]->m_processA, _clt->m_groups[i]->m_processB);
			free(_clt->m_groups[i]);
		}
		_clt->m_numOfGroupsActive = 0;
		
		return respond;
	}
	return BAD_RESPOND;
}

static Respond RunDebug (Client* _cltNet, char* _msgToSend, char *_msgRecv)
{
	if(strncmp(_msgToSend, "021103avi04pass", 16) == 0)
	{
		strncpy(_msgRecv, "212", 3);
	}
	if(strncmp(_msgToSend, "041103avi04h-71", 16) == 0)
	{
		strncpy(_msgRecv, "021709204.0.0.4043100", 3);
	}
}