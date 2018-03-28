#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PRINT_ERROR(x)      fprintf(stderr, "prog exit - error code %d\n!", x);
#define PRINT_MSG(x)        fprintf(stdout, "%s\n", x);

#define BUF_SIZE            200
#define QUEUE_LIM           5

/*
 Error Code:
    101:    port number missing or wrong format!
    102:    bind failed!
    103:    couldn't listen on the given socket!'
    104:    couldn't accept the new connection!
    104:    could close the socket!
 */

int main(int argc, char* argv[])
{
    if(argc < 2)
    {    
        PRINT_ERROR(101);
        exit(-1);
    }
    
    int newSockId;
    int sockId              = socket(AF_INET, SOCK_STREAM, 0);
    int sizeSockAddr        = sizeof(struct sockaddr); 
    int numBytesReceived    = 0;
    char msgBuffer[BUF_SIZE];
    
    struct in_addr sIPaddr;
    
    struct sockaddr_in sClientSockAddr;
    struct sockaddr_in sServSockAddr;
    
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
    if(listen(sockId, 1) < 0)
    {
        PRINT_ERROR(103);
        exit(-1);
    }
    printf("printing - %d\n", sockId);
    // start an infinite loop for accepting the connection
    while(1)
    {
        if((newSockId = accept(sockId, (struct sockaddr *) &sClientSockAddr, &sizeSockAddr) < 0))
        {
            PRINT_ERROR(104);
            exit(-1);            
        }
        
  printf("Connection From: %s:%d (%d)\n", 
         inet_ntoa(sClientSockAddr.sin_addr), //address as dotted quad
         ntohs(sClientSockAddr.sin_port),     //the port in host order
         newSockId);                        //the file descriptor number
        printf("sock id %d\n", newSockId);
        read(newSockId, msgBuffer, BUF_SIZE-1);
                    msgBuffer[numBytesReceived] = '\0';
            printf("her - %s\n", msgBuffer);
        
        while((numBytesReceived = read(newSockId, msgBuffer, BUF_SIZE-1)) >= 0)
        {
            
            msgBuffer[numBytesReceived] = '\0';
            printf("her - %s\n", msgBuffer);
            PRINT_MSG(msgBuffer);
        }
        
        if(close(newSockId) < 0)
        {
            PRINT_ERROR(105);
            exit(-1);  
        }
    }
    
}
