#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>


#define MAX_NUM_CHAR_CMD    200
#define PATH                {"/bin/", "/usr/bin/", "/home/vishal/Desktop/GitRepo/OS/proj2_MakingShell/", NULL}


const char * shell = "MyCLI>";
const char * path[] = PATH;

#define DEBUG fprintf(stdout, "before exit  %s\n", argList[count])


int main()
{
    pid_t shellProc, cmdProc;
    int exitStatus;
    char userCmd[MAX_NUM_CHAR_CMD];
    char * userCmdToken;
    char * argList[10];
    char func[100] ;
    int count = 0;
    
    shellProc = fork();
        
    if(shellProc)
        waitpid(shellProc, &exitStatus, 0);
    else
    {
        printf("\nEntering into a personalized shell... \n\n");
        while(1)
        {
            printf("%s ", shell);
            fgets(userCmd, sizeof(userCmd), stdin);
            
            userCmdToken = strtok(userCmd, "\n");
            if(userCmdToken == NULL)
                continue;
            
            cmdProc = fork();
            
            argList[count] = strtok(userCmdToken, " ");
            
            if(cmdProc)
            {
                if(!strcmp(argList[count], "cd"))
                {
                    if(chdir(strtok(NULL, " ")))
                        fprintf(stderr, "cd: directory not found\n");
                }
                else if(!strcmp(argList[count], "exit"))
                    exit(0);
                
                waitpid(cmdProc, &exitStatus, 0);
                if(exitStatus)
                    fprintf(stderr, "%s: command not found!\n", argList[count]);
            }
            else
            {
                if(!strcmp(argList[count], "cd"))
                    exit(0);
                    
                
                while(1)
                {
                    if(path[count] == NULL)
                         exit(-1);
                    
                    strcpy(func, path[count]);
                    strcat(func, argList[0]);
                    
                    if(access(func, X_OK) == 0)
                        break;
                    else
                    {
                        count++;
                        continue;
                    }
                }

                count = 0;
                
                while(argList[count]!=NULL)
                {
                    count++;
                    argList[count] = strtok(NULL, " ");

                    if(argList[count] != NULL)
                        if(!strcmp(argList[count], ">"))
                        {
                            int fd = open(strtok(NULL, " "), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
                            dup2(fd, 1);
                        
                            argList[count] = NULL;
                            break;
                        }
                        
                }
                
                execv(func, argList);

                exit(0);
            }
        }
    }
}
