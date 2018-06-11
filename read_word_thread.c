#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  cv[5];
int done[5] = {0};
int eof_done = 0;

FILE * fp = NULL;

void GetTheWord(char *str)
{
    int i = 0;

    while(1)
    {
        str[i] = fgetc(fp);
        if((str[i] != '\n') && (str[i] != ' ') && (str[i] != EOF))
            i++;
        else
            break;
    }

    str[i] = '\0';  
}

void PrintWord ( int i )
{
    char str[10];
    GetTheWord(str);
    if(feof(fp) == 0)
        printf("%s %d\n", str, i);
    else
    {
        for(int i = 0; i < 5; i++)
        {
            eof_done = 1;
            done[i] = 1;
            pthread_cond_signal(&cv[i]);
        }
    }
}

void * WaitForInvoke( void * arg )
{
    int i = *((int *) arg);
    int next = (i+1)%5;
    //sleep(1);    
    while(feof(fp) == 0)
    {   
        printf("thread %d\n", i);
        pthread_mutex_lock(&lock);
        while(done[i]==0)
        {
            printf("wait\n");
            pthread_cond_wait(&cv[i], &lock);
        }
        PrintWord(i);
        done[next] = 1;
        done[i] = 0;
        pthread_cond_signal(&cv[next]);
        while((eof_done == 0) && (feof(fp) == 0))
            pthread_cond_wait(&cv[i], &lock);
        pthread_mutex_unlock(&lock);
    }
    printf("-------------------------exiting %d\n", i);
}

int main()
{
    fp = fopen("t.txt", "r");
    pthread_t pid[5];

    for(int i = 0; i < 5; i++)
    {
        pthread_cond_init( &cv[i], NULL );
    }
    
    int temp[5];
    for(int i = 0; i < 5; i++)
    {
        temp[i] = i;
        pthread_create(&pid[i], NULL, WaitForInvoke, (void *)&temp[i]);
    }
    //sleep(1);
    pthread_mutex_lock(&lock);

    done[0] = 1;
    pthread_cond_signal(&cv[0]);
    pthread_mutex_unlock(&lock);    

    for(int i = 0; i < 5; i++)
        pthread_join(pid[i], NULL);

    fclose(fp);

}
