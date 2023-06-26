#ifndef __USER_MNG_H__
#define __USER_MNG_H__

#include "../User/user.h"

typedef struct UserMng UserMng; 

typedef enum UserMngErr
{
    USER_MNG_SUCCESS,
    USER_MNG_ADD_FAIL,
    USER_MNG_UNINITIALIZED, 
    USER_MNG_GROUP_DUPLICATE,
    USER_MNG_NULL_ITEM,
    USER_MNG_USER_NOT_FOUND,
    USER_MNG_REMOVAL_FAIL,
    USER_MNG_RMV_SUCCESS,
    USER_MNG_CORRECT_DETAILS,
    USER_MNG_WRONG_PASSWORD,
    USER_MNG_ADD_SUCCESS,
    USER_MNG_USER_ALREADY_EXISTS,
    USER_MNG_ALLOCATION_FAIL

} UserMngErr;

UserMng* UserMngCreate();

UserMngErr AddUser(UserMng* _userMng, char* _name, char* _password, char* _grpName, int _userId);

UserMngErr FindUser(UserMng* _userMng, char* _name, char* _password);

UserMngErr RemoveUser(UserMng* _userMng, char* _name, void** _pKey, void** _pValue);

UserMngErr AddGroupToUser(UserMng* _userMng, char* _name, char* _group);

UserMngErr RemoveGroupFromUser(UserMng* _userMng, char* _name, char* _group);

size_t IsUserListOfGroupsEmpty(UserMng* _userMng, char* _name);

List* GetUserListOfGroups(UserMng* _userMng, char* _name);

void UserMngDestroy(UserMng** _userMng);

#endif /*__USER_MNG_H__*/