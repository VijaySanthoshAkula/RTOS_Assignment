#include <unistd.h>
#include <stdlib.h>


int main(int argc, char** argv)
{
        int des_p[2];
        if(pipe(des_p) == -1) {
          perror("Pipe failed");
          exit(1);
        }

        if(fork() == 0)            //first fork
        {
            close(STDOUT_FILENO);  //closing stdout
            dup(des_p[1]);         //replacing stdout with pipe write 
            close(des_p[0]);       //closing pipe read
            system("ls -1 *.txt");
            
          //  exit(0);
        }

        else            
        {
            wait(0);
            close(STDIN_FILENO);   //closing stdin
            dup(des_p[0]);         //replacing stdin with pipe read
            close(des_p[1]);       //closing pipe write
            system("wc -l");
           // exit(0);
        }

        
       
        //wait(0);
        close(des_p[0]);
        close(des_p[1]);
        return 0;
}
