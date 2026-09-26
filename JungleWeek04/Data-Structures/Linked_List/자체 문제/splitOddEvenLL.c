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

/*
주어진 연결 리스트에서 홀수 값을 가진 노드와 짝수 값을 가진 노드를 
각각 기존 순서를 유지한 채 oddList와 evenList로 분리하는 함수
*/

void splitOddEvenLL(LinkedList *ll, LinkedList *oddList, LinkedList *evenList) {
    
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
            evenList->size++;
        } else {
            if (oddHead == NULL) {
                oddHead = cur;
                oddTail = cur;
            } else {
                oddTail->next = cur;
                oddTail = cur;
            }
            oddList->size++;
        }

        cur = cur->next;
    }

    
    if (evenHead != NULL && evenTail != NULL) {
        evenList->head = evenHead;
        evenTail->next = NULL;
    }

    if (oddHead != NULL && oddTail != NULL) {
        oddList->head = oddHead;
        oddTail->next = NULL;
    }


}