#include <stdlib.h>
#include <stdio.h>		/*snprintf*/
#include <string.h> 	/*strlen, strncpy, atoi*/

#include "Protocol.h"

#define MAX_LENGTH 100
#define MAX_SINGLE_VAL_SIZE 49
#define MTU 1500
#define LENGTH_SIZE 2
#define TAG_SIZE 2

static int MyAtoi(char *str);

int PackReg(char* _name, char* _password, char* _result)
{
	int nameSize, passSize, length;
	
	if((_name == NULL) || (_password == NULL)){
		return -1;
	}
	nameSize = strlen(_name);
	passSize = strlen(_password);
	if ((nameSize > MAX_SINGLE_VAL_SIZE) || (passSize > MAX_SINGLE_VAL_SIZE))
	{
		return -1;
	}
	length = nameSize + passSize + LENGTH_SIZE * 2;	
	snprintf(_result, MAX_LENGTH, "%02d%02d%02d%s%02d%s", REGISTER, length, nameSize, _name, passSize, _password);	
	return TAG_SIZE + LENGTH_SIZE + length;	
}

void UnpackReg(char* _toUnpack, char* _name, char* _password)
{
	char cTotalSize[MAX_SINGLE_VAL_SIZE + 1], cNameSize[MAX_SINGLE_VAL_SIZE + 1], cPassSize[MAX_SINGLE_VAL_SIZE + 1], toUnpack[MAX_LENGTH]; 
	char *test;
	int totalSize, nameSize, passSize;

	if((_toUnpack == NULL) || (_name == NULL) || (_password == NULL)){
		return;
	}
	strncpy(toUnpack, _toUnpack, MAX_LENGTH);
	strncpy(cTotalSize, toUnpack + TAG_SIZE, LENGTH_SIZE);
	totalSize = MyAtoi(cTotalSize);
	strncpy(cNameSize, toUnpack + TAG_SIZE + LENGTH_SIZE, LENGTH_SIZE);
	nameSize = MyAtoi(cNameSize);
	strncpy(cPassSize, toUnpack + nameSize + TAG_SIZE + LENGTH_SIZE * 2, LENGTH_SIZE);
	passSize = MyAtoi(cPassSize);
	strncpy(_name, toUnpack + TAG_SIZE + LENGTH_SIZE * 2, nameSize);
	strncpy(_password, toUnpack + TAG_SIZE + LENGTH_SIZE * 3 + nameSize, passSize);
	_name[nameSize] = '\0';
	_password[passSize] = '\0';
}

int PackLogin(char* _name, char* _password, char* _result)
{
	int nameSize, passSize, length;
	
	if((_name == NULL) || (_password == NULL)){
		return -1;
	}
	nameSize = strlen(_name);
	passSize = strlen(_password);
	if ((nameSize > MAX_SINGLE_VAL_SIZE) || (passSize > MAX_SINGLE_VAL_SIZE))
	{
		return -1;
	}
	length = nameSize + passSize + LENGTH_SIZE * 2;
	snprintf(_result, MAX_LENGTH, "%02d%02d%02d%s%02d%s", LOGIN, length, nameSize, _name, passSize, _password);
	return TAG_SIZE + LENGTH_SIZE + length;	
}

void UnpackLogin(char* _toUnpack, char* _name, char* _password)
{
	char cTotalSize[MAX_SINGLE_VAL_SIZE + 1], cNameSize[MAX_SINGLE_VAL_SIZE + 1], cPassSize[MAX_SINGLE_VAL_SIZE + 1], toUnpack[MAX_LENGTH]; 
	char *test;
	int totalSize, nameSize, passSize;

	if((_toUnpack == NULL) || (_name == NULL) || (_password == NULL)){
		return;
	}
	strncpy(toUnpack, _toUnpack, MAX_LENGTH);
	strncpy(cTotalSize, toUnpack + TAG_SIZE, LENGTH_SIZE);
	totalSize = MyAtoi(cTotalSize);
	strncpy(cNameSize, toUnpack + TAG_SIZE + LENGTH_SIZE, LENGTH_SIZE);
	nameSize = MyAtoi(cNameSize);
	strncpy(cPassSize, toUnpack + nameSize + TAG_SIZE + LENGTH_SIZE*2, LENGTH_SIZE);
	passSize = MyAtoi(cPassSize);
	strncpy(_name, toUnpack + TAG_SIZE + LENGTH_SIZE*2, nameSize);
	strncpy(_password, toUnpack + TAG_SIZE + LENGTH_SIZE*3 + nameSize, passSize);
	_name[nameSize] = '\0';
	_password[passSize] = '\0';
}
int PackExit(char* _name, char* _result)
{
	int nameSize, length, totalLength;
	
	if(!_name)
	{
		return -1;
	}
	nameSize = strlen(_name);	
	if (nameSize > MAX_SINGLE_VAL_SIZE)
	{
		return -1;
	}	
	length = nameSize;
	snprintf(_result, MAX_LENGTH, "%02d%02d%s", EXIT, length, _name);	
	totalLength = TAG_SIZE + LENGTH_SIZE + length;
	_result[totalLength] = '\0';
	return totalLength;
}

void UnpackExit(char* _toUnpack, char* _name)
{
	char cTotalSize[MAX_SINGLE_VAL_SIZE + 1], toUnpack[MAX_LENGTH]; 
	char *test;
	int totalSize, nameSize;

	if((_toUnpack == NULL) || (_name == NULL)){
		return;
	}
	strncpy(toUnpack, _toUnpack, MAX_LENGTH);
	strncpy(cTotalSize, toUnpack + TAG_SIZE, LENGTH_SIZE);
	totalSize = MyAtoi(cTotalSize);
	strncpy(_name, toUnpack + TAG_SIZE + LENGTH_SIZE, totalSize);
	_name[totalSize] = '\0';

}

int packMainRespond(Respond _respond, char* _result)
{
	switch (_respond)
	{
		case USERNAME_ALREADY_EXISTS:
			strncpy(_result, "111", 3);
			return 3;
		case REGISTRATION_SUCCEEDED:
			strncpy(_result, "112", 3);
			return 3;
		case LOGIN_FAILED:
			strncpy(_result, "211", 3);
			return 3;
		case LOGIN_SUCCESS:
			strncpy(_result, "212", 3);
			return 3;
		default:
			break;
	}
}

Respond unpackMainRespond(char* _toUnpack)
{
	if ((strncmp(_toUnpack,"111",3)) == 0)
	{
		return USERNAME_ALREADY_EXISTS;
	}
	else if((strncmp(_toUnpack,"112",3)) == 0)
	{
		return REGISTRATION_SUCCEEDED;
	}
	else if((strncmp(_toUnpack,"211",3)) == 0)
	{
		return LOGIN_FAILED;
	}
	else if((strncmp(_toUnpack,"212",3)) == 0)
	{
		return LOGIN_SUCCESS;
	}
	else{
		return BAD_RESPOND;
	}
}

/*-----------------group screen-------------------*/

int PackJoinGroup(char* _name, char* _groupName, char* _result)
{
	int nameSize, groupSize, length;
	
	if((_name == NULL) || (_groupName == NULL)){
		return -1;
	}
	nameSize = strlen(_name);
	groupSize = strlen(_groupName);
	if ((nameSize >MAX_SINGLE_VAL_SIZE) || (groupSize > MAX_SINGLE_VAL_SIZE)){
		return -1;
	}
	length = nameSize + groupSize + LENGTH_SIZE * 2;
	snprintf(_result, MAX_LENGTH, "%02d%02d%02d%s%02d%s", JOIN_GROUP, length, nameSize, _name, groupSize, _groupName);
	return TAG_SIZE + LENGTH_SIZE + length;	
}

void UnpackJoinGroup(char* _toUnpack, char* _name, char* _groupName)
{
	char cTotalSize[MAX_SINGLE_VAL_SIZE + 1], cNameSize[MAX_SINGLE_VAL_SIZE + 1], cGroupSize[MAX_SINGLE_VAL_SIZE + 1], toUnpack[MAX_LENGTH]; 
	char *test;
	int totalSize, nameSize, groupSize;

	if((_toUnpack == NULL) || (_name == NULL) || (_groupName == NULL)){
		return;
	}
	strncpy(toUnpack, _toUnpack, MAX_LENGTH);
	strncpy(cTotalSize, toUnpack + TAG_SIZE, LENGTH_SIZE);
	totalSize = MyAtoi(cTotalSize);
	strncpy(cNameSize, toUnpack + TAG_SIZE + LENGTH_SIZE, LENGTH_SIZE);
	nameSize = MyAtoi(cNameSize);
	strncpy(cGroupSize, toUnpack + nameSize + TAG_SIZE + LENGTH_SIZE*2, LENGTH_SIZE);
	groupSize = MyAtoi(cGroupSize);
	strncpy(_name, toUnpack + TAG_SIZE + LENGTH_SIZE*2, nameSize);
	strncpy(_groupName, toUnpack + TAG_SIZE + LENGTH_SIZE*3 + nameSize, groupSize);
	_name[nameSize] = '\0';
	_groupName[groupSize] = '\0';
}

int PackCreateGroup(char* _name, char* _groupName, char* _result)
{
	int nameSize, groupSize, length;
	
	if((_name == NULL) || (_groupName == NULL)){
		return -1;
	}
	nameSize = strlen(_name);
	groupSize = strlen(_groupName);
	if ((nameSize >MAX_SINGLE_VAL_SIZE) || (groupSize > MAX_SINGLE_VAL_SIZE)){
		return -1;
	}
	length = nameSize + groupSize + LENGTH_SIZE * 2;
	snprintf(_result, MAX_LENGTH, "%02d%02d%02d%s%02d%s", CREATE_GROUP, length, nameSize, _name, groupSize, _groupName);
	return TAG_SIZE + LENGTH_SIZE + length;	
}

void UnpackCreateGroup(char* _toUnpack, char* _name, char* _groupName)
{
	char cTotalSize[MAX_SINGLE_VAL_SIZE + 1], cNameSize[MAX_SINGLE_VAL_SIZE + 1], cGroupSize[MAX_SINGLE_VAL_SIZE + 1], toUnpack[MAX_LENGTH]; 
	char *test;
	int totalSize, nameSize, groupSize;

	if((_toUnpack == NULL) || (_name == NULL) || (_groupName == NULL))
	{
		return;
	}
	strncpy(toUnpack, _toUnpack, MAX_LENGTH);
	strncpy(cTotalSize, toUnpack + TAG_SIZE, LENGTH_SIZE);
	totalSize = MyAtoi(cTotalSize);
	strncpy(cNameSize, toUnpack + TAG_SIZE + LENGTH_SIZE, LENGTH_SIZE);
	nameSize = MyAtoi(cNameSize);
	strncpy(cGroupSize, toUnpack + nameSize + TAG_SIZE + LENGTH_SIZE*2, LENGTH_SIZE);
	groupSize = MyAtoi(cGroupSize);
	strncpy(_name, toUnpack + TAG_SIZE + LENGTH_SIZE*2, nameSize);
	strncpy(_groupName, toUnpack + TAG_SIZE + LENGTH_SIZE*3 + nameSize, groupSize);
	_name[nameSize] = '\0';
	_groupName[groupSize] = '\0';
}

int PackLeaveGroup(char* _name, char* _groupName, char* _result)
{
	int nameSize, groupSize, length;
	
	if((_name == NULL) || (_groupName == NULL)){
		return -1;
	}
	nameSize = strlen(_name);
	groupSize = strlen(_groupName);
	if ((nameSize >MAX_SINGLE_VAL_SIZE) || (groupSize > MAX_SINGLE_VAL_SIZE))
	{
		return -1;
	}
	length = nameSize + groupSize + LENGTH_SIZE * 2;
	snprintf(_result, MAX_LENGTH, "%02d%02d%02d%s%02d%s", LEAVE_GROUP, length, nameSize, _name, groupSize, _groupName);
	return TAG_SIZE + LENGTH_SIZE + length;	
}

void UnpackLeaveGroup(char* _toUnpack, char* _name, char* _groupName)
{
	char cTotalSize[MAX_SINGLE_VAL_SIZE + 1], cNameSize[MAX_SINGLE_VAL_SIZE + 1], cGroupSize[MAX_SINGLE_VAL_SIZE + 1], toUnpack[MAX_LENGTH]; 
	char *test;
	int totalSize, nameSize, groupSize;

	if((_toUnpack == NULL) || (_name == NULL) || (_groupName == NULL)){
		return;
	}
	strncpy(toUnpack, _toUnpack, MAX_LENGTH);
	strncpy(cTotalSize, toUnpack + TAG_SIZE, LENGTH_SIZE);
	totalSize = MyAtoi(cTotalSize);
	strncpy(cNameSize, toUnpack + TAG_SIZE + LENGTH_SIZE, LENGTH_SIZE);
	nameSize = MyAtoi(cNameSize);
	strncpy(cGroupSize, toUnpack + nameSize + TAG_SIZE + LENGTH_SIZE*2, LENGTH_SIZE);
	groupSize = MyAtoi(cGroupSize);
	strncpy(_name, toUnpack + TAG_SIZE + LENGTH_SIZE*2, nameSize);
	strncpy(_groupName, toUnpack + TAG_SIZE + LENGTH_SIZE*3 + nameSize, groupSize);
	_name[nameSize] = '\0';
	_groupName[groupSize] = '\0';
}

int PackLogOut(char* _name, char* _result)
{
	int nameSize, length;
	
	if(_name == NULL)
	{
		return -1;
	}
	nameSize = strlen(_name);
	if (nameSize > MAX_SINGLE_VAL_SIZE)
	{
		return -1;
	}
	length = nameSize;
	snprintf(_result, MAX_LENGTH, "%02d%02d%s", LOG_OUT, length, _name);
	
	return TAG_SIZE + LENGTH_SIZE + length;	
}

void UnpackLogOut(char* _toUnpack, char* _name)
{
	char cTotalSize[MAX_SINGLE_VAL_SIZE + 1], toUnpack[MAX_LENGTH]; 
	char *test;
	int totalSize, nameSize;

	if((_toUnpack == NULL) || (_name == NULL)){
		return;
	}
	strncpy(toUnpack, _toUnpack, MAX_LENGTH);
	strncpy(cTotalSize, toUnpack + TAG_SIZE, LENGTH_SIZE);
	totalSize = MyAtoi(cTotalSize);
	strncpy(_name, toUnpack + TAG_SIZE + LENGTH_SIZE, totalSize);
	_name[totalSize] = '\0';
}

int packJoinGroupRespond(Respond _respond, char* _groupIP, int _port, char* _result)
{
	int respSize = 2, portSize = 4,IpSize, length;
	char dummyIP[2] = "1";

	if(_groupIP == NULL){
		strncpy(_groupIP, dummyIP, 2);
	}
	IpSize = strlen(_groupIP);
	length = respSize + IpSize + portSize + LENGTH_SIZE * 3;
	snprintf(_result, MAX_LENGTH, "%02d%02d%02d%02d%02d%s%02d%04d", JOIN_GROUP, length, respSize, _respond, IpSize, _groupIP, portSize, _port);
	return TAG_SIZE + LENGTH_SIZE + length;	
}


Respond UnpackJoinGroupRespond(char* _toUnpack, char* _groupIP, char* _port)
{
	Respond respond;
	char cTotalSize[MAX_SINGLE_VAL_SIZE + 1], cIpSize[MAX_SINGLE_VAL_SIZE + 1], cRespond[MAX_SINGLE_VAL_SIZE+1]; 
	int totalSize, respSize = 2, ipSize, portSize = 4;

	if((_toUnpack == NULL) || (_groupIP == NULL)){
		return;
	}
	strncpy(cTotalSize, _toUnpack + TAG_SIZE, LENGTH_SIZE);
	totalSize = MyAtoi(cTotalSize);
	strncpy(cIpSize, _toUnpack + respSize + TAG_SIZE + LENGTH_SIZE*2, LENGTH_SIZE);
	ipSize = MyAtoi(cIpSize);
	strncpy(cRespond, _toUnpack + TAG_SIZE + LENGTH_SIZE * 2, respSize);
	respond = MyAtoi(cRespond);
	strncpy(_groupIP, _toUnpack + TAG_SIZE + LENGTH_SIZE * 3 + respSize, ipSize);
	strncpy(_port, _toUnpack + TAG_SIZE + LENGTH_SIZE * 4 + respSize + ipSize, portSize);
	_groupIP[ipSize] = '\0';
	_port[portSize] = '\0';
	return respond;
}

int packCreateGroupRespond(Respond _respond, char* _groupIP, int _port, char* _result)
{
	int respSize = 2, portSize = 4,IpSize, length;
	char dummyIP[2] = "1";

	if(_groupIP == NULL){
		strncpy(_groupIP, dummyIP, 2);
	}
	IpSize = strlen(_groupIP);
	length = respSize + IpSize + portSize + LENGTH_SIZE * 3;
	snprintf(_result, MAX_LENGTH, "%02d%02d%02d%02d%02d%s%02d%04d", CREATE_GROUP, length, respSize, _respond, IpSize, _groupIP, portSize, _port);
	return TAG_SIZE + LENGTH_SIZE + length;	
}

Respond UnpackCreateGroupRespond(char* _toUnpack, char* _groupIP, char* _port)
{
	Respond respond;
	char cTotalSize[MAX_SINGLE_VAL_SIZE + 1], cIpSize[MAX_SINGLE_VAL_SIZE + 1], cRespond[MAX_SINGLE_VAL_SIZE+1]; 
	int totalSize, respSize = 2, ipSize, portSize = 4;

	if((_toUnpack == NULL) || (_groupIP == NULL)){
		return;
	}
	strncpy(cTotalSize, _toUnpack + TAG_SIZE, LENGTH_SIZE);
	totalSize = MyAtoi(cTotalSize);
	strncpy(cIpSize, _toUnpack + respSize + TAG_SIZE + LENGTH_SIZE*2, LENGTH_SIZE);
	ipSize = MyAtoi(cIpSize);
	strncpy(cRespond, _toUnpack + TAG_SIZE + LENGTH_SIZE * 2, respSize);
	respond = MyAtoi(cRespond);
	strncpy(_groupIP, _toUnpack + TAG_SIZE + LENGTH_SIZE * 3 + respSize, ipSize);
	strncpy(_port, _toUnpack + TAG_SIZE + LENGTH_SIZE * 4 + respSize + ipSize, portSize);
	_groupIP[ipSize] = '\0';
	_port[portSize] = '\0';
	return respond;
}

int packGroupRespond(Respond _respond, char* _result)
{
	switch (_respond){
		case LEAVING_GROUP_FAILED:
			strncpy(_result, "611", 3);
			return 3;
		case LEAVING_GROUP_SUCCEEDED:
			strncpy(_result, "612", 3);
			return 3;
		case LOG_OUT_FAILED:
			strncpy(_result, "711", 3);
			return 3;
		case LOG_OUT_SUCCEEDED:
			strncpy(_result, "712", 3);
			return 3;
		default:
			break;
	}
}

Respond unpackGroupRespond(char* _toUnpack)
{
	if ((strncmp(_toUnpack,"611",3)) == 0){
		return LEAVING_GROUP_FAILED;
	}
	else if((strncmp(_toUnpack,"612",3)) == 0){
		return LEAVING_GROUP_SUCCEEDED;
	}
	else if((strncmp(_toUnpack,"711",3)) == 0){
		return LOG_OUT_FAILED;
	}
	else if((strncmp(_toUnpack,"712",3)) == 0){
		return LOG_OUT_SUCCEEDED;
	}
	else{
		return BAD_RESPOND;
	}
}

/*------------------------general-----------------------------*/

Tags GetMsgType(char* _toUnpack)
{
	char tagChar[3];
	int tag;

	if(_toUnpack == NULL){
		return -1;
	}
	strncpy(tagChar, _toUnpack, 2);
	tag = MyAtoi(tagChar);
	return tag;
}

int IsComplete(char* _message)
{
	char check[MTU], length[LENGTH_SIZE + 1];
	int len, actualSize;

	if(_message == NULL){
		return -1;
	}
	strncpy(check, _message, MTU);
	actualSize = strlen(check);
	strncpy(length, _message + 2, LENGTH_SIZE);
	len = MyAtoi(length);
	if(len != actualSize){
		return 0;
	}
	return 1;
}

static int MyAtoi(char *str)
{
	int i, buf = 0, result = 0, sLen = LENGTH_SIZE;
	for (i = 0; i < sLen; i++){
		if (str[i] >= '0' && str[i] <= '9'){
			buf = str[i] - '0';
			result = result * 10 + buf;
		}
		else{
			break;
		}
	}
	return result;
	
}
