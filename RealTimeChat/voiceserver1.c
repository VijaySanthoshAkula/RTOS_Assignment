/*program has to be terminated using "bye" statement only*/
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <string.h>
#include <stddef.h>
#include <pulse/simple.h>
#include <pulse/error.h>
#include<arpa/inet.h>
#define PORT 6542
#define BUFSIZE 21500
#define RATE 44100
static pa_simple *s = NULL;
static char name_buf[] = "PulseAudio default device";

int adin_pulseaudio_begin(char *arg)
{
  int error;

  static const pa_sample_spec ss = {
  	.format = PA_SAMPLE_S16LE,
  	.rate = RATE,
  	.channels = 1
  };

if (!(s = pa_simple_new(NULL, "Julius", PA_STREAM_PLAYBACK, NULL, "playback", &ss, NULL, NULL, &error))) {
        fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
        return 1;
    }
  return 0;
}

int adin_pulseaudio_end()
{
  if (s != NULL) {
    pa_simple_free(s);
    s = NULL;
  }
  return 0;
}

int main(int argc, char const *argv[])
{
    int srvr, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    int16_t buffer[BUFSIZE];
    int16_t Msg[BUFSIZE] ;
      int bufsize = BUFSIZE * sizeof(int16_t);
    // Creating socket file descriptor
    if ((srvr = socket(AF_INET, SOCK_DGRAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
       
    /*if (setsockopt(srvr, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }*/
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
   
    //chat begins
   ssize_t r;
    int error;
   
while(r!=50000){
printf("Waiting for data...");
adin_pulseaudio_begin("nothing");
 
   r = recvfrom( srvr , buffer, bufsize,0,(struct sockaddr *)&address, 
                       &addrlen) ;
            
   /*  if (write(STDOUT_FILENO, buffer, BUFSIZE/2) != BUFSIZE/2) {
            fprintf(stderr, __FILE__": write() failed: %s\n", strerror(errno));
            goto finish;
        }*/
    if (pa_simple_write(s, buffer, bufsize, &error) < 0) {
            fprintf(stderr, __FILE__": pa_simple_write() failed: %s\n", pa_strerror(error));
            goto finish;
        }
   // printf("message received from client\n");
    printf("number of bytes received %d\n",r);
adin_pulseaudio_end();
 
  /*  if (pa_simple_drain(s, &error) < 0) {
        fprintf(stderr, __FILE__": pa_simple_drain() failed: %s\n", pa_strerror(error));
        goto finish;
    }

*/}

    close(srvr);
    finish:
    adin_pulseaudio_end();

	return 0;
}
