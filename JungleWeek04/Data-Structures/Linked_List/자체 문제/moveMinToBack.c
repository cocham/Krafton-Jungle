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


int moveMinToBack(ListNode **ptrHead)
{
    /*
    고려해야 될 케이스
    - 최솟값이 맨 뒤에 있을 때 
    - 리스트가 비어 있을 때
    */

    if (*ptrHead == NULL) {
        return -1;
    }

    ListNode *cur = *ptrHead;
    ListNode *prev = NULL;

    ListNode *minPrev = NULL;
    ListNode *minNode = *ptrHead;

    while (cur != NULL) {
        if (cur->item < minNode->item) {
            minNode = cur;
            minPrev = prev;
        }

        prev = cur;
        cur = cur->next;
    }
    
    //최솟값이 맨 뒤에 있을 때
    if (minNode->next == NULL) {
        return minNode->item;
    }


    //최솟값이 맨 앞에 있을 때
    if (minPrev == NULL) {
        *ptrHead = minNode->next;
    } else {
        minPrev->next = minNode->next;
    }

    minNode->next = NULL;
    prev->next = minNode;

    return minNode->item;

}