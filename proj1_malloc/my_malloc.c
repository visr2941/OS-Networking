#include <stdio.h>
#include <stdlib.h>
#include "my_malloc.h"
#include <math.h>


/*********************************************************************************
 ***************** MACRO Definitions *********************************************
 ********************************************************************************/

#define SIZE_OVERHD     (sizeof(malloc_overhead_t))
#define ALIGN8(size)     if(size%8) size = size - (size%8 - 8) 


/*********************************************************************************
 ***************** Type Definitions **********************************************
 ********************************************************************************/

typedef long unsigned int ptr_to_int_t;

// double link list structure for the allocation overhead
typedef struct malloc_overhead 
{
    struct malloc_overhead * next;
    struct malloc_overhead * prev;
    int size;
    unsigned char * buffer;
    
} malloc_overhead_t;


/*********************************************************************************
 ***************** GLOBAL Variables **********************************************
 ********************************************************************************/

// defining a global variable to track the head pointer of the free memory list
malloc_overhead_t * hd_free_list = NULL;
// defining an array for malloc allocation
char arr_malloc[MAX_MALLOC_SIZE];


/*********************************************************************************
 ***************** Function Definitions ******************************************
 ********************************************************************************/

void InitMyMalloc( void )
{
    hd_free_list = (malloc_overhead_t *) arr_malloc;
    
    /* initializing the free memory list */
    hd_free_list->next = NULL;
    hd_free_list->prev = NULL;
    hd_free_list->size = MAX_MALLOC_SIZE - SIZE_OVERHD;
    hd_free_list->buffer = (unsigned char *) (arr_malloc + SIZE_OVERHD);
    
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
    malloc_overhead_t * ptr_to_return          = hd_free_list;
    malloc_overhead_t * ptrToFreeMemAfterSplit = NULL;
    
    // Make the size divisible by 8 (greater or equal to size)
    ALIGN8(size);
    
    // finding the first fit
    while(size > ptr_to_return->size)
    {
        ptr_to_return = ptr_to_return->next;
        if(!ptr_to_return)
            return NULL;
    };

    /* if we can't put overhead in remaining free space,
     * we will allocate that memory to the user (wasted)
     * otherwise split the free block into two memory
     * blocks, first one to be allocated and next as free.
     */
    if(ptr_to_return->size - size <= SIZE_OVERHD)
    {
        size = ptr_to_return->size;
        if(ptr_to_return->next)
            ptrToFreeMemAfterSplit = ptr_to_return->next;
        else
        {
            ptrToFreeMemAfterSplit         = (malloc_overhead_t *) (arr_malloc+MAX_MALLOC_SIZE);
            ptrToFreeMemAfterSplit->size   = 0;
            ptrToFreeMemAfterSplit->buffer = NULL;
            ptrToFreeMemAfterSplit->prev   = NULL;
            ptrToFreeMemAfterSplit->next   = NULL;
        }
    }
    else
    {
        ptrToFreeMemAfterSplit         =  (malloc_overhead_t *) ((char *) ptr_to_return + SIZE_OVERHD + size);
        ptrToFreeMemAfterSplit->size   =  ptr_to_return->size - size - SIZE_OVERHD;
        ptrToFreeMemAfterSplit->buffer =  (unsigned char *) ((char *) ptrToFreeMemAfterSplit + SIZE_OVERHD);
        ptrToFreeMemAfterSplit->next   =  ptr_to_return->next;
        ptrToFreeMemAfterSplit->prev   =  ptr_to_return->prev;
        
        ptr_to_return->next = ptrToFreeMemAfterSplit;
        ptr_to_return->size = size;
    }
        
    
    /* Removing the block - If pointer to the free block to be
     * allocated is same as pointer to the head of free list, 
     * the head pointer should be moved to point the next free
     * block. Otherwise just remove the free block from the list
     * and connect the adjacent ones.
     */
    if(ptr_to_return == hd_free_list)
        hd_free_list =  ptrToFreeMemAfterSplit;        
    else
        ptr_to_return->prev->next = ptr_to_return->next;

    
    return (void *)ptr_to_return->buffer;

}


void MyFree(void *buffer)
{    
    malloc_overhead_t * ptr_to_free_mem = (malloc_overhead_t *) ((char *) buffer - SIZE_OVERHD);
    malloc_overhead_t * ptr_temp = hd_free_list;
    
    if(!buffer || (arr_malloc > (char *) buffer) || (arr_malloc+MAX_MALLOC_SIZE < (char *) buffer))
        return;
    
    /* assign the memory to the free memory list and keep the head pointer
       of the list as the least address (sorted based on address) */   
    if((ptr_to_int_t) ptr_to_free_mem < (ptr_to_int_t) hd_free_list)
    {
        ptr_to_free_mem->next = hd_free_list;
        ptr_to_free_mem->prev = NULL;
        
        if(hd_free_list->size!=0)
            hd_free_list->prev = ptr_to_free_mem;
        
        hd_free_list = ptr_to_free_mem;
    }
    else
    {
        printf("ptr : %p %p\n", ptr_temp, ptr_to_free_mem);
        while((ptr_to_int_t)ptr_temp->next < (ptr_to_int_t) ptr_to_free_mem)
        {
            ptr_temp = ptr_temp->next;
        }
        printf("ptr : %p %p\n", ptr_temp, ptr_to_free_mem);
        ptr_to_free_mem->next = ptr_temp->next;
        ptr_to_free_mem->prev = ptr_temp;
        ptr_temp->next = ptr_to_free_mem;
    }
    
    
    // merging memories if adjacent on the right
    if((ptr_to_int_t)(ptr_to_free_mem->buffer + ptr_to_free_mem->size) == (ptr_to_int_t)ptr_to_free_mem->next)
    {
        ptr_to_free_mem->size += ptr_to_free_mem->next->size+SIZE_OVERHD;
        ptr_to_free_mem->next = ptr_to_free_mem->next->next;
    }
    // merging memories if adjacent on the left
    if (ptr_to_free_mem->prev)
    {
        if((ptr_to_int_t)(ptr_to_free_mem->prev->buffer + ptr_to_free_mem->prev->size) == (ptr_to_int_t) ptr_to_free_mem)
        {
            ptr_to_free_mem->prev->size += ptr_to_free_mem->size;
            ptr_to_free_mem->prev->next = ptr_to_free_mem->next;        
        }
    }
}

void PrintMyMallocFreeList()
{
    malloc_overhead_t * ptr_to_run = hd_free_list;
    
    if(hd_free_list->size < 0)
    {
        fprintf(stderr,"No free heap memory - call to print free memory list failed\n");
		fflush(stderr);
		return;
    }
    printf("\nPRINTING FREE MEMORY LIST...\n");
    do
    {
        printf("block: %p\n", ptr_to_run);
        printf("    size: %d\n", ptr_to_run->size);
        printf("    next: %p\n", ptr_to_run->next);
        printf("    prev: %p\n", ptr_to_run->prev);
        printf("    buffer: %p\n", ptr_to_run->buffer);
        
        ptr_to_run = ptr_to_run->next;
        
    } while (ptr_to_run);
}

