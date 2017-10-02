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
team_t team = 
{
    /* Team name */
    "515030910477",
    /* First member's full name */
    "Cheng Yu Chen",
    /* First member's email address */
    "rudeigerc@sjtu.edu.cn",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)

#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

#define WSIZE 4     /* Word and header/footer size (bytes) */
#define DSIZE 8     /* Double word size (bytes) */
#define CHUNKSIZE (1<<12)   /* Extend heap by this amount (bytes) */

#define MAX(x, y) ((x) > (y)? (x) : (y))

/* Pack a size and allocated bit into a word */
#define PACK(size, alloc) ((size) | (alloc))

/* Read and write a word at address p */
#define GET(p) (*(unsigned int *)(p))
#define PUT(p, val) (*(unsigned int *)(p) = (val))

/* Read the size and allocated fields form address p */
#define GET_SIZE(p) (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)

/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp) ((char *)(bp) - WSIZE)
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

/* Given block ptr bp, compute address of its predecessor and successor */
#define PREDRP(bp) ((char *)(bp))
#define SUCCRP(bp) ((char *)(bp) + WSIZE)

/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLKP(bp) ((char*)(bp) + GET_SIZE(((char*)(bp) - WSIZE)))
#define PREV_BLKP(bp) ((char*)(bp) - GET_SIZE(((char*)(bp) - DSIZE)))

static char *heap_listp;
static char *block;
static void *coalesce(void *bp);
static void *find_fit(size_t asize);
static void place(void *bp, size_t asize);
static void *extend_heap(size_t words);
static void add_bp(void *bp);
static void remove_bp(void *bp);
static int mm_check(void);


/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    /* Create the initial empty heap */
    if ((heap_listp = mem_sbrk(6*WSIZE)) == (void *)-1) return -1;
    PUT(heap_listp, 0);                             /* Alignment padding */
    PUT(heap_listp + (1*WSIZE), 0);                 /* Predecessor */
    PUT(heap_listp + (2*WSIZE), 0);                 /* Sucessor */
    PUT(heap_listp + (3*WSIZE), PACK(DSIZE, 1));    /* Prologue header */
    PUT(heap_listp + (4*WSIZE), PACK(DSIZE, 1));    /* Prologue footer */
    PUT(heap_listp + (5*WSIZE), PACK(0, 1));        /* Epilogue header */
    
    block = heap_listp + (1*WSIZE);
    heap_listp += (4*WSIZE);

    /* Extend the empty heap with a free block of CHUNKSIZE bytes */
    if (extend_heap(CHUNKSIZE/WSIZE) == NULL) return -1;
    return 0;
}

static void *extend_heap(size_t words)
{
    char *bp;
    size_t size;

    /* Allocate an even number of words to maintain alignment */
    size = (words % 2) ? (words + 1) * DSIZE : words * DSIZE;
    if ((long)(bp = mem_sbrk(size)) == -1) return NULL;

    /* Initialize free block header/footer and the epilogue header */
    PUT(HDRP(bp), PACK(size, 0));   /* Free block header */
    PUT(FTRP(bp), PACK(size, 0));   /* Free block footer */
    PUT(PREDRP(bp), 0);             /* Free block predecessor */
    PUT(SUCCRP(bp), 0);             /* Free block successor */
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));   /* New epilogue header */

    /* Coalesce if the previous block was free */
    return coalesce(bp);
}


/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    size_t asize;       /* Adjusted block size */
    size_t extendsize;  /* Amount to extend heap if no fit */
    char *bp;

    if (size == 0) return NULL; /* Ignore spurious requests */

    /* Adjust block size to include overhead and alignment reqs */
    if (size <= DSIZE) asize = 2 * DSIZE;
    else asize = DSIZE * ((size + (DSIZE) + (DSIZE-1)) / DSIZE);

    /* Search the free list for a fit */
    if ((bp = find_fit(asize)) != NULL)
    {
        place(bp, asize);
        return bp;
    }

    /* No fit found. Get more memory and place the block */
    extendsize = MAX(asize, CHUNKSIZE);
    if ((bp = extend_heap(extendsize/WSIZE)) == NULL) return NULL;
    place(bp, asize);
    return bp;
}

static void *find_fit(size_t asize)
{
    /*
    void *bp;
    for (bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp))
    {
        if (!GET_ALLOC(HDRP(bp)) && (asize <= GET_SIZE(HDRP(bp)))) return bp;
    }
    */
    char *bp;
    bp = (char *)GET(block);
    while (bp != NULL)
    {
        if (GET_SIZE(HDRP(bp)) >= asize) return bp;
        bp = (char *)GET(SUCCRP(bp));
    }
    
    return NULL;
}

static void place(void *bp, size_t asize)
{
    size_t csize = GET_SIZE(HDRP(bp));
    remove_bp(bp);
    if ((csize - asize) >= (2*DSIZE))
    {
        PUT(HDRP(bp), PACK(asize, 1));
        PUT(FTRP(bp), PACK(asize, 1));
        bp = NEXT_BLKP(bp);
        PUT(HDRP(bp), PACK(csize - asize, 0));
        PUT(FTRP(bp), PACK(csize - asize, 0));

        PUT(PREDRP(bp), 0);
        PUT(SUCCRP(bp), 0);
        coalesce(bp);
    }

    else 
    {
        PUT(HDRP(bp), PACK(csize, 1));
        PUT(FTRP(bp), PACK(csize, 1));   
    }
}
/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
    size_t size = GET_SIZE(HDRP(ptr));

    PUT(HDRP(ptr), PACK(size, 0));
    PUT(FTRP(ptr), PACK(size, 0));
    PUT(PREDRP(ptr), 0);
    PUT(SUCCRP(ptr), 0);
    coalesce(ptr);
}

static void *coalesce(void *bp)
{
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

    if (prev_alloc && next_alloc) {} //return bp;   /* Case 1 */

    else if (prev_alloc && !next_alloc)             /* Case 2 */
    {
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        remove_bp(NEXT_BLKP(bp));
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));
    }
    
    else if (!prev_alloc && next_alloc)             /* Case 3 */
    {
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        remove_bp(PREV_BLKP(bp));
        PUT(FTRP(bp), PACK(size, 0));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }
    
    else                                            /* Case 4 */
    {
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(FTRP(NEXT_BLKP(bp)));
        remove_bp(PREV_BLKP(bp));
        remove_bp(NEXT_BLKP(bp));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }

    add_bp(bp);
    return bp;
}

static void add_bp(void *bp)
{
    void *succ = (void *)GET(block);
    if (succ != NULL) PUT(PREDRP(succ), bp);
    PUT(SUCCRP(bp), succ);
    PUT(block, bp);
}

static void remove_bp(void *bp)
{
    void *pred = (void *)GET(PREDRP(bp));
    void *succ = (void *)GET(SUCCRP(bp));
    if (pred == NULL)
    {
        if (succ != NULL) PUT(PREDRP(succ), 0);
        PUT(block, succ);
    }
    else
    {
        if (succ != NULL) PUT(PREDRP(succ), pred);
        PUT(SUCCRP(pred), succ);
    }
    PUT(PREDRP(bp), 0);
    PUT(SUCCRP(bp), 0);
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    void *oldptr = ptr;
    void *newptr;
    size_t copySize;
    
    if (size == 0)
    {
        mm_free(oldptr);
        return NULL;
    }

    if (oldptr == NULL) return mm_malloc(size);

    newptr = mm_malloc(size);
    if (newptr == NULL) return NULL;

    copySize = GET_SIZE(HDRP(oldptr));
    if (size < copySize) copySize = size;
    memcpy(newptr, oldptr, size);
    mm_free(oldptr);
    return newptr;
}

/*
 * mm_check - check the status of heap consistency
 */
static int mm_check(void)
{
    void *check = (void *)GET(block);
    while (check)
    {
        printf("From 0x%x(size:%d, alloc:%d) to 0x%x(size:%d, alloc:%d).", 
        HDRP(check),GET_SIZE(HDRP(check)),GET_ALLOC(HDRP(check)),
        FTRP(check),GET_SIZE(HDRP(check)),GET_ALLOC(HDRP(check)));

        /* Check whether the size is available. */
        if (GET_SIZE(HDRP(check)) < 0)
        {
            printf("Block size unavailable.\n");
            return 0;
        }
    }
    printf("\n");
    return 1;
}