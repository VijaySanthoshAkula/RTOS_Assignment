/*please enter the integer value which has to be populated in shared memory as an arguemnt*/
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include  "shared.h"
#include  <sys/ipc.h>
#include  <sys/shm.h>

int main(int  argc, char *argv[])
{
     key_t          KEY;
     int            mID;
     struct Memory  *PTR;
pid_t childPID1;
printf("parent pid is %d \n", getpid());

childPID1 = fork();

if(childPID1 >= 0) // fork was successful
{
if(childPID1 == 0) // child process
{
KEY = ftok(".", 'x');
     mID = shmget(KEY, sizeof(struct Memory), 0666);
     if (mID < 0) {
          printf("shmget error\n");
          exit(1);
     }
     printf("Process 2 has received a shared memory...\n");

     PTR = (struct Memory *) shmat(mID, NULL, 0);
     if ((int) PTR == -1) {
          printf("shmat error (client)\n");
          exit(1);
     }
     printf("Process 2 has attached the shared memory...\n");

     while (PTR->status != FILLED)
          ;
     printf("Process 2 found the data is ready...\n");
     printf("Process 2 found %d in shared memory...\n",
                PTR->data[0]);

     PTR->status = TAKEN;
     printf("Process 2 has informed Process 1 that data have been taken...\n");
     shmdt((void *) PTR);
     printf("Process 2 has detached its shared memory...\n");
     printf("Process 2 exits...\n");
     exit(0);
}else{//parent
 if (argc != 2) {
          printf("Use: %s #1\n", argv[0]);
          exit(1);
     }

     KEY = ftok(".", 'x');
     mID = shmget(KEY, sizeof(struct Memory), IPC_CREAT | 0666);
     if (mID < 0) {
          printf("shmget error\n");
          exit(1);
     }
     printf("Process 1 has received a shared memory of one integer\n");

     PTR = (struct Memory *) shmat(mID, NULL, 0);
     if ((int) PTR == -1) {
          printf("shmat error\n");
          exit(1);
     }
     printf("Process 1 has attached the shared memory...\n");

     PTR->status  = NOT_READY;
     PTR->data[0] = atoi(argv[1]);
     printf("Process 1 has filled %d to shared memory...\n",
            PTR->data[0]);
     PTR->status = FILLED;

     printf("Please start the Process 2 in another window...\n");

     while (PTR->status != TAKEN)
          sleep(1);

     printf("Process 1 has detected the completion of Process 2 action...\n");
     shmdt((void *) PTR);
     printf("Process 1 has detached its shared memory...\n");
     shmctl(mID, IPC_RMID, NULL);
     printf("Process 1 has removed its shared memory...\n");
     printf("Process 1 exits...\n");
     exit(0);

}}else{
printf("\n Fork 1 failed, quitting!!!!!!\n");
return 1;
}
}
