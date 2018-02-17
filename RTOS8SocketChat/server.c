/*program has to be terminated using "bye" statement only*/
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#define PORT 4777
int main(int argc, char const *argv[])
{
    int srvr, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *Msg ;
      
    // Creating socket file descriptor
    if ((srvr = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
       
    if (setsockopt(srvr, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
      
    // binding
    if (bind(srvr, (struct sockaddr *)&address, 
                                 sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    //listen
    if (listen(srvr, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    //accept
    if ((new_socket = accept(srvr, (struct sockaddr *)&address, 
                       (socklen_t*)&addrlen))<0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    //chat begins
   
    while(!(strcmp(buffer,"bye")==0)){//program has to be terminated by typing bye
    valread = read( new_socket , buffer, 1024);
    printf("%s\n",buffer );
    printf("please enter text which has to be sent to client\n");
    scanf("%[^\n]%*c",Msg);
    send(new_socket , Msg , strlen(Msg)+1 , 0 );
    printf("message sent from server\n");
    }
    close(srvr);
}
