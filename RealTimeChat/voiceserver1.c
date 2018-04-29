/*Real time audio server*/
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
#include<pthread.h>
#define PORT 20000  /* port number */
#define BUFSIZE 21500
#define RATE 44100
static pa_simple *s = NULL;
static pa_simple *s1 = NULL;
static char name_buf[] = "PulseAudio default device";
static int srvr, new_socket, valread;
static struct sockaddr_in address;
int opt = 1;
int addrlen = sizeof(address);
static int16_t buffer[BUFSIZE];
static  int16_t Msg[BUFSIZE] ;
int bufsize = BUFSIZE * sizeof(int16_t);
pthread_t t1, t2;
int sendlength=40000;
int recvlength=34488;
int  adin_pulseaudio_begin(char *arg)
{
	int error;

	static const pa_sample_spec ss = {
		.format = PA_SAMPLE_S16LE,
				.rate = RATE,
				.channels = 1
	};
	/* Stream for Playing*/
	if (!(s = pa_simple_new(NULL, "server", PA_STREAM_PLAYBACK, NULL, "playback", &ss, NULL, NULL, &error))) {
		fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
		return 1;
	}
	return 0;
}

int  adin_pulseaudio_beginrecord(char *arg)
{
	int error;

	static const pa_sample_spec ss1 = {
		.format = PA_SAMPLE_S16LE,
				.rate = RATE,
				.channels = 1
	};
	/* Stream for Recording*/
	if (!(s1 = pa_simple_new(NULL, "server", PA_STREAM_RECORD, NULL, "record", &ss1, NULL, NULL, &error))) {
		printf("Error: adin_pulseaudio: pa_simple_new() failed: %s\n", pa_strerror(error));
		return 1;
	}
	return 0;
}
/* Kill the connection to server for playback stream*/
int  adin_pulseaudio_end()
{
	if (s != NULL) {
		pa_simple_free(s);
		s = NULL;
	}
	return 0;
}
/* Kill the connection to server for record stream*/
int  adin_pulseaudio_endrecord()
{
	if (s1 != NULL) {
		pa_simple_free(s1);
		s1 = NULL;
	}
	return 0;
}
/* method to read audio samples*/
int  adin_pulseaudio_read (int16_t *buf, int sampnum)
{
	int error;
	int cnt, bufsize;


	bufsize = sampnum * sizeof(int16_t);
	if (bufsize > BUFSIZE) bufsize = BUFSIZE;

	if (pa_simple_read(s1, buf, bufsize, &error) < 0) {
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
		adin_pulseaudio_beginrecord("nothing");
		adin_pulseaudio_read(Msg, BUFSIZE);


		r=sendto(srvr , Msg , bufsize ,0, (struct sockaddr *)&address, sizeof(address));
		printf("number of bytes sent %d",r);
		adin_pulseaudio_endrecord();
	}   
}
/*This thread receive the audio packet and plays the packet*/
void * Thread2Proc(){
	ssize_t r;
	int error;

	while(r!=recvlength){
		printf("Waiting for data...");
		adin_pulseaudio_begin("nothing");

		r = recvfrom( srvr , buffer, bufsize,0,(struct sockaddr *)&address, 
				&addrlen) ;


		if (pa_simple_write(s, buffer, bufsize, &error) < 0) {
			fprintf(stderr, __FILE__": pa_simple_write() failed: %s\n", pa_strerror(error));
			goto finish;
		}

		printf("number of bytes received %d\n",r);
		adin_pulseaudio_end();
		finish:
			adin_pulseaudio_end();
	}

}


int main(int argc, char const *argv[])
{

	/* Creating socket file descriptor UDP */
	if ((srvr = socket(AF_INET, SOCK_DGRAM, 0)) == 0)
	{
		perror("socket failed");
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

	//chat begins

	recvfrom( srvr , buffer, bufsize,0,(struct sockaddr *)&address, 
			&addrlen) ;
	printf("Please start talking\n");
	pthread_create(&t2,NULL,Thread2Proc,NULL);
	pthread_create(&t1,NULL,Thread1Proc,NULL);
	pthread_join(t2,NULL);
	pthread_join(t1,NULL);





	while(1);
	close(srvr);
	finish:
		adin_pulseaudio_end();

	return 0;
}
