#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "my_malloc.h"

int main(int argc, char *argv[])
{
	int size;
	int lsize;
	int curr;
	void *ptr[10];
	int i;

	InitMyMalloc();

	/*
	 * try mallocing four pieces, each 1/4 of total size
	 */
	size = MAX_MALLOC_SIZE / 4;

	ptr[0] = MyMalloc(size);
	if(ptr[0] == NULL)
	{
		printf("malloc of ptr[0] failed for size %d\n",
				size);
		exit(1);
	}

	PrintMyMallocFreeList();
	printf("\n");

	ptr[1] = MyMalloc(size);
	if(ptr[1] == NULL)
	{
		printf("malloc of ptr[1] failed for size %d\n",
				size);
		exit(1);
	}

	PrintMyMallocFreeList();
	printf("\n");

	ptr[2] = MyMalloc(size);
	if(ptr[2] == NULL)
	{
		printf("malloc of ptr[2] failed for size %d\n",
				size);
		exit(1);
	}

	PrintMyMallocFreeList();
	printf("\n");

	/*
	 * this one should fail due to rounding
	 */
	ptr[3] = MyMalloc(size);
	if(ptr[3] == NULL)
	{
		printf("malloc of ptr[3] fails correctly for size %d\n",
				size);
	}

	PrintMyMallocFreeList();
	printf("\n");

	/*
	 * free the first block
	 */
	MyFree(ptr[0]);

	PrintMyMallocFreeList();
	printf("\n");

	/*
	 * free the third block
	 */
	MyFree(ptr[2]);

	PrintMyMallocFreeList();
	printf("\n");

	/*
	 * now free secoond block
	 */
	MyFree(ptr[1]);

	PrintMyMallocFreeList();
	printf("\n");

	/*
	 * re-malloc first pointer
\	 */
	ptr[0] = MyMalloc(size);
	if(ptr[0] == NULL)
	{
		printf("re-malloc of ptr[0] failed for size %d\n",
				size);
		exit(1);
	}
	PrintMyMallocFreeList();
	printf("\n");

	/*
	 * try splitting the second block
	 */
	ptr[1] = MyMalloc(size/2);
	if(ptr[1] == NULL)
	{
		printf("split second block ptr[1] failed for size %d\n",
				size/2);
		exit(1);
	}
	PrintMyMallocFreeList();
	printf("\n");

	/*
	 * free first block and split of second
	 */
	MyFree(ptr[0]);
	MyFree(ptr[1]);

	PrintMyMallocFreeList();
	printf("\n");

	/*
	 * try mallocing a little less to make sure no split occurs
	 * first block from previous print should not be split
	 */
	ptr[0] = MyMalloc(size-1);
	if(ptr[0] == NULL)
	{
		printf("slightly smaller malloc of ptr[0] failed for size %d\n",
				size);
		exit(1);
	}
	PrintMyMallocFreeList();
	printf("\n");

	/*
	 * free it and make sure it comes back as the correct size
	 */
	MyFree(ptr[0]);
	
	PrintMyMallocFreeList();
	printf("\n");

	/*
	 * okay -- malloc up all but the last available blocks
	 */
	ptr[0] = MyMalloc(size);
	if(ptr[0] == NULL)
	{
		printf("run-up malloc of ptr[0] failed for size %d\n",
				size);
		exit(1);
	}
	ptr[1] = MyMalloc(size/2);
	if(ptr[1] == NULL)
	{
		printf("run-up malloc of ptr[1] failed for size %d\n",
				size/2);
		exit(1);
	}
	ptr[2] = MyMalloc(size/2);
	if(ptr[2] == NULL)
	{
		printf("run-up malloc of ptr[2] failed for size %d\n",
				size/2);
		exit(1);
	}
	ptr[3] = MyMalloc(size/2);
	if(ptr[3] == NULL)
	{
		printf("run-up malloc of ptr[3] failed for size %d\n",
				size/2);
		exit(1);
	}

	/*
	 * this one should fail by a smidge
	 */
	ptr[4] = MyMalloc(size/2);
	if(ptr[4] == NULL)
	{
		printf("run-up malloc of ptr[4] failed for size %d\n",
				size/2);
	}

	PrintMyMallocFreeList();
	printf("\n");

	/*
	 * at this point, we should start walking back from size/2 until we
	 * find a size we can malloc
	 */
	lsize = size;
	while((ptr[4] = MyMalloc(lsize)) == NULL)
	{
		lsize -= 1;
		if(lsize == 0)
		{
			printf("run back to zero\n");
			exit(1);
		}
	}
	
	printf("first size found: %d\n",lsize);
	printf("\n");

	/*
	 * lsize is now small enough to malloc
	 */
	curr = 5;

	while(curr < 10)
	{
		ptr[curr] = MyMalloc(lsize);
		if(ptr[curr] == NULL)
		{
			break;
		}
		curr++;
	}

	if(curr == 10)
	{
		printf("total pointer count exceeded\n");
		exit(1);
	}

	PrintMyMallocFreeList();
	printf("\n");

	/*
	 * now walk back a second time to try and get last block --
	 * list should be empty after
	 */
	while((ptr[curr] = MyMalloc(lsize)) == NULL)
	{
		lsize -= 1;
		if(lsize == 0)
		{
			printf("2nd run back to zero\n");
			exit(1);
		}
	}
	PrintMyMallocFreeList();
	printf("\n");

	curr++;

	/*
	 * now free them all -- the list should look coherent
	 */
	for(i=0; i < curr; i++)
	{
		MyFree(ptr[i]);
	}

	PrintMyMallocFreeList();
	printf("\n");

	printf("made it -- passed test\n");


	exit(0);
}

	

