#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>


/* Spawn a child process running a new program. PROGRAM is the name
of the program to run; the path will be searched for this program.
ARG_LIST is a NULL-terminated list of character strings to be
passed as the program’s argument list. Returns the process ID of
the spawned process. */
int spawn (char* program, char** arg_list)
{
    pid_t child_pid;
    /* Duplicate this process. */
    child_pid = fork ();
    if (child_pid != 0)
    /* This is the parent process. */
        return child_pid;
    else {
        /* Now execute PROGRAM, searching for it in the path. */
        execvp (program, arg_list);
        /* The execvp function returns only if an error occurs. */
        fprintf (stderr, "an error occurred in execvp\n");
        abort ();
    }
}

sig_atomic_t child_exit_status;

void clean_up_child_process (int signal_number)
{
    // Clean up the child process.
    int status;
    wait (&status);
    // Store its exit status in a global variable.
    child_exit_status = status;
    return;
}


/*
int main ()
{
    pid_t child_pid;
    // Handle SIGCHLD by calling clean_up_child_process.
    struct sigaction sigchld_action;
    memset (&sigchld_action, 0, sizeof (sigchld_action));
    sigchld_action.sa_handler = &clean_up_child_process;
    sigaction (SIGCHLD, &sigchld_action, NULL);
    // Create a child process.
    child_pid = fork ();
    
    if (child_pid > 0) {
        // This is the parent process. Sleep for a minute.
        printf("parent process going to sleep for 60 sec %d\n", (int) getpid());
        printf("wait\n");
        sleep(30); // this is not working here ???????????????????????????
        printf("wait 1\n");
        sleep (5);
        printf("parent process woke-up and now exits\n");
    }
    else {
        //sleep(10);
        // This is the child process. Exit immediately.
        printf("child process exits! %d %d\n", (int) getpid(), (int) getppid());
        exit (0);
    }
    //printf("wait\n");
    //sleep(30);
    //printf("wait cleared\n");
    return 0;
}
*/

/*
int main ()
{
    int child_status;
    // The argument list to pass to the "ls" command.
    char* arg_list[] = {
        "ls",
        "-l",
        ".",
         //"cd", // argv[0], the name of the program.
         //"-",
         //"/",
        NULL // The argument list must end with a NULL.
    };
    //Spawn a child process running the "ls" command. Ignore the returned child process ID.
    //spawn ("ls", arg_list);
    // Wait for the child process to complete.    
    wait (&child_status);
    
    if (WIFEXITED (child_status))
        printf ("the child process exited normally, with exit code %d\n", WEXITSTATUS (child_status));
    else
        printf ("the child process exited abnormally %d\n", WTERMSIG(child_status));
    return 0;
}
*/

/*
int main ()
{
    // The argument list to pass to the "ls" command.
    char* arg_list[] = {
         "ls", // argv[0], the name of the program.
         "-l",
        NULL // The argument list must end with a NULL.
    };
    // Spawn a child process running the "ls" command. Ignore the returned child process ID.
    spawn ("ls", arg_list);
    printf ("done with main program\n");
    return 0;
}
*/

/*
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
int main ()
{
    pid_t child_pid;
    printf ("the main program process ID is %d\n", (int) getpid ());
    child_pid = fork ();
    
    //wait(); // use this to see how parent process is executed only after the child process is finished
    
    if (child_pid != 0) {
        printf ("this is the parent process, with id %d\n", (int) getpid ());
        printf ("the child’s process ID is %d\n", (int) child_pid);
    }
    else
        printf ("this is the child process, with id %d\n", (int) getpid ());
    return 0;
}
*/

/*
#include <stdlib.h>
int main ()
{
    int return_value;
    return_value = system ("ls -l /");
    return return_value;
}
*/
