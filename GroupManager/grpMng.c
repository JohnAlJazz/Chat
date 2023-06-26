#include <stdlib.h> /*malloc*/
#include <string.h> /*strcmp*/
#include <stdio.h> /*sprintf*/

#include "grpMng.h"
#include "../Group/groups.h"

#include "../DsLibrary/HashMap.h"
#include "../DsLibrary/GenQ.h"
#include "../DsLibrary/ListItr.h"
#include "../DsLibrary/GenEngineDLL.h"

#define CAPACITY 200
#define QUEUE_SIZE 100
#define QUEUE_FAIL 0
#define IP_POOL 100
#define PORT 2000

static void ValDestroy(void* _grp);
static size_t Hash(void* _key);
static GrpMngResults Find(GroupMng* _grpMng, char* _grpName, Group** _getGrp);
static GrpMngResults CheckParameters(GroupMng* _GRPMng, char* _name, char* _password);
static int SearchForGroup(void* _key1, void* _key2);
static char* GetIpFromQueue(Queue* _queue);
static void FillQueueWithIpAddresses(Queue* _que);

/********************************************************************/

struct GroupMng   
{
    HashMap* m_groups;
    Queue* m_multiCastIpQueue;
};

/********************************************************************/

GroupMng* GroupMngCreate()
{
    GroupMng* grpMng;
    HashMap* map;
    Queue* que;

    if((map = HashMap_Create(CAPACITY, Hash, SearchForGroup)) == NULL)
    {
        return NULL;
    }
    if((que = QueueCreate(QUEUE_SIZE)) == NULL)
    {
        free(map);
        return NULL;
    }
    if((grpMng = (GroupMng*)malloc(sizeof(GroupMng))) == NULL)
    {
        free(map);
        free(que);
        return NULL;
    }   
    FillQueueWithIpAddresses(que); 
    grpMng->m_groups = map;
    grpMng->m_multiCastIpQueue = que;

    return grpMng;
}

GrpMngResults CreateNewGroup(GroupMng* _grpMng, char* _grpName, char* _name)
{
    char* multiIp, *grpN;
    int status;
    Group* grp, *getGrp;
    Map_Result stat;

    if((status = CheckParameters(_grpMng, _grpName, _name)) != GRP_MNG_SUCCESS)
    {
        return status;
    }
    
    if(Find(_grpMng, _grpName, &getGrp) == GRP_MNG_GRP_NOT_FOUND)
    {
        multiIp = GetIpFromQueue(_grpMng->m_multiCastIpQueue);
        if(multiIp == NULL)
        {
            return GRP_MNG_PULLING_IP_FAIL;
        }
        grp = CreateGroup(_grpName, _name, multiIp);
        if(grp == NULL)
        {
            return GRP_MNG_CREATE_GRP_FAIL;
        } 
        grpN = malloc(strlen(_grpName) + sizeof(char));
        if(grpN == NULL)
        {
            return GRP_MNG_CREATE_GRP_FAIL;
        }
        strcpy(grpN, _grpName);
        stat = HashMap_Insert(_grpMng->m_groups, grpN, grp);
        if(stat != MAP_SUCCESS)
        {
            return GRP_MNG_ADD_FAIL;
        }
        return GRP_MNG_ADD_SUCCESS;  
    }    
    return GRP_MNG_GRP_ALREADY_EXISTS;    
}

GrpMngResults AddUserToGroup(GroupMng* _grpMng, char* _grpName, char* _name)
{
    Group *getGrp;
    int status;

    if((status = CheckParameters(_grpMng, _grpName, _name)) != GRP_MNG_SUCCESS)
    {
        return status;
    }
    /*getGrp = (Group*)malloc(sizeof(void*));*/
    if(Find(_grpMng, _grpName, &getGrp) == GRP_MNG_GRP_FOUND)
    {
        status = AddMember(getGrp, _name);
        if(status == GRP_ADD_MEMBER_SUCCESS)
        {
            return GRP_MNG_ADD_SUCCESS;
        }         
    }
    return GRP_MNG_GRP_NOT_FOUND;
}

GrpMngResults RemoveUserFromGroup(GroupMng* _grpMng, char* _grpName, char* _name)
{
    Group *getGrp;
    int status;
    GrpMngResults stat;
    char* ip;

    if((status = CheckParameters(_grpMng, _grpName, _name)) != GRP_MNG_SUCCESS)
    {
        return status;
    }
    if(Find(_grpMng, _grpName, &getGrp) == GRP_MNG_GRP_NOT_FOUND)
    {
        return GRP_MNG_GRP_NOT_FOUND; 
    }
    stat = RemoveMember(getGrp, _name); 
    if(stat == GRP_IS_EMPTY)
    {
        ip = GetIp(_grpMng, _grpName);
        QueueInsert(_grpMng->m_multiCastIpQueue, ip);
        GroupDestroy(&(getGrp));
    }
    return GRP_MNG_REMOVAL_SUCCESS;   
}

char* GetIp(GroupMng* _grpM, char* _grpName)
{
    Map_Result stat;
    void* group;

    stat = HashMap_Find(_grpM->m_groups, _grpName, &group);
    {
       if(stat != MAP_SUCCESS)
       {
            return NULL;
       } 
       return GroupIp(group);
    }
}

int GetPort()
{
    return PORT;
}

void RemoveGroup(GroupMng* _grpM, char* _grpName)
{
    void* value;
    Map_Result stat;
    if(_grpM == NULL || _grpName == NULL)
    {
        return;
    }
    stat = HashMap_Find(_grpM->m_groups, _grpName, &value);
    if(stat != MAP_SUCCESS)
    {
        return;
    }
    GroupDestroy((Group**)&value);
}

void GroupMngDestroy(GroupMng** _grpMng)
{
    if(_grpMng == NULL || *_grpMng == NULL)
    {
        return;
    }
    HashMap_Destroy(&(*_grpMng)->m_groups, NULL, ValDestroy);
    QueueDestroy(&(*_grpMng)->m_multiCastIpQueue, NULL);
    free(*_grpMng);
    *_grpMng = NULL;
}


/***************** STATIC FUNCS *****************/

static size_t Hash(void* _key)
{
    char* key = _key;
    int i = 0;
    size_t counter = 1;

    while(key[i] != '\0')
    {
        counter *= key[i];
        i++;
    }
    return counter;
}

static void ValDestroy(void* _grp)
{
    GroupDestroy((Group**)&_grp);
}

static GrpMngResults Find(GroupMng* _grpMng, char* _grpName, Group** _getGrp)
{
    Map_Result stat;   
    stat = HashMap_Find(_grpMng->m_groups, _grpName, (void**)_getGrp);
    if(stat == MAP_KEY_NOT_FOUND_ERROR)
    {
        return GRP_MNG_GRP_NOT_FOUND;        
    }
    return GRP_MNG_GRP_FOUND;
}

static GrpMngResults CheckParameters(GroupMng* _grpMng, char* _grpName, char* _context)
{
    if(_grpMng == NULL)
    {
        return GRP_MNG_UNINITIALIZED;
    }
    if(_grpName == NULL || _context == NULL)
    {
        return GRP_MNG_NULL_ITEM;
    }
    return GRP_MNG_SUCCESS;
}

static char* GetIpFromQueue(Queue* _queue)
{
    void* item;
    QueueResult rmv;
    rmv = QueueRemove(_queue, &item);
    if(rmv != QUEUE_SUCCESS)
    {
        return NULL;
    }
    return (char*)item;
}

static int SearchForGroup(void* _key1, void* _key2)
{ 
    const char* name; 
    name = (char*)(((HashMapData*)(((Node*)_key1)->m_data))->m_key); 

    if(strcmp((name), ((const char*)_key2)) == 0)
    {
        return 1;
    }
    return 0;
}

#define MAX_IP_LENGHTH 11
#define NUM_OF_IP_ADDRESSES 100

static void FillQueueWithIpAddresses(Queue* _que)
{
    char *IP, fullIP[MAX_IP_LENGHTH], BaseIP[] = "224.0.0.";
    int i;

    for(i = 0; i < NUM_OF_IP_ADDRESSES; ++i)
    {        
        snprintf(fullIP, MAX_IP_LENGHTH, "%s%d", BaseIP, i);
        IP = (char*)malloc(MAX_IP_LENGHTH + sizeof(char));
        strcpy(IP, fullIP);
        QueueInsert(_que, IP);        
    }
    free(IP);        
}
