#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<sys/types.h>
#include<unistd.h>
#include<fcntl.h>

#define PACKET 4096
#define RATIO  256

int recvfile(int);

int main(int argc,char *argv[])
{
 if(argc!=2)
 {
  printf("Invalid no. of arguments\n");
  printf("Usage: recieve <server-ip>\n");
  exit(0);
 }
 
 struct addrinfo hints,*res;
 struct sockaddr_storage cli_addr;
 socklen_t size;
 memset(&hints,0,sizeof(struct sockaddr_in));
 
 int sd;
 int new_sd;
 
 sd=socket(PF_INET,SOCK_STREAM,0);
 if(sd==-1)
 {
  printf("Socket creation failed\n");
  exit(1);
 }
 
 hints.ai_family=AF_INET;
 hints.ai_socktype=SOCK_STREAM;
 getaddrinfo(argv[1],"8000",&hints,&res);
 
 if(bind(sd,res->ai_addr,res->ai_addrlen)==-1)
 {
  printf("Failed to bind\n");
  exit(1);
 }

 if(listen(sd,1)==-1)
 {
  printf("Failed to listen\n");
  exit(1);
 }
 
 printf("Server is up, waiting for connections..\n");
 new_sd=accept(sd,(struct sockaddr *)&cli_addr,&size);

 if(new_sd==-1)
 {
  printf("Connection could not be accepted\n");
  exit(1);
 }
 printf("Connected\n");
 printf("Recieving file...\n");
 
 recvfile(new_sd);
 close(sd);
 return 0;
}




int recvfile(int sock)
{
 int fp,bt;
 int sz=0;
 
 char buf[PACKET];
 memset(&buf,0,PACKET);
 
 char name[50];
 memset(&name,0,50);
 
 recv(sock,name,50,0);
 
 fp=open(name,O_TRUNC|O_WRONLY|O_CREAT);
 if(fp==-1)
 {
  printf("File creation failed\n");
  close(sock);
  return 1;
 }
 
 bt=send(sock,"start",5,0);
 if(bt!=5)
  return 1;

 while(1)
 {
  
  bt=recv(sock,buf,PACKET,0);
  if(bt==0)
   break;
  write(fp,buf,bt);
  sz++;
  
  if((sz%RATIO)==0)
   printf("%d Mb recieved\n",sz/RATIO);
 }
 close(sock);
 close(fp);
 printf("File recieved!\n");
}
