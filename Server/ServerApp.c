#include <stdio.h> /* printf() */
#include <stdlib.h> /* malloc(, size_t */

#include "ServerApp.h"
#include "ServerNet.h"
#include "../Protocol/Protocol.h"
#include "../UserManager/userMng.h"
#include "../GroupManager/grpMng.h"

#include "../DsLibrary/GenDLL.h"
#include "../DsLibrary/GenEngineDLL.h"

struct AppData
{
	UserMng* m_userMgr;
	GroupMng* m_groupMgr;
};

static void NewClient(int _clientFd, void* _context, void* _server);
static ServerNetErr GotMsg(int _clientFd, char* _msgFromClient, void* _context, int  _msgSize, Server* _server);

static ServerNetErr ServerRegistration(int _clientFd, char* _msgFromClient, AppData* _appData, Server* _server);
static ServerNetErr ServerLogIn(int _clientFd, char* _msgFromClient, AppData* _appData, Server* _server);
static ServerNetErr ServerExit(char* _msgFromClient, AppData* _appData);
static ServerNetErr ServerJoinGroup(int _clientFd, char* _msgFromClient, AppData* _appData, Server* _server);
static ServerNetErr ServerCreateGroup(int _clientFd, char* _msgFromClient, AppData* _appData, Server* _server);
static ServerNetErr ServerLeaveGroup(int _clientFd, char* _msgFromClient, AppData* _appData, Server* _server);
static ServerNetErr ServerLogOut(int _clientFd, char* _msgFromClient, AppData* _appData, Server* _server);

static void CloseClient(int _clientFd, void* _context, void* _server);
static void Fail(void* _context, char* _error, void* _server);

Server* Create(int _port)
{
	AppData* ptrAppData;
	UserMng* ptrUserMgr;
	GroupMng* ptrGroupMgr;
	Server* ptrServer;
	
	ptrAppData = (AppData*)malloc(sizeof(AppData));
	if(ptrAppData == NULL)
	{
		return NULL;
	}
	ptrUserMgr = UserMngCreate();
	if(ptrUserMgr == NULL)
	{
		free(ptrAppData);
		return NULL;
	}
	ptrGroupMgr = GroupMngCreate();
	if(ptrGroupMgr == NULL)
	{
		free(ptrAppData);
		UserMngDestroy(&ptrUserMgr);
		return NULL;
	}
	ptrAppData -> m_userMgr = ptrUserMgr;
	ptrAppData -> m_groupMgr = ptrGroupMgr;

	ptrServer = CreateServer(NewClient, GotMsg, CloseClient, Fail, _port, ptrAppData);
	if(ptrServer == NULL)
	{
		free(ptrAppData);
		UserMngDestroy(&ptrUserMgr);
		GroupMngDestroy(&ptrGroupMgr);
		return NULL;
	}
	return ptrServer;
}

void Run(Server* _srv)
{
	RunServer(_srv);
}

void Destroy(Server* _srv)
{
	
	if(_srv)
	{
		UserMngDestroy(&(((AppData*)GetContext(_srv)) -> m_userMgr));
		GroupMngDestroy(&(((AppData*)GetContext(_srv)) -> m_groupMgr));
		free(GetContext(_srv));
		DestroyServer(_srv);
	}
}

/************************************     static functions     *****************************/

static void NewClient(int _clientFd, void* _context, void* _server)
{
	
}

static ServerNetErr GotMsg(int _clientFd, char* _msgFromClient, void* _context, int _msgSize, Server* _server)
{
	Tags tag;
	tag = GetMsgType(_msgFromClient);

	switch(tag)
	{
		case 1:
			return ServerRegistration(_clientFd, _msgFromClient, (AppData*)_context, _server);
		case 2:
			return ServerLogIn(_clientFd, _msgFromClient, (AppData*)_context, _server);
		case 3:
			return ServerExit(_msgFromClient, (AppData*)_context);
		case 4:
			return ServerJoinGroup(_clientFd, _msgFromClient, (AppData*)_context, _server);
		case 5:
			return ServerCreateGroup(_clientFd, _msgFromClient, (AppData*)_context, _server);
		case 6:
			return ServerLeaveGroup(_clientFd, _msgFromClient, (AppData*)_context, _server);
		case 7:
			return ServerLogOut(_clientFd, _msgFromClient, (AppData*)_context, _server);
	}
}

static void CloseClient(int _clientFd, void* _context, void* _server)
{
	
}

static void Fail(void* _context, char* _error, void* _server)
{
	
}

static ServerNetErr ServerRegistration(int _clientFd, char* _msgFromClient, AppData* _appData, Server* _server)
{
	char name[50], password[50], respond[30];
	UserMngErr statusUser;
	
	UnpackReg(_msgFromClient, name, password);
	statusUser = AddUser(_appData->m_userMgr, name, password, NULL, _clientFd);

	if(statusUser != USER_MNG_ADD_SUCCESS)
	{			
		packMainRespond(USERNAME_ALREADY_EXISTS, respond);
		SendMsg(_server, _clientFd, respond);
		return GOT_MSG_SUCCESS;
	}
	packMainRespond(REGISTRATION_SUCCEEDED, respond);
	SendMsg(_server, _clientFd, respond);
	return GOT_MSG_SUCCESS;
}

static ServerNetErr ServerLogIn(int _clientFd, char* _msgFromClient, AppData* _appData, Server* _server)
{
	char name[50], password[50], respond[30];
	UserMngErr statusUser;
	
	UnpackLogin(_msgFromClient, name, password);
	statusUser = FindUser(_appData->m_userMgr, name, password);
	if(statusUser == USER_MNG_USER_NOT_FOUND || statusUser == USER_MNG_WRONG_PASSWORD)
	{		
		packMainRespond(LOGIN_FAILED, respond);
		SendMsg(_server, _clientFd, respond);
		return GOT_MSG_SUCCESS;
	}
	packMainRespond(LOGIN_SUCCESS, respond);
	SendMsg(_server, _clientFd, respond);
	return GOT_MSG_SUCCESS;
}

static ServerNetErr ServerExit(char* _msgFromClient, AppData* _appData)
{
	void *pKey, *pValue;
	char name[50];	
	UserMngErr status;

	UnpackExit(_msgFromClient, name);
	status = RemoveUser(_appData-> m_userMgr, name, &pKey, &pValue);
	if(status == USER_GROUP_RMV_SUCCESS)
	{		
		return CLOSE_AND_REMOVE_SOCK;
	}
	return status;
}

static ServerNetErr ServerJoinGroup(int _clientFd, char* _msgFromClient, AppData* _appData, Server* _server)
{
	int port;
	char name[50], respond[30], groupName[30], *ip, *dummy = "1";
	UserMngErr statusUser;
	GrpMngResults statusGroup;
	
	UnpackJoinGroup(_msgFromClient, name, groupName);
	statusGroup = AddUserToGroup(_appData->m_groupMgr, groupName, name);
	if(statusGroup != GRP_MNG_ADD_SUCCESS)
	{		
		packJoinGroupRespond(JOIN_GROUP_FAILED, dummy, 0, respond);
		SendMsg(_server, _clientFd, respond);
		return GOT_MSG_SUCCESS;
	}
	statusUser = AddGroupToUser(_appData-> m_userMgr, name, groupName);
	if(statusUser != USER_MNG_ADD_SUCCESS)
	{		
		RemoveUserFromGroup(_appData->m_groupMgr, groupName, name);
		packJoinGroupRespond(JOIN_GROUP_FAILED, dummy, 0, respond);
		SendMsg(_server, _clientFd, respond);
		return GOT_MSG_SUCCESS;
	}
	ip = GetIp(_appData->m_groupMgr, groupName);
	port = GetPort();
	packJoinGroupRespond(JOIN_GROUP_SUCCEEDED, ip, port, respond);
	SendMsg(_server, _clientFd, respond);
	return GOT_MSG_SUCCESS;
}

static ServerNetErr ServerCreateGroup(int _clientFd, char* _msgFromClient, AppData* _appData, Server* _server)
{
	int port;
	char name[50], respond[30], groupName[30], *ip, *dummy = "1";
	UserMngErr statusUser;
	GrpMngResults statusGroup;
	
	UnpackCreateGroup(_msgFromClient, name, groupName);
	statusGroup = CreateNewGroup(_appData->m_groupMgr, groupName, name);
	if(statusGroup != GRP_MNG_ADD_SUCCESS)
	{
		packCreateGroupRespond(CREATE_GROUP_FAILED, dummy, 0, respond);
		SendMsg(_server, _clientFd, respond);
		return GOT_MSG_SUCCESS;
	}
	statusUser = AddGroupToUser(_appData->m_userMgr, name, groupName);
	if(statusUser != USER_MNG_ADD_SUCCESS)
	{
		RemoveGroup(_appData->m_groupMgr, groupName);
		packCreateGroupRespond(CREATE_GROUP_FAILED, dummy, 0, respond);
		SendMsg(_server, _clientFd, respond);
		return GOT_MSG_SUCCESS;
	}
	ip = GetIp(_appData->m_groupMgr, groupName);
	port = GetPort();
	packCreateGroupRespond(GROUP_CREATED, ip, port, respond);
	SendMsg(_server, _clientFd, respond);
	return GOT_MSG_SUCCESS;
}

static ServerNetErr ServerLeaveGroup(int _clientFd, char* _msgFromClient, AppData* _appData, Server* _server)
{
	char name[50], respond[30], groupName[30];
	UserMngErr statusUser;
	GrpMngResults statusGroup;
	
	UnpackLeaveGroup(_msgFromClient, name, groupName);
	statusUser = RemoveGroupFromUser(_appData->m_userMgr, name, groupName);
	if(statusUser != USER_MNG_RMV_SUCCESS)
	{
		packGroupRespond(LEAVING_GROUP_FAILED, respond);
		SendMsg(_server, _clientFd, respond);
		return GOT_MSG_SUCCESS;
	}
	statusGroup = RemoveUserFromGroup(_appData-> m_groupMgr, groupName, name);
	if(statusGroup != GRP_MNG_REMOVAL_SUCCESS)
	{
		AddGroupToUser(_appData-> m_userMgr, name, groupName);
		packGroupRespond(LEAVING_GROUP_FAILED, respond);
		SendMsg(_server, _clientFd, respond);
		return GOT_MSG_SUCCESS;
	}
	packGroupRespond(LEAVING_GROUP_SUCCEEDED, respond);
	SendMsg(_server, _clientFd, respond);
	return GOT_MSG_SUCCESS;
}

static ServerNetErr ServerLogOut(int _clientFd, char* _msgFromClient, AppData* _appData, Server* _server)
{
	size_t numOfGroups;
	ListItr itr, end;
	void* data;
	char name[50], respond[30], groupName[30];
	GrpMngResults statusGroup;
	List* list;
	
	UnpackLogOut(_msgFromClient, name);
	numOfGroups = IsUserListOfGroupsEmpty(_appData->m_userMgr, name);	
	
	if(!numOfGroups)
	{
		packGroupRespond(LOG_OUT_SUCCEEDED, respond);
		SendMsg(_server, _clientFd, respond);
		return GOT_MSG_SUCCESS;
	}

	list = GetUserListOfGroups(_appData->m_userMgr, name);
	
	itr = ListItrBegin(list);
	end = ListItrEnd(list);

	while(itr != end)
	{
		data = ListItrGet(itr);
		statusGroup = RemoveUserFromGroup(_appData->m_groupMgr, (char*)data, name);
		if(statusGroup != GRP_MNG_REMOVAL_SUCCESS)
		{						
			packGroupRespond(LEAVING_GROUP_FAILED, respond);
			SendMsg(_server, _clientFd, respond);		
			return GOT_MSG_SUCCESS;
		}		
		itr = ListItrNext(itr);
	}	
	packGroupRespond(LOG_OUT_SUCCEEDED, respond);
	SendMsg(_server, _clientFd, respond);
	return GOT_MSG_SUCCESS;
}