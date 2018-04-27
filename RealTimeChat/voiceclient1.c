/*Real time chat client*/
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <string.h>
#include <stddef.h>
#include<pthread.h>
#include<unistd.h>
#include<math.h>
#include<arpa/inet.h>


#include <pulse/simple.h>
#include <pulse/error.h>
#define PORT 20000  /* port number */
#define BUFSIZE 21500
#define RATE 44100
static pa_simple *s = NULL;
static pa_simple *s1 = NULL;
static char name_buf[] = "PulseAudio default device";
struct sockaddr_in address;
int sckt = 0, valread;
struct sockaddr_in serv_addr;
int addrlen = sizeof(serv_addr);
int16_t Msg[BUFSIZE];
int bufsize = BUFSIZE * sizeof(int16_t);
int16_t buffer[BUFSIZE];
pthread_t t1, t2;
int sendlength=40000;
int recvlength=34488;
int adin_pulseaudio_standby(int sfreq, void *dummy)
{
	return 0;
}

int adin_pulseaudio_begin(char *arg)
{
	int error;

	static const pa_sample_spec ss = {
		.format = PA_SAMPLE_S16LE,
				.rate = RATE,
				.channels = 1
	};
	/* Stream for Recording*/
	if (!(s = pa_simple_new(NULL, "client", PA_STREAM_RECORD, NULL, "record", &ss, NULL, NULL, &error))) {
		printf("Error: adin_pulseaudio: pa_simple_new() failed: %s\n", pa_strerror(error));
		return 1;
	}
	return 0;
}

int adin_pulseaudio_beginplay(char *arg)
{
	int error;

	static const pa_sample_spec ss1 = {
		.format = PA_SAMPLE_S16LE,
				.rate = RATE,
				.channels = 1
	};
	/* Stream for Playing*/
	if (!(s1 = pa_simple_new(NULL, "client", PA_STREAM_PLAYBACK, NULL, "playback", &ss1, NULL, NULL, &error))) {
		fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
		return 1;
	}
	return 0;
}
/* Kill the connection to server for record stream*/
int adin_pulseaudio_end()
{
	if (s != NULL) {
		pa_simple_free(s);
		s = NULL;
	}
	return 0;
}
/* Kill the connection to server for playback stream*/
int adin_pulseaudio_endplay()
{
	if (s1 != NULL) {
		pa_simple_free(s1);
		s1 = NULL;
	}
	return 0;
}
/* method to read audio samples*/
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

/*This thread records and sends the audio packet*/
void * Thread1Proc(){

	int i=0;
	ssize_t r;


	printf("enter the string to be send to server\n");

	while(r!=sendlength){
		adin_pulseaudio_begin("nothing");
		adin_pulseaudio_read(Msg, BUFSIZE);
		//usleep(5);

		/*if (write(STDOUT_FILENO, Msg, BUFSIZE) != BUFSIZE) {
            fprintf(stderr, __FILE__": write() failed: %s\n", strerror(errno));
            goto finish;
        }*/

		r=sendto(sckt , Msg , bufsize ,0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

		// printf("message sent from client\n");
		printf("number of bytes sent %d",r);
		//valread = read( sckt , buffer, bufsize);

		adin_pulseaudio_end();
	}   
}
/*This thread receive the audio packet and plays the packet*/
void * Thread2Proc(){
	ssize_t r;
	int error;

	while(r!=recvlength){
		adin_pulseaudio_beginplay("nothing");
		r = recvfrom( sckt , buffer, bufsize,0,(struct sockaddr *)&serv_addr, 
				&addrlen) ;

		if (pa_simple_write(s1, buffer, bufsize, &error) < 0) {
			fprintf(stderr, __FILE__": pa_simple_write() failed: %s\n", pa_strerror(error));
			goto finish;
		}

		printf("number of bytes received %d\n",r);
		adin_pulseaudio_endplay();
		finish:
			adin_pulseaudio_endplay();
	}
}


int main(int argc, char const *argv[])
{
	/* Creating socket file descriptor UDP */
	if ((sckt = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		printf("\n Not able to create socket \n");
		return -1;
	}
	/* Clearing the Server address strcture */
	memset(&serv_addr, '0', sizeof(serv_addr));
	/* Specifying port and address of server */
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(PORT);

	int i=0;
	//chat begins
	printf("please start talking..............\n");
	pthread_create(&t1,NULL,Thread1Proc,NULL);
	pthread_create(&t2,NULL,Thread2Proc,NULL);
	pthread_join(t1,NULL);
	pthread_join(t2,NULL);

	while(1);

	close(sckt);
	finish:
		adin_pulseaudio_end();

	return 0;

}
