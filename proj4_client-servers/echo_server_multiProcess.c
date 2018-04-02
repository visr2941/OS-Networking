/* 
** This is an echo server - it will print the data as received from the client. This server 
* can listen to the multiple client at once using parent-child process. This server can accept 
* request from any IP address.
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
#include <fcntl.h>

/*****************************************************************************************
 ************************** MACRO DEFINITIONS ********************************************
******************************************************************************************/

#define PRINT_ERROR(x)      fprintf(stderr, "Parent Process: prog exit - error code %d!\n", x);
#define PRINT_MSG(x)        fprintf(stdout, "%d: %s", ntohs(sClientSockAddr.sin_port), x);

#define BUF_SIZE            200
#define QUEUE_LIM           5

/*
 Error Code:
    101:    port number missing or wrong format!
    102:    bind failed!
    103:    couldn't listen on the given socket!'
    104:    couldn't accept the new connection!
    105:    couldn't close the socket!
 */


/******************************************************************************************/


int main(int argc, char* argv[])
{
    int childPid;
    int newSockId;
    int sockId              = socket(AF_INET, SOCK_STREAM, 0);
    int sizeSockAddr        = sizeof(struct sockaddr); 
    int numBytesReceived    = 0;
    char msgBuffer[BUF_SIZE];
    
    struct in_addr sIPaddr;
    
    struct sockaddr_in sClientSockAddr;
    struct sockaddr_in sServSockAddr;
    
    if(argc < 2)
    {    
        PRINT_ERROR(101);
        exit(-1);
    }
    
    
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
    
    
    // start an infinite loop for accepting the connection
    while(1)
    {
        fprintf(stdout, "Parent Process: Waiting for new connection...\n");

        if(((newSockId = accept(sockId, (struct sockaddr *) &sClientSockAddr, &sizeSockAddr)) < 0))
        {
            PRINT_ERROR(104);
            exit(-1);            
        }
        else
        {
            fprintf(stdout, "\nParent Process: Connection accepted from - %s:%d\n", inet_ntoa(sClientSockAddr.sin_addr), ntohs(sClientSockAddr.sin_port));
        }
        
        /* create a child process who will communicate with the new client. the parent process
         * will go back and wait for new connection to accept*/
        if((childPid = fork()) == 0)
        {
            // closing the parent listening socket in child process
            if(close(sockId) < 0)
            {
                PRINT_ERROR(105);
                exit(-1);  
            }

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
                exit(-1);
            }
            
            fprintf(stdout, "Child Process: Port:%d closed.\n", ntohs(sClientSockAddr.sin_port));
            exit(-1);
        }
        
        // parent process (who is listening): closes the port assigned to child for communication
        if(close(newSockId) < 0)
        {
            PRINT_ERROR(105);
            exit(-1);  
        }        
    }
    
}
