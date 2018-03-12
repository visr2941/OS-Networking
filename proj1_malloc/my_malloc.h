#if !defined(MY_MALLOC_H)
#define MY_MALLOC_H

#define MAX_MALLOC_SIZE (1024*16)

void InitMyMalloc();
void *MyMalloc(int size);
void MyFree(void *buffer);

void PrintMyMallocFreeList();		/* optional for debugging */


#endif

