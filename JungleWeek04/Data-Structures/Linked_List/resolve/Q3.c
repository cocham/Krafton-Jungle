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
} LinkedList;			// You should not change the definition of LinkedList


void moveOddItemsToBack(LinkedList *ll)
{
	
    ListNode *cur = ll->head;

    ListNode *oddHead = NULL;
    ListNode *oddTail = NULL;
    ListNode *evenHead = NULL;
    ListNode *evenTail = NULL;

    while (cur != NULL) {
        if (cur->item % 2 == 0) {
            if (evenHead == NULL) {
                evenHead = cur;
                evenTail = cur;
            } else {
                evenTail->next = cur;
                evenTail = cur;
            }
        } else {
            if (oddHead == NULL) {
                oddHead = cur;
                oddTail = cur;
            } else {
                oddTail->next = cur;
                oddTail = cur;
            }
        }

        cur = cur->next;
    }

	if (oddTail != NULL) {
        oddTail->next = NULL;
    }

    if (evenHead != NULL) {
        ll->head = evenHead;
        evenTail->next = oddHead;
    } else {
        ll->head = oddHead;
    }
}