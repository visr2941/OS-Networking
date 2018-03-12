#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h> //for struct in_addr
#include <arpa/inet.h>  //for inet_ntoa()
#include <netdb.h>
#include <unistd.h>


#define BUF_SIZE 4096

int main(){

  char hostname[]="127.0.0.1";   //localhost ip address to bind to
  short port=1845;               //the port we are to bind to


  struct sockaddr_in saddr_in;  //socket interent address of server
  struct sockaddr_in client_saddr_in;  //socket interent address of client

  socklen_t saddr_len = sizeof(struct sockaddr_in); //length of address

  int server_sock, client_sock;         //socket file descriptor


  char response[BUF_SIZE];           //what to send to the client
  int n;                             //length measure

  //set up the address information
  saddr_in.sin_family = AF_INET;
  inet_aton(hostname, &saddr_in.sin_addr);
  saddr_in.sin_port = htons(port);

  //open a socket
  if( (server_sock = socket(AF_INET, SOCK_STREAM, 0))  < 0){
    perror("socket");
    exit(1);
  }

  //bind the socket
  if(bind(server_sock, (struct sockaddr *) &saddr_in, saddr_len) < 0){
    perror("bind");
    exit(1);
  }

  //ready to listen, queue up to 5 pending connectinos
  if(listen(server_sock, 5)  < 0){
    perror("listen");
    exit(1);
  }


  saddr_len = sizeof(struct sockaddr_in); //length of address

  printf("Listening On: %s:%d\n", inet_ntoa(saddr_in.sin_addr), ntohs(saddr_in.sin_port));

  //accept incoming connections
  if((client_sock = accept(server_sock, (struct sockaddr *) &client_saddr_in, &saddr_len)) < 0){
    perror("accept");
    exit(1);
  }


  printf("Connection From: %s:%d (%d)\n", 
         inet_ntoa(client_saddr_in.sin_addr), //address as dotted quad
         ntohs(client_saddr_in.sin_port),     //the port in host order
         client_sock);                        //the file descriptor number

  //read from client
  if((n = read(client_sock,response, BUF_SIZE-1)) < 0){
    perror("read");
    exit(1);
  }
  response[n] = '\0'; //NULL terminate string

  printf("Read from client: %s", response);

  //construct response
  snprintf(response, BUF_SIZE, "Hello %s:%d \nGo Navy! Beat Army\n", 
           inet_ntoa(client_saddr_in.sin_addr),    //address as dotted quad
           ntohs(client_saddr_in.sin_port));       //the port in host order

  printf("Sending: %s",response);

  //send response
  if(write(client_sock, response, strlen(response)) < 0){
    perror("write");
    exit(1);
  }

  printf("Closing socket\n\n");

  //close client_sock
  close(client_sock);

  //close the socket
  close(server_sock);

  return 0; //success
}

/*
int main(){

  char hostname[]="google.com";    //the hostname we are looking up
  short port=80;                 //the port we are connecting on

  struct addrinfo *result;       //to store results
  struct addrinfo hints;         //to indicate information we want

  struct sockaddr_in *saddr_in;  //socket interent address

  int s,n;                       //for error checking

  int sock;                      //socket file descriptor

  char request[]="GET /index.html\n\r"; //the GET request

  char response[4096];           //read in 4096 byte chunks


  //setup our hints
  memset(&hints,0,sizeof(struct addrinfo));  //zero out hints
  hints.ai_family = AF_INET; //we only want IPv4 addresses

  //Convert the hostname to an address
  if( (s = getaddrinfo(hostname, NULL, &hints, &result)) != 0){
    fprintf(stderr, "getaddrinfo: %s\n",gai_strerror(s));
    exit(1);
  }

  //convert generic socket address to inet socket address
  saddr_in = (struct sockaddr_in *) result->ai_addr;

  //set the port in network byte order
  saddr_in->sin_port = htons(port);

  //open a socket
  if( (sock = socket(AF_INET, SOCK_STREAM, 0))  < 0){
    perror("socket");
    exit(1);
  }

  //connect to the server
  if(connect(sock, (struct sockaddr *) saddr_in, sizeof(*saddr_in)) < 0){
    perror("connect");
    exit(1);
  }

  //send the request
  if(write(sock,request,strlen(request)) < 0){
    perror("send");
  }

  //read the response until EOF
  while( (n = read(sock, response, 4096)) > 0){

    //write response to stdout
    if(write(1, response, n) < 0){
      perror("write");
      exit(1);
    }
  }

  if (n<0){
    perror("read");
  }

  //close the socket
  close(sock);

  return 0; //success
}
*/

/*
int main(){

  char hostname[]="www.usna.edu";  //the hostname we are looking up

  struct addrinfo *result;    //to store results
  struct addrinfo hints;      //to indicate information we want

  struct sockaddr_in *saddr;  //to reference address

  int s; //for error checking

  memset(&hints,0,sizeof(struct addrinfo));  //zero out hints
  hints.ai_family = AF_INET; //we only want IPv4 addresses

  //Convert the hostname to an address
  if( (s = getaddrinfo(hostname, NULL, &hints, &result)) != 0){
    fprintf(stderr, "getaddrinfo: %s\n",gai_strerror(s));
    exit(1);
  }

  //convert generic socket address to inet socket address
  saddr = (struct sockaddr_in *) result->ai_addr;

  //print the address
  printf("Hello %s\n", inet_ntoa(saddr->sin_addr));

  //free the addrinfo struct
  freeaddrinfo(result);
}
*/

/*
int main(){

  //in_addr struct has a single member s_addr
  struct in_addr addr;

  //Convert the IP dotted quad into struct in_addr
  inet_aton("10.4.32.41", &(addr));

  printf("Hello %s\n", inet_ntoa(addr));

}
*/

/*
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>


int main(){

  //in_addr struct has a single member s_addr
  struct in_addr addr;

  //Convert the IP dotted quad into struct in_addr
  inet_aton("10.4.32.41", &(addr));

  printf("Hello %s\n", inet_ntoa(addr));

}
*/

/*
int main(){

  //in_addr struct has a single member s_addr
  struct in_addr addr;
  unsigned char * ip;

  //have ip point to s_addr
  ip = (unsigned char *) &(addr.s_addr);

  //set the bytes for "10.4.32.41"
  ip[0]=10;
  ip[1]=4;
  ip[2]=32;
  ip[3]=41;

  //print it out
  printf("Hello %s\n", inet_ntoa(addr));

}
*/
