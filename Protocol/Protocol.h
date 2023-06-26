#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

typedef enum{
	REGISTER = 1,
	LOGIN,
	EXIT,
	JOIN_GROUP,
	CREATE_GROUP,
	LEAVE_GROUP,
	LOG_OUT	
} Tags;

typedef enum {
		USERNAME_ALREADY_EXISTS,
		REGISTRATION_SUCCEEDED,
		GROUPS_FULL_CAPACITY,
		LOGIN_SUCCESS,
		LOGIN_FAILED,
		EXIT_EXECUTED,
		JOIN_GROUP_FAILED,
		JOIN_GROUP_SUCCEEDED,
		CREATE_GROUP_FAILED,
		GROUP_CREATED,
		LEAVING_GROUP_FAILED,
		LEAVING_GROUP_SUCCEEDED,
		LOG_OUT_FAILED,
		LOG_OUT_SUCCEEDED,
		BAD_RESPOND,
		ALLREADY_CONNECTED
} Respond;

/*-----------------main screen-------------------*/

/*
description: packs registeration requst
input: _name: maximum size 49
		_password: maximum size 49
		array to put the result
output: size of message
errors: -1 if one of variables is NULL or bigger than 49
*/
int PackReg(char* _name, char* _password, char* _result);

/*
description: unpackes registration requst
input: pointers for the value
errors: returns if one of variables is NULL
*/
void UnpackReg(char* _toUnpack, char* _name, char* _password);

int PackLogin(char* _name, char* _password, char* _result);
void UnpackLogin(char* _toUnpack, char* _name, char* _password);

int PackExit(char* _name, char* _result);
void UnpackExit(char* _toUnpack, char* _name);

int packMainRespond(Respond _respond, char* _result);
Respond unpackMainRespond(char* _toUnpack);

/*-----------------group screen-------------------*/

int PackJoinGroup(char* _name, char* _groupName, char* _result);
void UnpackJoinGroup(char* _toUnpack, char* _name, char* _groupName);

int PackCreateGroup(char* _name, char* _groupName, char* _result);
void UnpackCreateGroup(char* _toUnpack, char* _name, char* _groupName);

int PackLeaveGroup(char* _name, char* _groupName, char* _result);
void UnpackLeaveGroup(char* _toUnpack, char* _name, char* _groupName);

int PackLogOut(char* _name, char* _result);
void UnpackLogOut(char* _toUnpack, char* _name);

/*
description: respond back from server to client
input: respund according to enum Respond
		_grouIP for multycast chat
		_port for multycast chat
		array to get the result
output: size of result
*/
int packJoinGroupRespond(Respond _respond, char* _groupIP, int _port, char* _result);

Respond UnpackJoinGroupRespond(char* _toUnpack, char* _groupIP, char* _port);

/*
description: respond back from server to client
input: respund according to enum Respond
		_grouIP for multycast chat
		_port for multycast chat
		array to get the result
output: size of result
*/
int packCreateGroupRespond(Respond _respond, char* _groupIP, int _port, char* _result);

Respond UnpackCreateGroupRespond(char* _toUnpack, char* _groupIP, char* _port);

/*
description: respond back from server to client
input: respund according to enum Respond
		array to get the result
output: size of result
*/
int packGroupRespond(Respond _respond, char* _result);

Respond unpackGroupRespond(char* _toUnpack);



/*------------------------general-----------------------------*/
/*
description:
input: _toUnpack: message that needs to unpack
output: enum number of Action, fron 1 to 7 only
errors: -1 if _toUnpack is NULL
*/
Tags GetMsgType(char* _toUnpack);

int IsComplete(char* _message);

#endif /*__PROTOCOL_H__*/
