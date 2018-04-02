#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>
#include <arpa/inet.h>

/*****************************************************************************************
 ************************** MACRO DEFINITIONS ********************************************
******************************************************************************************/

#define PRINT_ERROR(x)      fprintf(stderr, "prog exit - error code %d\n!", x);

#define BUF_SIZE            200
#define QUEUE_LIM           5

/*
 Error Code:
    201:    host address/port number missing or wrong!
    202:    connection to the server failed!
    203:    data sending failed!'
 */


/******************************************************************************************/

int main(int argc, char* argv[])
{
    int sockId              = socket(AF_INET, SOCK_STREAM, 0);
    int sizeSockAddr        = sizeof(struct sockaddr); 
    int numBytesSent    = 0;
    char msgBuffer[BUF_SIZE];
            
    struct sockaddr_in sServSockAddr;
    
    if(argc < 3)
    {    
        PRINT_ERROR(201);
        exit(-1);
    }
    
    // defining and filling up the socket address for the server 
    sServSockAddr.sin_port          = htons((unsigned short) atoi(argv[2]));
    sServSockAddr.sin_family        = AF_INET;
    if(inet_aton(argv[1], &sServSockAddr.sin_addr) == 0)
    {    
        PRINT_ERROR(201);
        exit(-1);
    }
    
    // start connecting to the server
    if(connect(sockId, (struct sockaddr *) &sServSockAddr, sizeSockAddr) < 0)
    {
        PRINT_ERROR(202);
        exit(-1);
    }
    
    // start an infinite loop for sending data
    while(1)
    {   
        if(fgets(msgBuffer, sizeof(msgBuffer), stdin) == NULL)
        {
            *msgBuffer = '\0';
        }
        
        // write data to server
        if(write(sockId, msgBuffer, strlen(msgBuffer)) < 0)
        {
            PRINT_ERROR(203);
            exit(-1);
        }
    }
    
}
