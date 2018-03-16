

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define TRUE  1
#define FALSE 0 

int main(int argc, char *argv[]) 
{
    char ent = 0;
    int lcount = 0, wcount = 0,  acount = 0;
    bool insideWord = FALSE, newLine = TRUE;
    FILE * fp;

    fp = fopen(argv[argc-1], "r");
        
    if(!fp)
    {
        fprintf(stderr, "%s", "File not valid!\n");
        return -1;
    }
    
    while(1)
    {
        fscanf(fp,"%c", &ent);
        
        if(feof(fp))
            break;
        else if(ent == ' ' )
            insideWord = FALSE;
        else if(ent == '\n')
        {
            newLine = TRUE;
            insideWord = FALSE;
        }
        else
        {
            if(!insideWord)
                wcount++;
            if(newLine)
                lcount++;

            insideWord = TRUE;
            newLine = FALSE;
        }
        acount++;
    }    
    
    if(!strcmp(argv[1],"-l"))
        printf(" %d %s\n", lcount, argv[argc-1]);
    else    
        printf(" %d  %d %d %s\n", lcount, wcount, acount, argv[argc-1]);
 
    return 0;
}
