#include <stdio.h>
#include <stdlib.h>
#include "my_malloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "my_malloc.h"

/************************************************************************************************************
 *                                             Typedefs
 * **********************************************************************************************************/
typedef struct _ListNode {
    struct _ListNode* left;
    struct _ListNode* right;
    int size;
    void* buffer;
} ListNode_t;

/************************************************************************************************************
 *                                               MACROS
 * **********************************************************************************************************/
#define GET_BUFFER_PTR_FROM_HEAD(x) ((void*)((char*)x+sizeof(ListNode_t)))
#define GET_HEAD_PTR_FROM_BUFFER(x) ((ListNode_t*)((char*)x-sizeof(ListNode_t)))
#define HEADER_SIZE (sizeof(ListNode_t))

/************************************************************************************************************
 *                                      Static Functions Prototype
 * **********************************************************************************************************/
static int   _FixAlignment8Bytes(int sz);
static void  _ProcessNodeFound(ListNode_t* tmpHead, int size);
static void* _FirstFitAlgorithm(int size);
static void  _CoalesceFreeNodes(ListNode_t* node);

/************************************************************************************************************
 *                                          Global Variable
 * **********************************************************************************************************/
// a global ptr to free list
ListNode_t* head;
unsigned char BigBuffer[MAX_MALLOC_SIZE];

/************************************************************************************************************
 *                                      Global Function Definitions
 * **********************************************************************************************************/
void InitMyMalloc()
{
    head = (ListNode_t*)BigBuffer;
    head->left = NULL;
    head->right = NULL;
    head->buffer = GET_BUFFER_PTR_FROM_HEAD(head);
    head->size = MAX_MALLOC_SIZE - sizeof(ListNode_t);
}

void *MyMalloc(int size)
{
    size = _FixAlignment8Bytes(size);
    return _FirstFitAlgorithm(size);
}

void MyFree(void *buffer)
{
    // best to keep the free list sorted based on address - That is free blocks with 
    // lower addresses occur before free blocks with higher addresses on the list.

    if (buffer != NULL && ((long int)buffer & -248L))
    {
        ListNode_t* tmpHead = head;
        ListNode_t* node = GET_HEAD_PTR_FROM_BUFFER(buffer);

        // if all memory was allocated
        if (head == NULL)
        {
            head = node;
            head->right = NULL;
            head->left = NULL;
            return;
        }

        if ((long int)tmpHead < (long int)node)
        {
            // traverse through the free list and insert 
            // based on address
            while (tmpHead->right != NULL)
            {
                if ((long int)tmpHead->right < (long int)node)
                {
                    tmpHead = tmpHead->right;
                }
                else
                {
                    break;
                }
            }

            // insert the new free node
            node->right = tmpHead->right;
            tmpHead->right = node;
            node->left = tmpHead;
        }
        else
        {
            tmpHead->left = node;
            node->right = tmpHead;
            node->left = NULL;
            head = node;
        }

        // Coalesce if required
        _CoalesceFreeNodes(node);
    }
}

void PrintMyMallocFreeList(void)
{
    ListNode_t* tmpHead = head;
    while (tmpHead != NULL)
    {
        printf("BlockPtr %p, BlockPtr->left %p, BlockPtr->right %p, BlockPtr->size %d\n", \
                tmpHead, tmpHead->left, tmpHead->right, tmpHead->size);
        
        tmpHead = tmpHead->right;
    }
}

/************************************************************************************************************
 *                                      Static Functions Definitions
 * **********************************************************************************************************/
static void* _FirstFitAlgorithm(int size)
{
    ListNode_t* tmpHead = head;

    while (tmpHead->right != NULL && size > tmpHead->size)
    {
        tmpHead = tmpHead->right;
    }

    if (tmpHead != NULL)
    {
        // Free space found, update the free list and the 
        // newly allocated node header
        _ProcessNodeFound(tmpHead, size);
    }

    return (tmpHead != NULL ? tmpHead->buffer : NULL);
}

static int _FixAlignment8Bytes(int sz)
{
    return ((sz+7) >> 3) << 3;
}

static void _ProcessNodeFound(ListNode_t* tmpHead, int size)
{
    int newSize = tmpHead->size - size - HEADER_SIZE;
    bool negSize = false;
    tmpHead->size = size;

    ListNode_t* newNode = (ListNode_t*)((char*)GET_BUFFER_PTR_FROM_HEAD(tmpHead) + size);

    if (newSize < 0) {
        negSize = true;
    }

    if(tmpHead != head)
    {
        if (negSize) {
            tmpHead->left->right = tmpHead->right;
            if (tmpHead->right) {
                tmpHead->right->left = tmpHead->left;
            }
        } else {
            newNode->size = newSize;
            newNode->left = tmpHead->left;
            newNode->right = tmpHead->right;

            tmpHead->left->right = newNode;
            if (tmpHead->right) {
                tmpHead->right->left = newNode;
            }
        }
    }
    else
    {
        if (!negSize) {
            newNode->size = newSize;
            newNode->right = head->right;
            newNode->left = NULL;
        }
        head = negSize ? NULL : newNode;
    }

    // update allocated node header
    tmpHead->left = NULL;
    tmpHead->right = NULL;
    tmpHead->buffer = GET_BUFFER_PTR_FROM_HEAD(tmpHead);
}

static void  _CoalesceFreeNodes(ListNode_t* node)
{
    if (node != NULL)
    {
        ListNode_t* l = node->left, *r = node->right;

        // merge with left node
        if (l != NULL)
        {
            if ((ListNode_t*)((char*)(l->buffer) + l->size) == l->right)
            {
                l->size += node->size;
                l->right = node->right;   
            }

            node = l;
        }

        // merge with right node
        if ((ListNode_t*)((char*)(node->buffer) + node->size) == node->right)
        {
            node->size += r->size;
            node->right = r->right;  
        }
    }

    return;
}
