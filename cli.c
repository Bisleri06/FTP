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

int sendfile(int,char *);

int main(int argc,char *argv[])
{
 if(argc!=3||(strlen(argv[1])>50))
 {
  printf("Invalid arguments\n");
  printf("Usage: send <file> <dest-ip>\n");
  printf("Please keep filename below 50 chars.\n");
  exit(0);
 }

 struct addrinfo hints,*res;
 char *fname=argv[1];
 char *ip=argv[2];
 int soc;
 int temp;
 
 memset(&hints,0,sizeof(struct addrinfo));
 hints.ai_family=AF_INET;
 hints.ai_socktype=SOCK_STREAM;

 soc=socket(PF_INET,SOCK_STREAM,0);
 if(soc==-1)
 {
  perror("Socket");
  exit(1);
 }

 getaddrinfo(ip,"8000",&hints,&res);
 
 temp=connect(soc,res->ai_addr,res->ai_addrlen);
 if(temp==-1)
 {
  perror("Connection");
  exit(1);
 }
 
 sendfile(soc,fname);
 printf("File has been sent!\n");
}




int sendfile(int sock,char file[50])
{
 int fp,bytes;
 char buf[PACKET];
 int count=0;
 memset(&buf,0,PACKET);
 char *test=(char *)malloc(5);
 
 fp=open(file,O_RDONLY);
 if(fp==-1)
 {
  perror("File error");
  exit(2);
  close(sock);
 }
 
 bytes=send(sock,file,strlen(file),0);
 if(bytes<strlen(file))
 {
  printf("Send error\n");
  exit(2);
 }
 
 bytes=recv(sock,test,5,0);
 
 if(bytes!=5||strcmp(test,"start")!=0)
 {
  perror("Verify");
  printf("Could not verify reciever\n");
  exit(2);
 }
 
 printf("Sending file...\n");
 
 while(1)
 {
  bytes=read(fp,buf,PACKET);
  send(sock,buf,PACKET,0);
  count++;

  if(count%RATIO==0)
  {
   printf("%d Mb sent..\n",(count/RATIO));
  }
  
  if(bytes<PACKET)
  {
   printf("Closing..\n");
   break;
  }
 }
 close(sock);
 close(fp);
 return 0;
}
