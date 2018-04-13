/*program has to be terminated by typing "bye" only*/
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#define PORT 6998
  
pthread_t t1, t2;
int sckt = 0;
int  valread;
char buffer[1024] = {0};
char Msg[1024];
 struct sockaddr_in address;
    
    struct sockaddr_in serv_addr;

void * Thread1Proc(){
while(1){
    printf("enter the string to be send to server\n");
    scanf("%[^\n]%*c",Msg);
    write(sckt , Msg , strlen(Msg)+1 , 0 );
    printf("message sent to server\n");
    }
}

void * Thread2Proc(){
 while(!(strcmp(buffer,"bye")==0)){
    valread = recv( sckt , buffer, sizeof(buffer),0);
    printf("%s\n",buffer );
printf("message received from server\n");
}
}
int main(int argc, char const *argv[])
{
   
    
    
    if ((sckt = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Not able to create socket \n");
        return -1;
    }
  
    memset(&serv_addr, '0', sizeof(serv_addr));
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
      
    
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) 
    {
        printf("\nInvalid address/ Address is not supported \n");
        return -1;
    }
    //connect
    if (connect(sckt, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    } 
    //chat begins
    pthread_create(&t1,NULL,Thread1Proc,NULL);
    pthread_create(&t2,NULL,Thread2Proc,NULL);
    pthread_join(t1,NULL);
    pthread_join(t2,NULL);
   /* while(!(strcmp(buffer,"bye")==0)){//program has to be terminated by typing bye
    printf("enter the string to be send to server\n");
    scanf("%[^\n]%*c",Msg);
    write(sckt , Msg , strlen(Msg)+1 , 0 );
    printf("message sent from client\n");
    valread = read( sckt , buffer, 1024);
    printf("%s\n",buffer );
    }
    close(sckt);*/
    while(1);
}
