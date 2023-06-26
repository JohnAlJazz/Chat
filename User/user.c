#include "user.h"
#include <string.h> /*strcmp*/
#include <stdlib.h> /*malloc*/
#include <stddef.h> /*size_t*/

#include "../DsLibrary/GenDLL.h"
#include "../DsLibrary/ListItr.h"
#include "../DsLibrary/GenEngineDLL.h"

#define NO_USER 0

static int FindGroup(void* _itr, void* _grpName);

/*************************************************/

struct User
{
    char* m_name;
    char* m_password;
    int m_userId;
    List* m_groups;
};

/**************************************************/

User* CreateUser(char* _name, char* _password, int _userId, char* _grpName)
{
    User* user;
    List* groupsList;
    ListResult stat;
    char *name, *pass, *grpName;
    
    if((user = (User*)malloc(sizeof(User))) == NULL)
    {
        return NULL;
    }

    groupsList = ListCreate();

    if(groupsList == NULL)
    {
        free(user);
        return NULL;
    }
    
    if((name = (char*)malloc(strlen(_name) + sizeof(char))) == NULL)
    {
        free(user);
        free(groupsList);
        return NULL;
    }

    if((pass = (char*)malloc(strlen(_password) + sizeof(char))) == NULL)
    {
        free(user);
        free(groupsList);
        free(name);
        return NULL;
    }

    if(_grpName)
    {        
        if((grpName = (char*)malloc(strlen(_grpName) + sizeof(char))) == NULL)
        {
            free(user);
            free(groupsList);
            free(name);
            free(pass);
            return NULL;
        }
        strcpy(grpName, _grpName);
        stat = ListPushHead(groupsList, grpName);
        if(stat != LIST_SUCCESS)
        {
            return NULL;
        }
    }

    strcpy(name, _name);
    strcpy(pass, _password); 

    user->m_name = name;
    user->m_password = pass;
    user->m_userId = _userId;
    user->m_groups = groupsList;    

    return user;
}

UserResults AddGroup(User* _user, char* _group)
{
    ListResult stat;
    void *begin, *end;
    char* grp;
    size_t counter = 0;

    if(!_user)
    {
        return USER_UNINITIALIZED;
    }
    if(!_group)
    {
        return USER_GROUP_NULL;
    }  

    begin = ListItrBegin(_user->m_groups);
    end = ListItrEnd(_user->m_groups);
    counter = ListItrCountIf(begin, end, FindGroup, _group);

    if(counter == 0)
    {
    	grp = (char*)malloc(strlen(_group) + 1);
    	if(!grp)
    	{
    		return USER_ADD_GROUP_FAIL;
    	}
    	strcpy(grp, _group);
        stat = ListPushHead(_user->m_groups, grp);
        if(stat != LIST_SUCCESS)
        {
            return USER_ADD_GROUP_FAIL;
        }
        return USER_ADD_GROUP_SUCCESS;
    } 
    else
    {        
        return USER_GROUP_DUPLICATE;
    }    
}

UserResults RemoveAGroup(User* _user, char* _group)
{
    void *begin, *end;
    void* itr, *removedGrp;

    if(_user == NULL)
    {
        return USER_UNINITIALIZED;
    }

    if(_group == NULL)
    {
        return USER_GROUP_NULL;
    }

    begin = ListItrBegin(_user->m_groups);
    end = ListItrEnd(_user->m_groups);
    itr = ListItrFindFirst(begin, end, FindGroup, (void*)_group);

    if(ListItrEquals(itr, end) == EQUALS)
    {
        return USER_GROUP_NOT_FOUND;
    }

    removedGrp = ListItrRemove(itr);

    return USER_GROUP_RMV_SUCCESS;
}

size_t NumOfGroups(User* _user)
{
    return ListSize(_user->m_groups);
}

void DestroyUser(User** _user)
{
    List* groupsList;

    if(!_user || !*_user)
    {
        return;
    }

    groupsList = GetGroups(*_user); 

    ListDestroy(&groupsList, NULL);

    free((*_user)->m_name);
    free((*_user)->m_password);
    free(*_user);
    
    *_user = NULL;
}

char* GetUserPassWord(User* _user)
{
    if(_user == NULL)
    {
        return NULL;
    }

    return _user->m_password;
}

int GetUserId(User* _user)
{
    if(_user == NULL)
    {
        return NO_USER;
    }

    return _user->m_userId;
}

/***********************/

static int FindGroup(void* _itr, void* _grpName)
{
    void* data;
    data = ListItrGet(_itr);
    
    if(strcmp(((const char*)data), (const char*)_grpName) == 0)
    {
        return 1;
    }

    return 0;
}

List* GetGroups(User* _user)
{
    if(!_user)
    {
        return NULL;
    }

    return _user->m_groups;
}