/*
Keep the respective arithmetic operation seperated by space ex:(2 + 3) in a file and pass the respective file as arguement to this program 
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>

#define SERVER 1L
typedef struct {
    long    msg_to;
    long    msg_fm;
    char    buffer[BUFSIZ];
} MESSAGE;
 

int mid;
key_t key;
struct msqid_ds buf;
MESSAGE msg;
FILE *inFile;
 
int main(int argc, char** argv) {
 
    //getting Message Queue ID
    key = ftok(".", 'z');
    mid = msgget(key, 0);
 
    //Display Message Queue and Client ID
    printf("Message Queue ID: %d\n", mid);
    printf("Client ID: %ld\n", (long)getpid());
 
    //Opening input file
    inFile = fopen(argv[1], "r");
    if(inFile == NULL){
        printf("Unable to open File = %s\n", argv[1]);
        return 1;
    }
 
    //Copy input characters into msg.buffer,
    int i = 0;
    while(1){
        msg.buffer[i] = fgetc(inFile);
        if(msg.buffer[i]==EOF){
            msg.buffer[i] = '\0';
            break;
        }
        i++;
    }
 
    //Displaying message
    printf("Message before conversion:\n");
    printf("%s\n", msg.buffer);
 
    //Getting Client PID 
    long iD = (long)getpid();
    msg.msg_to = SERVER;
    msg.msg_fm = (long)getpid();
   
 
    //Send message to Message Queue for Server, throws and error for invalid input
    if(msgsnd(mid, &msg, sizeof(msg.buffer), 0)==-1){
        perror("msgsnd");
        exit(-1);
    }
 
    //Client waits for response from Server, throws an error if invalid input
    if(msgrcv(mid, &msg, sizeof(msg), iD, 0)<0){
        perror("msgrcv");
        exit(-1);
    }
 
    //Display new message.
    printf("Message after Corresponding computation from server\n");
    printf("%s\n", msg.buffer);

    //Client exits
    return (EXIT_SUCCESS);
}
