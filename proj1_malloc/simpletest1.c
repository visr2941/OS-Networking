#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "my_malloc.h"

int main(int argc, char *argv[])
{
	char *array;
    char * ar1;
    char * ar2;
    char * ar3;
    char * ar4;

	int i;

	/*
	 * must be first call in the program
	 */
	InitMyMalloc();

	array = MyMalloc(16);
   	PrintMyMallocFreeList();
    
    //ar1 = MyMalloc(16);
    //PrintMyMallocFreeList();
    
    ar1 = MyMalloc(32);
    PrintMyMallocFreeList();
    
    ar2 = MyMalloc(256);
    PrintMyMallocFreeList();
    
    ar3 = MyMalloc(128);
    PrintMyMallocFreeList();
    
    ar4 = MyMalloc(256);
    PrintMyMallocFreeList();
    
    MyFree(ar1);
    PrintMyMallocFreeList();
    
    MyFree(ar3);
    PrintMyMallocFreeList();
    
    //ar1 = MyMalloc(40);
    //PrintMyMallocFreeList();

	if(array == NULL)
	{
		fprintf(stderr,"call to array MyMalloc() failed\n");
		fflush(stderr);
		exit(1);
	}

	if(ar1 == NULL)
	{
		fprintf(stderr,"call to ar MyMalloc() failed\n");
		fflush(stderr);
		exit(1);
	}
	
	for(i=0; i < 9; i++)
	{
		array[i] = 'a' + i;
	}
	array[9] = 0;

	printf("here is my nifty new string: %s\n",array);

	MyFree(array);
    PrintMyMallocFreeList();
    
    //MyFree(ar3);
    //PrintMyMallocFreeList();



	return(0);
}

	

