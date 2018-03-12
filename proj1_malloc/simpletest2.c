#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "my_malloc.h"

int main(int argc, char *argv[])
{
	char *a1;
	char *a2;
	char *a3;
	char *a4;

	/*
	 * must be first call in the program
	 */
	InitMyMalloc();

	a1 = (char *)MyMalloc(128);
	if(a1 == NULL)
	{
		fprintf(stderr,"call to MyMalloc(128) failed\n");
		fflush(stderr);
		exit(1);
	}

	printf("FREE LIST after malloc(128)\n");
	PrintMyMallocFreeList();

	a2 = (char *)MyMalloc(32);
	if(a2 == NULL)
	{
		fprintf(stderr,"first call to MyMalloc(32) failed\n");
		fflush(stderr);
		exit(1);
	}

	printf("FREE LIST after malloc(32)\n");
	PrintMyMallocFreeList();

	MyFree(a1);

	printf("FREE LIST after free of first 128 malloc()\n");
	PrintMyMallocFreeList();

	a3 = (char *)MyMalloc(104);
	if(a3 == NULL)
	{
		fprintf(stderr,"call to MyMalloc(104) failed\n");
		fflush(stderr);
		exit(1);
	}

	printf("FREE LIST after malloc(104)\n");
	PrintMyMallocFreeList();

	a4 = (char *)MyMalloc(8);
	if(a4 == NULL)
	{
		fprintf(stderr,"call to MyMalloc(8) failed\n");
		fflush(stderr);
		exit(1);
	}
	printf("FREE LIST after malloc(8)\n");
	PrintMyMallocFreeList();

	/*
	 * free it all -- notice that a1 is already free
	 */
	MyFree(a2);
	MyFree(a3);
	MyFree(a4);
	printf("FREE LIST after all free\n");
	PrintMyMallocFreeList();


	return(0);
}

	

