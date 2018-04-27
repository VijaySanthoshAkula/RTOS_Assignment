/*program has to be terminated by typing "bye" only*/
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
#define PORT 6542
#define BUFSIZE 21500
#define RATE 44100
static pa_simple *s = NULL;
static char name_buf[] = "PulseAudio default device";
int adin_pulseaudio_standby(int sfreq, void *dummy)
{
  return 0;
}
 
/** 
 * Start recording.
 * @a pathname is dummy.
 *
 * @param arg [in] argument
 * 
 * @return TRUE on success, FALSE on failure.
 */
int adin_pulseaudio_begin(char *arg)
{
  int error;

  static const pa_sample_spec ss = {
  	.format = PA_SAMPLE_S16LE,
  	.rate = RATE,
  	.channels = 1
  };
  
  if (!(s = pa_simple_new(NULL, "Julius", PA_STREAM_RECORD, NULL, "record", &ss, NULL, NULL, &error))) {
    printf("Error: adin_pulseaudio: pa_simple_new() failed: %s\n", pa_strerror(error));
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

int adin_pulseaudio_read (int16_t *buf, int sampnum)
{
  int error;
  int cnt, bufsize;
  

  bufsize = sampnum * sizeof(int16_t);
  if (bufsize > BUFSIZE) bufsize = BUFSIZE;

  if (pa_simple_read(s, buf, bufsize, &error) < 0) {
        printf("Error: pa_simple_read() failed: %s\n", pa_strerror(error));
  }
  cnt = bufsize / sizeof(int16_t);
  return (cnt);
}



int main(int argc, char const *argv[])
{
    struct sockaddr_in address;
    int sckt = 0, valread;
    struct sockaddr_in serv_addr;
    int16_t Msg[BUFSIZE];
    int bufsize = BUFSIZE * sizeof(int16_t);
    int16_t buffer[BUFSIZE];
    if ((sckt = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Not able to create socket \n");
        return -1;
    }
  
    memset(&serv_addr, '0', sizeof(serv_addr));
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr=inet_addr("172.16.83.132");
    
    if(inet_pton(AF_INET, "172.16.83.132", &serv_addr.sin_addr)<=0) 
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
    
    
        int i=0;
ssize_t r;

    
    printf("enter the string to be send to server\n");
    while(r!=50000){
    adin_pulseaudio_begin("nothing");
	adin_pulseaudio_read(Msg, BUFSIZE);
	usleep(5);
       
    /*if (write(STDOUT_FILENO, Msg, BUFSIZE) != BUFSIZE) {
            fprintf(stderr, __FILE__": write() failed: %s\n", strerror(errno));
            goto finish;
        }*/
    
   r=write(sckt , Msg , bufsize , 0 );
  
   // printf("message sent from client\n");
 printf("number of bytes sent %d",r);
    //valread = read( sckt , buffer, bufsize);
 adin_pulseaudio_end();
    
 }   
close(sckt);
finish:
	adin_pulseaudio_end();

	return 0;
    
}
