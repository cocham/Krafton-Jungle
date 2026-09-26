#include <stdio.h>
#include <stdlib.h>


typedef struct _listnode
{
	int item;
	struct _listnode *next;
} ListNode;			

typedef struct _linkedlist
{
	int size;
	ListNode *head;
} LinkedList;	


void alternateMergeLinkedList(LinkedList *ll1, LinkedList *ll2)
{

	/*
		ll1의 노드 개수만큼 ll2에서 가져와서 중간에 삽입하기
		ll1 -> ll2 -> ll1 -> ll2 이런 식으로
			
	*/

    ListNode *ll1Cur = ll1->head;
    ListNode *ll2Cur = ll2->head;
    int size = ll1->size;

    while (ll1Cur != NULL && ll2Cur != NULL) {
        // ListNode *prevll1 = ll1Cur;
        // ListNode *prevll2 = ll2Cur;

        //원래 가리키던 노드를 알고 있어야 다음에 연결 가능
        ListNode *nextll1 = ll1Cur->next;
        ListNode *nextll2 = ll2Cur->next;

        ll1Cur->next = ll2Cur;
        ll2Cur->next = nextll1;

        ll1Cur = nextll1;
        ll2Cur = nextll2;
    }   
	
}