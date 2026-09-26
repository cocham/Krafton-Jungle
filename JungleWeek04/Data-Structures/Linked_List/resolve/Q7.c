#include <stdio.h>
#include <stdlib.h>

//////////////////////////////////////////////////////////////////////////////////

typedef struct _listnode
{
	int item;
	struct _listnode *next;
} ListNode;			// You should not change the definition of ListNode

typedef struct _linkedlist
{
	int size;
	ListNode *head;
} LinkedList;	


void RecursiveReverse(ListNode **ptrHead)
{
    
    ListNode *first = *ptrHead;
    
    if (first == NULL || first->next == NULL) {
        return;
    }

    ListNode *rest = first->next;

    RecursiveReverse(&rest);
    first->next->next = first;
    first->next = NULL;
    *ptrHead = rest;
}
