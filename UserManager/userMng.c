#include <stdlib.h> /*malloc*/
#include <string.h> /*strcmp*/

#include "userMng.h"
#include "../User/user.h"

#include "../DsLibrary/HashMap.h"
#include "../DsLibrary/ListItr.h"
#include "../GroupManager/grpMng.h"

#define CAPACITY 1000

static void ValueDestroy(void* _user);
static int Eq(void* _key1, void* _key2);
static size_t Hash(void* _key);
static UserMngErr Find(UserMng* _userMng, char* _name, char* _password);
static UserMngErr CheckParameters(UserMng* _userMng, char* _name, char* _password);

/********************************************************************/

struct UserMng
{
   HashMap* m_users; 
};

/********************************************************************/

UserMng* UserMngCreate()
{
    UserMng* userMng;
    HashMap* map;

    if((map = HashMap_Create(CAPACITY, Hash, Eq)) == NULL)
    {
        return NULL;
    }

    if((userMng = (UserMng*)malloc(sizeof(UserMng))) == NULL)
    {
        HashMap_Destroy(&map, NULL, NULL);

        return NULL;
    }  

    userMng->m_users = map;

    return userMng;
}

UserMngErr AddUser(UserMng* _userMng, char* _name, char* _password, char* _grpName, int _userId)
{
    User* user;
    Map_Result stat;
    UserMngErr paramsStat;
    char* key;     
    
    if((paramsStat = CheckParameters(_userMng, _name, _password)) != USER_MNG_SUCCESS)
    {        
        return paramsStat;
    }

    if(Find(_userMng, _name, _password) == USER_MNG_USER_NOT_FOUND)
    {        
        user = CreateUser(_name, _password, _userId, _grpName);

        if(!user)
        {
            return USER_MNG_ALLOCATION_FAIL;
        } 

        key = malloc(strlen(_name) + sizeof(char));

        if(key == NULL)
        {
            DestroyUser(&user);

            return USER_MNG_ADD_FAIL;
        }

        strcpy(key, _name);

        stat = HashMap_Insert(_userMng->m_users, key, user);

        if(stat != MAP_SUCCESS)
        {
            return USER_MNG_ADD_FAIL;
        }

        return USER_MNG_ADD_SUCCESS;  
    }     

    return USER_MNG_USER_ALREADY_EXISTS;     
}

UserMngErr FindUser(UserMng* _userMng, char* _name, char* _password)
{
    if(_userMng == NULL)
    {
        return USER_MNG_UNINITIALIZED;
    } 

    if(_name == NULL || _password == NULL)
    {
        return USER_MNG_NULL_ITEM;
    }

    return Find(_userMng, _name, _password);
}

UserMngErr RemoveUser(UserMng* _userMng, char* _name, void** _pKey, void** _pValue)
{
    Map_Result stat;

    if(_userMng == NULL)
    {
        return USER_MNG_UNINITIALIZED;
    }
    if(_name == NULL)
    {
        return USER_MNG_USER_NOT_FOUND;
    } 
    stat = HashMap_Remove(_userMng->m_users, _name, _pKey, _pValue);
    if(stat != MAP_SUCCESS)
    {
        return USER_MNG_REMOVAL_FAIL;
    }
    DestroyUser((User**)(&(*_pValue)));  

    return USER_MNG_RMV_SUCCESS;
}

UserMngErr AddGroupToUser(UserMng* _userMng, char* _name, char* _group)
{
    Map_Result stat;
    void* user;

    if(!_userMng)
    {
        return USER_MNG_UNINITIALIZED;
    } 
    if(!_name || !_group)
    {
        return USER_MNG_NULL_ITEM;
    }
    stat = HashMap_Find(_userMng->m_users, _name, &user);
    if(stat != MAP_SUCCESS)
    {
        return USER_MNG_ADD_FAIL;
    }
    stat = AddGroup((User*)user, _group);
    if(stat == USER_GROUP_DUPLICATE)
    {
        return USER_MNG_GROUP_DUPLICATE;
    }
    return USER_MNG_ADD_SUCCESS;
}

UserMngErr RemoveGroupFromUser(UserMng* _userMng, char* _name, char* _group)
{
    Map_Result stat;
    void* user;

    if(_userMng == NULL)
    {
        return USER_MNG_UNINITIALIZED;
    } 
    if(_name == NULL || _group == NULL)
    {
        return USER_MNG_NULL_ITEM;
    }
    stat = HashMap_Find(_userMng->m_users, _name, &user);
    if(stat != MAP_SUCCESS)
    {
        return USER_MNG_REMOVAL_FAIL;
    }
    stat = RemoveAGroup((User*)user, _group);
    if(stat != USER_GROUP_RMV_SUCCESS)
    {
        return USER_MNG_REMOVAL_FAIL;
    }
    return USER_MNG_RMV_SUCCESS;

}

List* GetUserListOfGroups(UserMng* _userMng, char* _name)
{
    Map_Result stat;
    User* user;
    
    if(!_userMng || !_name)
    {
        return NULL;
    }

    stat = HashMap_Find(_userMng->m_users, _name, (void**)&user);

    if(stat == MAP_KEY_NOT_FOUND_ERROR)
    {
        return NULL;        
    } 

    return GetGroups(user);
}

size_t IsUserListOfGroupsEmpty(UserMng* _userMng, char* _name)
{
    Map_Result stat;
    User* user;
    
    stat = HashMap_Find(_userMng->m_users, _name, (void**)&user);

    if(stat == MAP_KEY_NOT_FOUND_ERROR)
    {
        return USER_MNG_USER_NOT_FOUND;        
    }    
    return NumOfGroups(user); 
}

void UserMngDestroy(UserMng** _userMng)
{
    if(_userMng == NULL || *_userMng == NULL)
    {
        return;
    }

    HashMap_Destroy(&((*_userMng)->m_users), NULL, ValueDestroy);  
    free(*_userMng);   
    *_userMng = NULL;
}

/***************** STATIC FUNCS *****************/

static size_t Hash(void* _key)
{
    char* key = _key;
    int i = 0;
    size_t counter = 0;

    while(key[i] != '\0')
    {
        counter *= key[i];
        i++;
    }

    return counter;
}

static int Eq(void* _key1, void* _key2)
{  
    void *data;
    void *key;
    data = ((Node*)_key1)->m_data;
    key = ((HashMapData*)data)->m_key;
  
    if(strcmp(((const char*)key), ((const char*)_key2)) == 0)
    {
        return 1;
    }
    
    return 0;
}

static void ValueDestroy(void* _user)
{
    DestroyUser((User**)_user);
}

static UserMngErr Find(UserMng* _userMng, char* _name, char* _password)
{
    Map_Result stat;
    User* user;
    const char* password;

    stat = HashMap_Find(_userMng->m_users, _name, (void**)&user);

    if(stat == MAP_KEY_NOT_FOUND_ERROR)
    {
        return USER_MNG_USER_NOT_FOUND;        
    }

    password = GetUserPassWord(user);

    if(stat == MAP_SUCCESS)
    {
        if(strcmp(password, _password) == 0)
        {
            return USER_MNG_CORRECT_DETAILS;
        }

        return USER_MNG_WRONG_PASSWORD;
    }
}

static UserMngErr CheckParameters(UserMng* _userMng, char* _name, char* _password)
{
    if(!_userMng)
    {
        return USER_MNG_UNINITIALIZED;
    }

    if(!_name || !_password)
    {
        return USER_MNG_NULL_ITEM;
    }
    
    return USER_MNG_SUCCESS;
}