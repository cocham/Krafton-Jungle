#include <stdio.h>
#include <stdlib.h>

//////////////////////////////////////////////////////////////////////////////////

typedef struct _listnode{
	int item;
	struct _listnode *next;
} ListNode;			// You should not change the definition of ListNode

typedef struct _linkedlist{
	int size;
	ListNode *head;
} LinkedList;	


void frontBackSplitLinkedList(LinkedList *ll, LinkedList *resultFrontList, LinkedList *resultBackList)
{
	/* add your code here */
    

    ListNode *cur = ll->head;
    ListNode *frontHead = NULL;
    ListNode *frontTail = NULL;
    ListNode *backHead = NULL;
    ListNode *backTail = NULL;

    int mid = (ll->size + 1) / 2; 
    int idx = 0;

    while (idx < mid) {
        if (frontHead == NULL) {
            frontHead = ll->head;
            frontTail = ll->head;
        } else {
            frontTail->next = cur;
            frontTail = cur;
        }

        cur = cur->next;
        idx++;
    }

    resultFrontList->head = frontHead;
    if (frontTail != NULL) {
        frontTail->next = NULL;
    }

    if (cur != NULL) {
        if (backHead == NULL) {
            backHead = cur;
            backTail = cur;
        } else {
            backTail->next = cur;
            backTail = cur;
        }

        cur = cur->next;
    }

    resultBackList->head = backHead;
    
    if (backTail != NULL) {
        backTail->next = NULL;
    }
}