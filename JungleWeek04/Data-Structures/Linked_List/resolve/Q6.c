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



int moveMaxToFront(ListNode **ptrHead)
{
    /* add your code here */

	/*
	이미 최댓값이 맨 앞에 있는 경우 (예: 70, 30, 20) → 이동시킬 필요 없음, prev가 없는 경우 처리
	리스트가 노드 1개인 경우
	리스트가 비어있는 경우 (*ptrHead == NULL)
	최댓값이 중복으로 여러 개 있는 경우 (첫 번째 걸 옮기는지, 마지막 걸 옮기는지 — 문제에 명시 없으면 보통 처음 나온 것 기준)
	*/

    if(*ptrHead == NULL) {
        return 0;
    }
    
	ListNode *cur = *ptrHead;
    ListNode *prev = NULL;
    ListNode *maxPrev = NULL;
    ListNode *maxNode = *ptrHead;


    while (cur != NULL) {
        if (cur->item > maxNode->item) {
            maxNode = cur;
            maxPrev = prev;
        }

        prev = cur;
        cur = cur->next;
    }

    //최대값이 헤드일 때 
    if (maxPrev == NULL) {
        return maxNode->item;
    }

    maxPrev->next = maxNode->next;
    maxNode->next = *ptrHead;
    *ptrHead = maxNode;
   
    
    return maxNode->item;
}



