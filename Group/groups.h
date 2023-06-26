#ifndef __GROUPS_H__
#define __GROUPS_H__

#include "../DsLibrary/GenDLL.h"

/*#define EQUALS 0*/

typedef struct Group Group;

typedef enum GroupsResults
{
    GRP_UNINITIALIZED, 
    GRP_NULL_ITEM,
    GRP_ADD_MEMBER_FAIL,
    GRP_ADD_MEMBER_SUCCESS,
    GRP_MEMBER_ALREADY_EXISTS,
    GRP_MEMBER_NOT_FOUND,
    GRP_MEMBER_REMOVE_SUCCESS,
    GRP_IS_EMPTY,
    GRP_REMOVAL_SUCCESS   

}GroupsResults;

Group* CreateGroup(char* _grpName, char* _name, char* _grpIp);

GroupsResults FindMember(Group* _grp, char* _name);

GroupsResults AddMember(Group* _grp, char* _name);

GroupsResults RemoveMember(Group* _grp, char* _name);

char* GroupIp(Group* _grp);

void GroupDestroy(Group** _grp);

#endif /*__GROUPS_H__*/

