#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PRINT_ERROR(x)      fprintf(stderr, "prog exit - error code %d\n!", x);
#define PRINT_MSG(x)        fprintf(stdout, "%s\n", x);

#define BUF_SIZE            200
#define QUEUE_LIM           5

/*
 Error Code:
    201:    host address/port number missing or wrong!
    202:    connection to the server failed!
    203:    data sending failed!'
 */

int main(int argc, char* argv[])
{
    if(argc < 3)
    {    
        PRINT_ERROR(201);
        exit(-1);
    }
    
    int sockId              = socket(AF_INET, SOCK_STREAM, 0);
    int sizeSockAddr        = sizeof(struct sockaddr); 
    int numBytesSent    = 0;
    char msgBuffer[BUF_SIZE];
            
    struct sockaddr_in sServSockAddr;
    
    // defining and filling up the socket address for the server 
    sServSockAddr.sin_port          = htons((unsigned short) atoi(argv[2]));
    sServSockAddr.sin_family        = AF_INET;
    sServSockAddr.sin_addr.s_addr   = htonl((unsigned long) atoi(argv[1]));
    
    fscanf(stdin, "%s", msgBuffer);
    
    // start connecting to the server
    if(connect(sockId, (struct sockaddr *) &sServSockAddr, sizeSockAddr) < 0)
    {
        PRINT_ERROR(202);
        exit(-1);
    }
    
    // start an infinite loop for sending data
    while(1)
    {   
        numBytesSent = fscanf(stdin, "%s", msgBuffer);
        printf("printing - %s %d %d\n", msgBuffer, sockId, numBytesSent);
        if(write(sockId, msgBuffer, 10) < 0)
        {
            PRINT_ERROR(203);
            exit(-1);
        }
        //while( (n = read(sockId, msgBuffer, BUF_SIZE)) > 0);
    }
    
}
