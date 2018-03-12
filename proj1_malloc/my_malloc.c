#include <stdio.h>
#include <stdlib.h>
#include "my_malloc.h"
#include <math.h>


/*********************************************************************************
 ***************** MACRO Definitions *********************************************
 ********************************************************************************/
#define SIZE_OVERHD sizeof(malloc_overhead_t)


/*********************************************************************************
 ***************** Type Definitions **********************************************
 ********************************************************************************/
typedef long unsigned int ptr_to_int_t;

/* double link list structure for the allocation overhead */
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
/* defining a global variable to track the head pointer of the 
   free memory list */
malloc_overhead_t * hd_free_list = NULL;
char arr_malloc[MAX_MALLOC_SIZE];


/*********************************************************************************
 ***************** Function Definitions ******************************************
 ********************************************************************************/

void InitMyMalloc( void )
{
    hd_free_list = (malloc_overhead_t *) arr_malloc;
    
    //initializing the free memory list
    hd_free_list->next = NULL;
    hd_free_list->prev = NULL;
    hd_free_list->size = MAX_MALLOC_SIZE - SIZE_OVERHD;
    hd_free_list->buffer = (unsigned char *) (arr_malloc + SIZE_OVERHD);
    
}


void* MyMalloc(int size)
{
    malloc_overhead_t * ptr_to_return = hd_free_list;
    int temp = size%8;
    
    
    // for making allocation 8-bit aligned
    if(temp)
        size = size - temp + 8;
    
    // finding the first fit
    while(size > ptr_to_return->size)
    {
        ptr_to_return = ptr_to_return->next;
        if(!ptr_to_return)
            return NULL;
    };
    
    // if we can't put overhead in remaining free space,
    // we will allocate that memory to the user (wasted)
    if(ptr_to_return->size - size <= SIZE_OVERHD)
        size = ptr_to_return->size;
    
    
    // remove the block from the free memory list   
    if(ptr_to_return == hd_free_list)
    {
        if(!ptr_to_return->next)
        {
            hd_free_list = (malloc_overhead_t * )((char *)ptr_to_return + SIZE_OVERHD + size);
            hd_free_list->size = ptr_to_return->size - size - SIZE_OVERHD;
            hd_free_list->buffer = (unsigned char *)((char *)hd_free_list + SIZE_OVERHD);
        }
        else 
        {
            hd_free_list = ptr_to_return->next;
            hd_free_list->size = ptr_to_return->next->size;
            hd_free_list->buffer = ptr_to_return->next->buffer;
        }
            
    }
    else
    {
        (ptr_to_return->prev)->next = ptr_to_return->next;
        if(ptr_to_return->next)
            ptr_to_return->next->prev = ptr_to_return->prev;
    }

    
    ptr_to_return->size = size;
    
    return (void *)ptr_to_return->buffer;

}


void MyFree(void *buffer)
{    
    malloc_overhead_t * ptr_to_mem_to_add = (malloc_overhead_t *) ((char *)buffer - SIZE_OVERHD);
    malloc_overhead_t * ptr_temp = hd_free_list;
    
    if(!buffer)
        return;
    
    /* assign the memory to the free memory list and keep the head pointer
       of the list as the least address (sorted based on address)*/
    
    if((ptr_to_int_t) ptr_to_mem_to_add < (ptr_to_int_t) hd_free_list)
    {
        ptr_to_mem_to_add->next = hd_free_list;
        ptr_to_mem_to_add->prev = NULL;
        hd_free_list->prev = ptr_to_mem_to_add;
        
        hd_free_list = ptr_to_mem_to_add;
    }
    else
    {
        while((ptr_to_int_t)ptr_temp->next < (ptr_to_int_t) ptr_to_mem_to_add)
        {
            ptr_temp = ptr_temp->next;
            if(!ptr_temp)
                break;
        }
        
        ptr_to_mem_to_add->next = ptr_temp->next;
        ptr_to_mem_to_add->prev = ptr_temp;
        ptr_temp->next = ptr_to_mem_to_add;
    }
    
    
    // merging memories if adjacent (on the right)
    if((ptr_to_int_t)(ptr_to_mem_to_add->buffer + ptr_to_mem_to_add->size) == (ptr_to_int_t)ptr_to_mem_to_add->next)
    {
        ptr_to_mem_to_add->size += ptr_to_mem_to_add->next->size+SIZE_OVERHD;
        ptr_to_mem_to_add->next = ptr_to_mem_to_add->next->next;
    }
    // merging memories if adjacent (on the left)
    if (ptr_to_mem_to_add->prev)
    {
        if((ptr_to_int_t)(ptr_to_mem_to_add->prev->buffer + ptr_to_mem_to_add->prev->size) == (ptr_to_int_t) ptr_to_mem_to_add)
        {
            ptr_to_mem_to_add->prev->size += ptr_to_mem_to_add->size;
            ptr_to_mem_to_add->prev->next = ptr_to_mem_to_add->next;        
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

