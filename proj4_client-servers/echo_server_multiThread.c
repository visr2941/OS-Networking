/* 
** This is an echo server - it will print the data as received from the client. This server can
* listen to the multiple client at once using multithreading. This server can accept request 
* from any IP address.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include <signal.h>

/*****************************************************************************************
 ************************** MACRO DEFINITIONS ********************************************
******************************************************************************************/

#define PRINT_ERROR(x)      fprintf(stderr, "prog exit - error code %d!\n", x);
#define PRINT_MSG(x)        fprintf(stdout, "%s", x);

#define BUF_SIZE            200
#define QUEUE_LIM           5
#define MAX_NUM_OF_SOCKET   100

/*
 Error Code:
    101:    port number missing or wrong format!
    102:    bind failed!
    103:    couldn't listen on the given socket!'
    104:    couldn't accept the new connection!
    105:    couldn't close the socket!
 */

/*****************************************************************************************
 ************************** GLOBAL VARIABLES *********************************************
******************************************************************************************/


/* defining sockId as a global static so that it can be closed
 * using signal handler when user enter Ctrl+C */
static int sockId;


/*****************************************************************************************
 ************************** SIGNAL HANDLER ***********************************************
******************************************************************************************/

/* signal handler defined for Ctrl+C to close the socket before
 * exit */
void TermHandler( int signalNumber )
{
    if(close(sockId) < 0)
    {
        PRINT_ERROR(105);
        exit(-1);
    }
    fprintf(stdout, "\nSERVER: Server Closed!\n");
    exit(0);
}


/*****************************************************************************************
 ************************** Functions/Procedure ******************************************
******************************************************************************************/

void * ProcessClient( void * arg )
{
    char msgBuffer[BUF_SIZE];
    int numBytesReceived  = 0;
    int newSockId = *((int *) arg);

    // reading the data from client and echoing 
    while((numBytesReceived = read(newSockId, msgBuffer, BUF_SIZE-1)) > 0)
    {
        msgBuffer[numBytesReceived] = '\0';
        PRINT_MSG(msgBuffer);
    }
    
    // close the socket if client closes the connection or read is unsuccessful
    if(close(newSockId) < 0)
    {
        PRINT_ERROR(105);
    }
            
    return NULL;;
}

/******************************************************************************************/

int main(int argc, char* argv[])
{
    pthread_t       tid;
    int newSockId;
    int sizeSockAddr        = sizeof(struct sockaddr);

    struct sockaddr_in sServSockAddr;
    struct sockaddr_in sClientSockAddr;
    
    // to run a server, port is required as input
    if(argc < 2)
    {    
        PRINT_ERROR(101);
        exit(-1);
    }
    
    // creatng a listening socket
    sockId = socket(AF_INET, SOCK_STREAM, 0);
    
    /* defining a signal handler for Ctrl+C to close
     the listening socket created before the program
     exits */
    struct sigaction sa;
    memset (&sa, 0, sizeof (sa));
    sa.sa_handler = &TermHandler; 
    sigaction(SIGINT, &sa, NULL);
    
    // defining and filling up the socket address for the server 
    sServSockAddr.sin_port          = htons((unsigned short) atoi(argv[1]));
    sServSockAddr.sin_family        = AF_INET;
    sServSockAddr.sin_addr.s_addr   = htonl(INADDR_ANY);
    
    // binding the socket with the socket address
    if(bind(sockId, (struct sockaddr *) &sServSockAddr, sizeSockAddr) < 0)
    {
        PRINT_ERROR(102);
        exit(-1);
    }
    
    
    // start listening (for single client)
    if(listen(sockId, QUEUE_LIM) < 0)
    {
        PRINT_ERROR(103);
        exit(-1);
    }
    
    fprintf(stdout, "SERVER: Waiting for new connection...\n");
    // accepting new connections
    while((newSockId = accept(sockId, (struct sockaddr *) &sClientSockAddr, &sizeSockAddr)) >= 0)
    {
        fprintf(stdout, "SERVER: Connection accepted from - %s:%d\n", inet_ntoa(sClientSockAddr.sin_addr), ntohs(sClientSockAddr.sin_port));
        int * ptrNewSockId       = (int *) malloc(sizeof(int));
        pthread_t * ptrThreadId  = (pthread_t *) malloc(sizeof(pthread_t));
        *ptrNewSockId = newSockId; 
        pthread_create( ptrThreadId, NULL, (void *) &ProcessClient, (void *) ptrNewSockId );
    }
        
    // main - problem in accepting new connection, so exiting ... 
    PRINT_ERROR(104);
    exit(-1); 
}


