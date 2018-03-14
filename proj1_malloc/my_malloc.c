#include <stdio.h>
#include <stdlib.h>
#include "my_malloc.h"
#include <math.h>


/*********************************************************************************
 ***************** MACRO Definitions *********************************************
 ********************************************************************************/

#define SIZE_OVERHD      (sizeof(mallocOvrHd_t))
#define ALIGN8(size)     if(size%8) size = size - (size%8 - 8)

/*********************************************************************************
 ***************** Type Definitions **********************************************
 ********************************************************************************/

// double link list structure for the allocation overhead
typedef struct mallocOvrHd
{
    struct mallocOvrHd * next;
    struct mallocOvrHd * prev;
    int size;
    unsigned char * buffer;

} mallocOvrHd_t;

typedef long unsigned int int_t;
typedef mallocOvrHd_t * ptrMallocOvrHd_t;

/*********************************************************************************
 ***************** GLOBAL Variables **********************************************
 ********************************************************************************/

// defining a global variable to track the head pointer of the free memory list
mallocOvrHd_t * hdrFreeList = NULL;
// defining an array for malloc allocation
char arrayMalloc[MAX_MALLOC_SIZE];


/*********************************************************************************
 ***************** Function Definitions ******************************************
 ********************************************************************************/

void InitMyMalloc( void )
{
    hdrFreeList = (mallocOvrHd_t *) arrayMalloc;

    /* initializing the free memory list */
    hdrFreeList->next = NULL;
    hdrFreeList->prev = NULL;
    hdrFreeList->size = MAX_MALLOC_SIZE - SIZE_OVERHD;
    hdrFreeList->buffer = (unsigned char *) (arrayMalloc + SIZE_OVERHD);

}

/*************************************************************************
*  Description:
*            This function provides pointer to a valid memory of requested
*            size based on the FIRST FIT method.
*  Parameter:
*            int size - size of memory required in bytes
*  return:
*            void * - a pointer of memory location as a void *
**************************************************************************/

void* MyMalloc(int size)
{
    mallocOvrHd_t * ptrAllocMemBlock          = hdrFreeList;
    mallocOvrHd_t * ptrToFreeMemAfterSplit = NULL;

    // Make the size divisible by 8 (greater or equal to size)
    ALIGN8(size);

    // finding the first fit
    while(size > ptrAllocMemBlock->size)
    {
        ptrAllocMemBlock = ptrAllocMemBlock->next;
        if(!ptrAllocMemBlock)
            return NULL;
    };

    /* if we can't put overhead in remaining free space,
     * we will allocate that memory to the user (wasted)
     * otherwise split the free block into two memory
     * blocks, first one to be allocated and next as free.
     */
    if(ptrAllocMemBlock->size - size <= SIZE_OVERHD)
    {
        size = ptrAllocMemBlock->size;
        if(ptrAllocMemBlock->next)
            ptrToFreeMemAfterSplit = ptrAllocMemBlock->next;
	// if the memory to be allocated is the last block in the 
	// Free List, set the head pointer to the end of the Array
        else
        {
            ptrToFreeMemAfterSplit         = (ptrMallocOvrHd_t) (arrayMalloc+MAX_MALLOC_SIZE);
            ptrToFreeMemAfterSplit->size   = 0;
            ptrToFreeMemAfterSplit->buffer = NULL;
            ptrToFreeMemAfterSplit->prev   = NULL;
            ptrToFreeMemAfterSplit->next   = NULL;
        }
    }
    // split the block selected
    else
    {
        ptrToFreeMemAfterSplit         =  (ptrMallocOvrHd_t) ((char *) ptrAllocMemBlock + SIZE_OVERHD + size);
        ptrToFreeMemAfterSplit->size   =  ptrAllocMemBlock->size - size - SIZE_OVERHD;
        ptrToFreeMemAfterSplit->buffer =  (unsigned char *) ((char *) ptrToFreeMemAfterSplit + SIZE_OVERHD);
        ptrToFreeMemAfterSplit->next   =  ptrAllocMemBlock->next;
        ptrToFreeMemAfterSplit->prev   =  ptrAllocMemBlock->prev;

        ptrAllocMemBlock->next = ptrToFreeMemAfterSplit;
        ptrAllocMemBlock->size = size;
    }


    /* Removing the block - If pointer to the free block to be
     * allocated is same as pointer to the head of free list,
     * the head pointer should be moved to point the next free
     * block. Otherwise just remove the free block from the list
     * and connect the adjacent ones.
     */
    if(ptrAllocMemBlock == hdrFreeList)
        hdrFreeList =  ptrToFreeMemAfterSplit;
    else
        ptrAllocMemBlock->prev->next = ptrAllocMemBlock->next;


    return (void *)ptrAllocMemBlock->buffer;

}



/*************************************************************************
*  Description:
*            This function frees the memeory provided by the user and adds 
*            that up into the Free List of memory
*  Parameter:
*            void *buffer - pointer of the memory to be freed
*  return:
*            void - Nothing
**************************************************************************/
void MyFree(void *buffer)
{
    mallocOvrHd_t * ptrFreeMemBlock = (ptrMallocOvrHd_t) ((char *) buffer - SIZE_OVERHD);
    mallocOvrHd_t * ptrTempForLoop  = hdrFreeList;

    if(!buffer || (arrayMalloc > (char *) buffer) || (arrayMalloc + MAX_MALLOC_SIZE < (char *) buffer))
        return;

    /* add the memory to the free memory list and keep the head pointer
       of the list as the least address (sorted based on address) */
    if((int_t) ptrFreeMemBlock < (int_t) hdrFreeList)
    {
        ptrFreeMemBlock->prev = NULL;
	
        if(hdrFreeList->size!=0)
        {
            hdrFreeList->prev = ptrFreeMemBlock;
            ptrFreeMemBlock->next = hdrFreeList;
        }
	// if the freed memory is the only block in the Free List,
	// it's next pointer should be NULL.
        else
        {
            ptrFreeMemBlock->next = NULL;
        }

        hdrFreeList = ptrFreeMemBlock;
    }
    // if freeing the same pointer more than once, it should return (do nothing)
    else if ((int_t) ptrFreeMemBlock == (int_t) hdrFreeList)
        return;
    // the free memory block will not replace the head block of Free List, so 
    // search the list for the reight spot to add it
    else
    {
        while((int_t)ptrTempForLoop->next < (int_t) ptrFreeMemBlock)
        {
	    // breaking the loop because the free block fits at the
	    // end of the Free List of memory
            if(!ptrTempForLoop->next)
                break;
            ptrTempForLoop = ptrTempForLoop->next;
        }

        // if freeing the same pointer more than once, it should return (do nothing)
        if((int_t)ptrTempForLoop->next == (int_t) ptrFreeMemBlock )
            return;
	
	// adding the freed memory to the Free List of memory
        ptrFreeMemBlock->next = ptrTempForLoop->next;
        ptrFreeMemBlock->prev = ptrTempForLoop;
        ptrTempForLoop->next = ptrFreeMemBlock;
    }


    // merging memories if adjacent on the right
    if((int_t)(ptrFreeMemBlock->buffer + ptrFreeMemBlock->size) == (int_t)ptrFreeMemBlock->next)
    {
        ptrFreeMemBlock->size += ptrFreeMemBlock->next->size+SIZE_OVERHD;
        ptrFreeMemBlock->next = ptrFreeMemBlock->next->next;
    }
    // merging memories if adjacent on the left
    if (ptrFreeMemBlock->prev)
    {
        if((int_t)(ptrFreeMemBlock->prev->buffer + ptrFreeMemBlock->prev->size) == (int_t) ptrFreeMemBlock)
        {
            ptrFreeMemBlock->prev->size += ptrFreeMemBlock->size+SIZE_OVERHD;
            ptrFreeMemBlock->prev->next = ptrFreeMemBlock->next;
        }
    }
}


/*************************************************************************
*  Description:
*            This function prints the list of free memory
*  Parameter:
*            void  - Nothing
*  return:
*            void  - Nothing
**************************************************************************/
void PrintMyMallocFreeList( void )
{
    mallocOvrHd_t * ptrTempForLoop = hdrFreeList;

    if(hdrFreeList->size < 0)
    {
        fprintf(stderr,"No free heap memory - call to print free memory list failed\n");
		fflush(stderr);
		return;
    }
    printf("\nPRINTING FREE MEMORY LIST...\n");
    do
    {
        printf("block: %p\n", ptrTempForLoop);
        printf("    size: %d\n", ptrTempForLoop->size);
        printf("    next: %p\n", ptrTempForLoop->next);
        printf("    prev: %p\n", ptrTempForLoop->prev);
        printf("    buffer: %p\n", ptrTempForLoop->buffer);

        ptrTempForLoop = ptrTempForLoop->next;

    } while (ptrTempForLoop);
}
