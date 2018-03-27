#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <math.h>
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

float calculate(float firstNumber,char operator[],float secondNumber){
float result=-1;
switch(operator[0])
    {
        case '+':
            result=firstNumber + secondNumber;
            printf("%.1lf + %.1lf = %.1lf",firstNumber, secondNumber, result);
            break;

        case '-':
            result=firstNumber - secondNumber;
            printf("%.1lf - %.1lf = %.1lf",firstNumber, secondNumber, result);
            break;

        case '*':
            result=firstNumber * secondNumber;
            printf("%.1lf * %.1lf = %.1lf",firstNumber, secondNumber, result);
            break;

        case '/':
            result=firstNumber / secondNumber;
            printf("%.1lf / %.1lf = %.1lf",firstNumber, secondNumber, result);
            break;

        
        default:
            printf("Error! operator is not correct");
    }
return result;
}


int main(int argc, char** argv) {
    char sInstruction[BUFSIZ];
    char *FirstOperand;
    char *Operation;
    char *SecondOperand;
    char *search=" ";
    float result=0;
    //Creating a message queue
    key = ftok(".", 'z');
    if((mid = msgget(key, IPC_CREAT | 0660))<0){
        printf("Error Creating Message Queue\n");
        exit(-1);
    }
 
    //Display Message Queue and Server ID
    printf("Message Queue ID: %d\n", mid);
    printf("Server ID: %ld\n", (long)getpid());    
    while(1){
    //Receiving message from client, throws and error if input is invalid
    if(msgrcv(mid, &msg, sizeof(msg.buffer), SERVER, 0)<0){
        perror("msgrcv");
        exit(-1);
    }
 
    //displaying received message
    printf("SERVER receives:\n");
    printf("%s\n", msg.buffer);
 
    //reading Cliend PID to for returning
    long client = msg.msg_fm;
    
    //read the msg to instruction string
    int i=0;
    while(msg.buffer[i] != '\0'){
        sInstruction[i] = msg.buffer[i];
        i++;
    }
 
    FirstOperand =strtok(sInstruction,search);
    Operation=strtok(NULL,search);
    SecondOperand=strtok(NULL,search);
    result=calculate(atof(FirstOperand),Operation,atof(SecondOperand));
    gcvt(result,7, msg.buffer);//converting float to string
    //preparing return message
    msg.msg_fm = SERVER;
    msg.msg_to = client;
 
    //send converting message back to client, throws and error if input is invalid
    if(msgsnd(mid, (struct MESSAGE*)&msg, sizeof(msg.buffer), 0)==-1){
        perror("msgsnd");
        exit(-1);
    }
 
    //server exits
    //return (EXIT_SUCCESS);
}
}
