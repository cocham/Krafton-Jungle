/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 *
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "ateam",
    /* First member's full name */
    "Harry Bovik",
    /* First member's email address */
    "bovik@cs.cmu.edu",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~0x7)

#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

/* Basic constants and macros */
#define WSIZE      4 //word size
#define DSIZE      8 //double word size
#define CHUNKSIZE  (1<<12) //초기 힙 = 4096 bytes

#define MAX(x, y) ((x) > (y)? (x) : (y))

/* Pack a size and allocated bit into a word */ 
#define PACK(size, alloc) ((size) | (alloc)) // 1비트를 alloc용도로 사용

/* Read and write a word at address p */
// 특정 주소에 있는 4바이트를 읽고/쓰는 것
#define GET(p)       (*(unsigned int *)(p))
#define PUT(p, val)  (*(unsigned int *)(p) = (val))


/* Read the size and allocated fields from address p */
#define GET_SIZE(p)  (GET(p) & ~0x7) //get size
#define GET_ALLOC(p) (GET(p) & 0x1) //is alloacted


/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp) ((char *)(bp) - WSIZE) // bp - 4
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE) // bp + block_size - 8 => footer 시작 주소

/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE))) // 다음 블록의 payload 시작 주소
// GET_SIZE(((char *)(bp) - WSIZE))) => 현재 헤더에서 블록의 전체 사이즈를 읽음
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE))) // 이전 블록의 payload 시작점

/* 전역 변수 */
static char *heap_listp;   /* 프롤로그 payload를 가리킴 */

/* 도우미 함수 프로토타입 */
static void *extend_heap(size_t words);
static void *coalesce(void *bp);
//static void *find_fit(size_t asize);
static void place(void *bp, size_t asize);
static void *best_fit(size_t asize);

/*
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    /* Create the initial empty heap */
    if ((heap_listp = mem_sbrk(4*WSIZE)) == (void *)-1)
        return -1;

    PUT(heap_listp, 0);                          /* Alignment padding */
    PUT(heap_listp + (1*WSIZE), PACK(DSIZE, 1)); /* Prologue header */
    PUT(heap_listp + (2*WSIZE), PACK(DSIZE, 1)); /* Prologue footer */
    PUT(heap_listp + (3*WSIZE), PACK(0, 1));     /* Epilogue header */
    heap_listp += (2*WSIZE); // heap_listp -> prologue의 payload 위치

    /*
    초기상태

    padding
    ┌──────┐
    │  0   │
    └──────┘

    prologue
    ┌──────┬──────┐
    │ 8/1  │ 8/1  │
    │ hdr  │ ftr  │
    └──────┴──────┘

    epilogue
    ┌──────┐
    │ 0/1  │
    │ hdr  │
    └──────┘
    */


    /* Extend the empty heap with a free block of CHUNKSIZE bytes */
    if (extend_heap(CHUNKSIZE/WSIZE) == NULL)
        return -1;

    return 0;
}


/*
 * extend_heap - Extend heap with a free block and return its block pointer.
 */
static void *extend_heap(size_t words)

/*

[ prologue ][ 기존 블록들 ][ 새 free block ][ epilogue ]
즉 기존 epilogue는 없어지고, 그 자리에 새 free block의 header가 들어간다

*/
{
    char *bp;
    size_t size;

    /* Allocate an even number of words to maintain alignment */
    size = (words % 2) ? (words+1) * WSIZE : words * WSIZE; // 8 바이트 정렬 맞추기

    if ((long)(bp = mem_sbrk(size)) == -1)
        return NULL;

    /* Initialize free block header/footer and the epilogue header */
    PUT(HDRP(bp), PACK(size, 0));         /* Free block header */
    PUT(FTRP(bp), PACK(size, 0));         /* Free block footer */
    /*
    
    ┌────────┬──────────────────┬────────┐
    │ size/0 │      free        │ size/0 │
    │ header │                  │ footer │
    └────────┴──────────────────┴────────┘

    */


    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1)); /* New epilogue header */
    /*
        ┌────────┬──────────────┬────────┐
        │ size/0 │     free     │ size/0 │
        └────────┴──────────────┴────────┘
                                          ┌─────┐
                                          │ 0/1 │
                                          └─────┘
    */
    /* Coalesce if the previous block was free */
    // 새로 heap을 확장했는데 기존 마지막 블록도 free였을 수 있기 때문
    // [ allocated ][ free + free ][ epilogue ]
    return coalesce(bp);
}



/*
 * mm_free - Freeing a block.
 */
void mm_free(void *bp)
{
    size_t size = GET_SIZE(HDRP(bp));

    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    coalesce(bp);
}


static void *coalesce(void *bp)
{

    /*
        previous       current        next
            ↓             ↓             ↓
         [  ?  ]       [ free ]       [ ? ]
    
    */
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp))); //이전 블록의 footer에서 alloc 비트를 읽음
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp))); 
    size_t size = GET_SIZE(HDRP(bp));

    if (prev_alloc && next_alloc) {            /* Case 1 */
        return bp;
    }

    else if (prev_alloc && !next_alloc) {     /* Case 2 */
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size,0));  //왜 FTRP(bp)가 next의 footer를 가리키는가? 합쳐진 블록의 끝이 원래 next block의 끝이기 때문.
    }

    else if (!prev_alloc && next_alloc) {     /* Case 3 */
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT(FTRP(bp), PACK(size, 0));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp); // 합쳐진 블록의 시작점은 previous block의 bp
    }

    else {                                    /* Case 4 */
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) +
                GET_SIZE(FTRP(NEXT_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }

    return bp; // 합쳐진 블록의 payload 시작 주소 반환
}



// 힙을 앞에서부터 훑어서, 비어 있고 크기가 asize 이상인 첫 블록의 bp를 돌려준다. 없으면 NULL.
// static void *find_fit(size_t asize)
// {
//     void *bp;

//     for (bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)) {
//         if (!GET_ALLOC(HDRP(bp)) && (asize <= GET_SIZE(HDRP(bp)))) { //할당이 안 돼있고 사이즈가 맞으면
//             return bp;
//         }
//     }

//     return NULL;
// }


static void *best_fit(size_t asize) {
    void *bp;

    void *best_bp = NULL;               // 지금까지 발견한 가장 적절한 free block 주소 저장
    size_t best_size = (size_t) - 1;    // 큰 값으로 초기화


    for (bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)) {
        size_t curSize = GET_SIZE(HDRP(bp));

        if (!GET_ALLOC(HDRP(bp)) && (asize <= curSize)) { //할당이 안 돼있고 들어갈 수 있다면
            if (asize == curSize) {
                return bp;
            }
            if (curSize < best_size) {
                best_bp = bp;
                best_size = curSize;
            }
        }
    }

    return best_bp;

}



static void place(void *bp, size_t asize)
{
    size_t csize = GET_SIZE(HDRP(bp));

    if ((csize - asize) >= (2*DSIZE)) { //최소 블록 크기 = 8 * 2 = 16byte
        PUT(HDRP(bp), PACK(asize, 1)); // 헤더 푸터에 asize | 할당
        PUT(FTRP(bp), PACK(asize, 1));

        bp = NEXT_BLKP(bp); // 바뀐 크기만큼 이동

        PUT(HDRP(bp), PACK(csize - asize, 0)); // 남는 크기 | 가용
        PUT(FTRP(bp), PACK(csize - asize, 0));
    } else {
        PUT(HDRP(bp), PACK(csize, 1));
        PUT(FTRP(bp), PACK(csize, 1));

        /*
        남는 공간
            │
            ├── 충분히 큼 → 새로운 free block으로 만든다
            │              (분할)
            │
            └── 너무 작음 → 기존 allocated block에 포함시킨다
                            (통째로 할당)
        */
    }


}



/*
 * mm_malloc - Allocate a block with at least size bytes of payload.
 */
void *mm_malloc(size_t size)
{
    size_t asize;      /* Adjusted block size */
    size_t extendsize; /* Amount to extend heap if no fit */
    char *bp;

    /* Ignore spurious requests */
    if (size == 0)
        return NULL;

    /* Adjust block size to include overhead and alignment reqs. */
    if (size <= DSIZE)
        asize = 2*DSIZE;
    else
        asize = DSIZE * ((size + (DSIZE) + (DSIZE-1)) / DSIZE); // header/footer overhead를 더하고, 8바이트 단위로 올림

    /* Search the free list for a fit */
    if ((bp = best_fit(asize)) != NULL) {
        place(bp, asize);
        return bp;
    }

    /* No fit found. Get more memory and place the block */
    extendsize = MAX(asize, CHUNKSIZE);
    if ((bp = extend_heap(extendsize/WSIZE)) == NULL)
        return NULL;

    place(bp, asize);
    return bp; //payload 시작 주소  
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    void *oldptr = ptr;
    void *newptr;
    size_t copySize;

    newptr = mm_malloc(size);
    if (newptr == NULL)
        return NULL;
    copySize = *(size_t *)((char *)oldptr - SIZE_T_SIZE);
    if (size < copySize)
        copySize = size;
    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);
    return newptr;
}