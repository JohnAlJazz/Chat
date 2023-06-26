#include <string.h> /*strcmp*/
#include <stdlib.h> /*malloc*/
#include <stddef.h> /*size_t*/

#include "groups.h"

#include "../DsLibrary/GenDLL.h"
#include "../DsLibrary/ListItr.h"
#include "../DsLibrary/GenEngineDLL.h"

#define NO_USER 0
#define EMPTY 1
#define EQUALS 0

static int SearchForMember(void* _key1, void* _key2);

/*************************************************/

struct Group
{
    char* m_grpName;
    char* m_grpIp;
    List* m_grpMembers;
};

/**************************************************/

Group* CreateGroup(char* _grpName, char* _name, char* _grpIp)
{
    Group* grp;
    List* list;
    ListResult stat;
    char* grpN, *grpMultiIp;

    if((grp = (Group*)malloc(sizeof(Group))) == NULL)
    {
        return NULL;
    }
    if((grpN = (char*)malloc(strlen(_grpName) + sizeof(char))) == NULL)
    {
        free(grp);
        return NULL;
    }
    if((grpMultiIp = (char*)malloc(strlen(_grpIp) + sizeof(char))) == NULL)
    {
        free(grp);
        free(grpN);
        return NULL;
    }
    list = ListCreate();
    if(list == NULL)
    {
        free(grp);
        free(grpN);
        free(grpMultiIp);
        return NULL;
    }
    strcpy(grpN, _grpName);
    strcpy(grpMultiIp, _grpIp);
    stat = ListPushHead(list, (void*)_name);
    if(stat != LIST_SUCCESS)
    {
        return NULL;
    }
    grp->m_grpIp = grpMultiIp;
    grp->m_grpName = grpN;
    grp->m_grpMembers = list;

    return grp;
}

GroupsResults AddMember(Group* _grp, char* _name)
{
    ListResult stat;
    void *begin, *end, *itr;
    List* list;

    if(_grp == NULL)
    {
        return GRP_UNINITIALIZED;
    }
    if(_name == NULL)
    {
        return GRP_NULL_ITEM;
    }
    begin = ListItrBegin(_grp->m_grpMembers);
    end = ListItrEnd(_grp->m_grpMembers);
    itr = ListItrFindFirst(begin, end, SearchForMember, (void*)_name);
    if(ListItrEquals(itr, end) == EQUALS)
    {
        stat = ListPushHead(_grp->m_grpMembers, _name);
        if(stat != LIST_SUCCESS)
        {
            return GRP_ADD_MEMBER_FAIL;
        }
        return GRP_ADD_MEMBER_SUCCESS;
    }
    return GRP_MEMBER_ALREADY_EXISTS;
}

GroupsResults RemoveMember(Group* _grp, char* _name)
{
    ListResult stat;
    void *begin, *end, *itr, *data;
    List* list;

    if(_grp == NULL)
    {
        return GRP_UNINITIALIZED;
    }
    if(_name == NULL)
    {
        return GRP_NULL_ITEM;
    }
    begin = ListItrBegin(_grp->m_grpMembers);
    end = ListItrEnd(_grp->m_grpMembers);
    itr = ListItrFindFirst(begin, end, SearchForMember, (void*)_name);
    if(ListItrEquals(itr, end) == EQUALS)
    {
        return GRP_MEMBER_NOT_FOUND;
    }
    data = ListItrRemove(itr);
    if(ListSize(_grp->m_grpMembers) == EMPTY)
    {
        return GRP_IS_EMPTY;
    }
    return GRP_REMOVAL_SUCCESS;
}

char* GroupIp(Group* _grp)
{
    if(_grp == NULL)
    {
        return NULL;
    }
    return _grp->m_grpIp;
}

void GroupDestroy(Group** _grp)
{
    if(_grp == NULL || *_grp == NULL)
    {
        return;
    }
    ListDestroy(&((*_grp)->m_grpMembers), NULL);
    free((*_grp)->m_grpName);
    free((*_grp)->m_grpIp);
    free(*_grp);
    *_grp = NULL;
}

/*****************************************************/

static int SearchForMember(void* _key1, void* _key2)
{ 
    const char* name; 
    name = ((Node*)_key1)->m_data; 

    if(strcmp((name), ((const char*)_key2)) == 0)
    {
        return 1;
    }
    return 0;
}