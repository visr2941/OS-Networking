#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>

/*********************************************************************************************
********************** Macro Definition ******************************************************
*********************************************************************************************/

#define MAX_NUM_CHAR_CMD    200
#define MAX_NUM_OF_ARG      20
#define PATH                {"/bin/", "/usr/bin/", NULL}
#define DEBUG               fprintf(stdout, "XXXXXX\n")
#define TRUE                1
#define FALSE               0
#define EXIT_SUCCESS        0
#define EXIT_FAILURE       -1

/*********************************************************************************************
********************** Global Variables ******************************************************
*********************************************************************************************/

const char * shell = "MyCLI>";
const char * path[] = PATH;



/*********************************************************************************************
********************** Main Routine **********************************************************
*********************************************************************************************/

int main()
{
    pid_t   shellProc, cmdProc;
    int     exitStatus;
    char    userCmd[MAX_NUM_CHAR_CMD];
    char*   userCmdToken;
    char*   argList[MAX_NUM_OF_ARG];
    char    func[MAX_NUM_CHAR_CMD] ;
    int     count = 0;
    
    // a new Shell process (CLI)
    shellProc = fork();
    
    // a shell process will always run until the user or the system kills it
    if(shellProc)
        waitpid(shellProc, &exitStatus, 0);
    // child process
    else
    {
        printf("\nEntering into a personalized shell... \n\n");
        
        // entering into infinite loop 
        while(TRUE)
        {
            printf("%s ", shell);
            
            // if user issues Ctrl+D commands
            if(fgets(userCmd, sizeof(userCmd), stdin) == NULL)
            {
                printf("\n");
                break;
            }
                
            // taking the command unput from the user
            userCmdToken = strtok(userCmd, "\n");
            if(userCmdToken == NULL)
                continue;
            
            // creating a new process to run the program
            cmdProc = fork();
            
            // extracting the main function name
            argList[count] = strtok(userCmdToken, " ");
            
            // parent process inside the shell
            if(cmdProc)
            {
                // cd and exit command need to be handled nside the parent process
                if(!strcmp(argList[count], "cd"))
                {
                    if(chdir(strtok(NULL, " ")))
                        fprintf(stderr, "cd: error occured\n");
                }
                else if(!strcmp(argList[count], "exit"))
                    exit(EXIT_SUCCESS);
                
                // waiting for the child process to exit
                waitpid(cmdProc, &exitStatus, 0);
                if(exitStatus)
                    fprintf(stderr, "%s: error occured!\n", argList[count]);
            }
            // child process to execute the command issued by the user
            else
            {
                // cd command is supposed to be not handled by the child process 
                if(!strcmp(argList[count], "cd"))
                    exit(EXIT_SUCCESS);
                    
                // searching the function in the PATH of the current environment (our shell)
                while(TRUE)
                {
                    if(path[count] == NULL)
                         exit(EXIT_FAILURE);
                    
                    strcpy(func, path[count]);
                    strcat(func, argList[0]);
                    
                    if(access(func, X_OK) == TRUE)
                        break;
                    else
                    {
                        count++;
                        continue;
                    }
                }

                // re-initializing the variable
                count = 0;
                
                // making the argument for execv from the user input
                while((argList[++count] = strtok(NULL, " "))!=NULL)
                {
                    // redirection operator - writing to an existing or a new file
                    if(!strcmp(argList[count], ">"))
                    {
                        // create if doesn't exist, clear the file if exist, give user rights
                        int fd = open(strtok(NULL, " "), O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
                        // cahnging output from stdout(1) to file(fd) 
                        dup2(fd, 1);
                       
                        argList[count] = NULL;
                        break;
                    }
                    // redirection operator >> - appending to an existing file
                    else if(!strcmp(argList[count], ">>"))
                    {
                        // open exiting fle for read and write, give user rights
                        int fd = open(strtok(NULL, " "), O_RDWR | O_APPEND, S_IRUSR | S_IWUSR);
                        // cahnging output from stdout(1) to file(fd) 
                        dup2(fd, 1);
                       
                        argList[count] = NULL;
                        break;
                    }                    
                }
                
                // to execute the program input by the user
                execv(func, argList);
                
                // execv doesn't return if successful, in case of failure it returns back
                exit(EXIT_FAILURE);
            }
        } // end of the mail while loop to run the CLI
    }
} // end of main
