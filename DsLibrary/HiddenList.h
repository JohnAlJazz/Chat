#ifndef __HIDDEN_LIST_H__
#define __HIDDEN_LIST_H__

typedef struct Node Node;
typedef struct List List;

struct Node
{
	void* m_data;
	Node* m_next;
	Node* m_prev;
};

struct List
{
	Node m_head;
	Node m_tail;
};

#endif /*__HIDDEN_LIST_H__*/