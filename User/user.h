#ifndef __USER_H__
#define __USER_H__

#include "../DsLibrary/GenDLL.h"

#define EQUALS 1

typedef enum UserResults
{
    USER_ADD_GROUP_SUCCESS,
    USER_ADD_GROUP_FAIL,
    USER_GROUP_DUPLICATE,
    USER_GROUP_NOT_FOUND,
    USER_UNINITIALIZED,
    USER_GROUP_NULL,
    USER_GROUP_RMV_SUCCESS

}UserResults;

typedef struct User User;

User* CreateUser(char* _name, char* _password, int _userId, char* _grpName);

UserResults AddGroup(User* _user, char* _group);

UserResults RemoveAGroup(User* _user, char* _group);

size_t NumOfGroups(User* _user);

void DestroyUser(User** _user);

char* GetUserPassWord(User* _user);

int GetUserId(User* _user);

List* GetGroups(User* _user);

#endif /*__USER_H__*/
