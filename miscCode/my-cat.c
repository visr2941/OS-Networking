
#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE_SIZE  150

int main(int argc, char** argv)
{
    FILE * fd;
    char buf[MAX_LINE_SIZE]; 
    char loop = 1;

    if(argc < 2)
    {
        fprintf(stderr, "my-cat: need filename as argument!\n");
        exit(-1);
    }
    
    while(loop < argc)
    {
        fd = fopen(argv[loop], "r");

        if(!fd)
        {
            fprintf(stderr, "my-cat: file doesn't exist or can't open the file\n");
            exit(-1);
        }
        else
        {
            while(fgets(buf, sizeof(buf), fd) != NULL)
                fprintf(stdout, "%s",  buf);
        }   
 
        loop++; 
        fclose(fd);
    }
    
    return 0;
}
