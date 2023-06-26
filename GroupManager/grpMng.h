#ifndef __GRP_MNG_H__
#define __GRP_MNG_H__

struct HashMapData
{
	void* m_key;
	void* m_value;
};

typedef struct GroupMng GroupMng;

typedef enum GrpMngResults
{
    GRP_MNG_SUCCESS, 
    GRP_MNG_UNINITIALIZED,
    GRP_MNG_NULL_ITEM,
    GRP_MNG_GRP_FOUND,
    GRP_MNG_GRP_NOT_FOUND, 
    GRP_MNG_GRP_ALREADY_EXISTS, 
    GRP_MNG_GROUP_IS_EMPTY, 
    GRP_MNG_PULLING_IP_FAIL, 
    GRP_MNG_CREATE_GRP_FAIL, 
    GRP_MNG_ADD_FAIL, 
    GRP_MNG_ADD_SUCCESS, 
    GRP_MNG_REMOVAL_SUCCESS 

}GrpMngResults;

/*descript - creates a struct that contains a hash map (to hold groups) and a queue (multicast adds)*/

GroupMng* GroupMngCreate();

/*descript - creates a group */

GrpMngResults CreateNewGroup(GroupMng* _grpMng, char* _grpName, char* _name);

GrpMngResults AddUserToGroup(GroupMng* _grpMng, char* _grpName, char* _name);

GrpMngResults RemoveUserFromGroup(GroupMng* _grpMng, char* _grpName, char* _name);

char* GetIp(GroupMng* _grpM, char* _grpName);

int GetPort();

void RemoveGroup(GroupMng* _grpM, char* _grpName);

void GroupMngDestroy(GroupMng** _grpMng);

#endif /*__GRP_MNG_H__*/