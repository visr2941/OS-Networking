
CC = gcc
EXECUTABLES = testmymalloc testnocompact simpletest1 simpletest2

LIBDIR = 
LIBS = 
CFLAGS = -g -Wall

all: $(EXECUTABLES)

clean:
	rm -f core $(EXECUTABLES) a.out

simpletest1: simpletest1.c my_malloc.o my_malloc.h
	$(CC) $(CFLAGS) -o simpletest1 simpletest1.c my_malloc.o $(LIBS)

simpletest2: simpletest2.c my_malloc.o my_malloc.h
	$(CC) $(CFLAGS) -o simpletest2 simpletest2.c my_malloc.o $(LIBS)

testnocompact: testnocompact.c my_malloc_nocompact.o my_malloc.h
	$(CC) $(CFLAGS) -o testnocompact testnocompact.c my_malloc_nocompact.o $(LIBS)

testmymalloc: testmymalloc.c my_malloc.o my_malloc.h
	$(CC) $(CFLAGS) -o testmymalloc testmymalloc.c my_malloc.o $(LIBS)
