#include <stdio.h>
#include <stdlib.h>


typedef struct _listnode{
	int item;
	struct _listnode *next;
} ListNode;			

typedef struct _linkedlist{
	int size;
	ListNode *head;
} LinkedList;	


int insertSortedLL(LinkedList *ll, int item)
{
	/* add your code here */
    int idx = 0;
    ListNode *prev = NULL;
    ListNode *cur = ll->head;

    while (cur != NULL) {
        if (cur->item > item) {
            break;
        }

        if (cur->item == item) {
            return -1;
        }

        prev = cur;
        cur = cur->next;
        idx++;
    }
    
    ListNode *newNode = malloc(sizeof(ListNode));
    newNode->item = item;
    //prev->next = newNode;
    newNode->next = cur;

    //리스트가 비었을 때 + 맨 앞에 삽입할 때
    if (prev == NULL) {
        ll->head = newNode;
    } else {
        prev->next = newNode;
    }

    //맨 앞에 삽입할 때
    // if (newNode->item > ll->head->item) {
    //     newNode->next = ll->head;
    //     ll->head = newNode;
    // }

    //맨 뒤에 삽입할 때
    // if (cur == NULL) {
    //     prev->next = newNode;
    // }

    ll->size++;
    return idx;
}