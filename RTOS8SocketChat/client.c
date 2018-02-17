/*program has to be terminated by typing "bye" only*/
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#define PORT 4777
  
int main(int argc, char const *argv[])
{
    struct sockaddr_in address;
    int sckt = 0, valread;
    struct sockaddr_in serv_addr;
    char *Msg ;
    char buffer[1024] = {0};
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
    while(!(strcmp(buffer,"bye")==0)){//program has to be terminated by typing bye
    printf("enter the string to be send to server\n");
    scanf("%[^\n]%*c",Msg);
    write(sckt , Msg , strlen(Msg)+1 , 0 );
    printf("message sent from client\n");
    valread = read( sckt , buffer, 1024);
    printf("%s\n",buffer );
    }
    close(sckt);
}
