/* Rtp Server*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stddef.h>
#include <arpa/inet.h>
#include <pulse/error.h>
#include <pthread.h>
#include <pulse/simple.h>

#define BUFFERSIZE 21100

#define PORT 23000 /*even port number is used for rtp in between 16384 to 32767*/

#define RATE 44100 /* Inorder to cover all the audio frequencies*/

struct RTP_Header 
{
	uint8_t version:2;
uint8_t padding:1;
uint8_t extension:1;
uint8_t csrcCount:4;
uint8_t marker:1;
uint8_t payloadtype:7;
uint16_t sequencenumber;
uint32_t timeStamp;
uint32_t SSRC;
uint16_t buffer[BUFFERSIZE]; 
}TransmissionPacket, ReceivedPacket;

uint16_t SendingsideSeqNum = 0;
uint16_t ReceivingSeqNum = 0;   

int maximumsize=65536;
int sendlength=40000;
int recvlength=34488;
static const pa_sample_spec ss1 = {
	.format = PA_SAMPLE_S16LE,
			.rate = RATE,
			.channels = 1 
};


static const pa_sample_spec ss = {
	.format = PA_SAMPLE_S16LE,
			.rate = RATE,
			.channels = 1 
};



static pa_simple *playing = NULL;
static pa_simple *recording = NULL;
static int srvr_sockt;
static struct sockaddr_in address;
int addrlen = sizeof(address);
int bufsize = BUFFERSIZE * sizeof(int16_t); 


pthread_t SendThread, ReceiveThread;

int  pulseaudioPlayingHandle()
{
	int error;

	static const pa_sample_spec play = {
		.format = PA_SAMPLE_S16LE,
				.rate = RATE,
				.channels = 1
	};
	/* Stream for Playing*/
	if (!(playing = pa_simple_new(NULL, "rtp", PA_STREAM_PLAYBACK, NULL, "playback", &play, NULL, NULL, &error))) {
		fprintf(stderr, __FILE__"pulseaudioPlayingHandle %s\n", pa_strerror(error));
		return 1;
	}
	return 0;
}

int  pulseaudioRecordHandle()
{
	int error;

	static const pa_sample_spec record = {
		.format = PA_SAMPLE_S16LE,
				.rate = RATE,
				.channels = 1
	};
	/* Stream for Recording*/
	if (!(recording = pa_simple_new(NULL, "rtp", PA_STREAM_RECORD, NULL, "record", &record, NULL, NULL, &error))) {
		printf("pulseaudioRecordHandle failed: %s\n", pa_strerror(error));
		return 1;
	}
	return 0;
}
/* Kill the connection to server for playback stream*/
int  pulseaudioplay_end()
{
	if (playing != NULL) {
		pa_simple_free(playing);
		playing = NULL;
	}
	return 0;
}
/* Kill the connection to server for record stream*/
int  pulseaudio_endrecord()
{
	if (recording != NULL) {
		pa_simple_free(recording);
		recording = NULL;
	}
	return 0;
}

/* method to read audio samples*/
int  pulseaudio_read (int16_t *buff, int size)
{
	int error;
	int count, finalsize;


	finalsize = size * sizeof(int16_t);
	if (finalsize > BUFFERSIZE) finalsize = BUFFERSIZE;

	if (pa_simple_read(recording, buff, finalsize, &error) < 0) {
		printf("Error: pa_simple_read() failed: %s\n", pa_strerror(error));
	}
	count = finalsize / sizeof(int16_t);
	return (count);
}


/*This thread records and sends the audio packet*/

void * Thread1Send()
{
	size_t length;
	while(length!=sendlength)
	{
		pulseaudioRecordHandle();
		pulseaudio_read(TransmissionPacket.buffer, BUFFERSIZE);
		if(maximumsize == SendingsideSeqNum)
		{
			SendingsideSeqNum = 0;
		}
		TransmissionPacket.sequencenumber = SendingsideSeqNum++;

		struct timeval time;
		gettimeofday(&time, NULL); 
		TransmissionPacket.timeStamp = time.tv_sec;


		length=sendto(srvr_sockt, &TransmissionPacket, sizeof(TransmissionPacket), 0, (struct sockaddr *)&address, sizeof(address));
		printf("packet of bytes %ld and sequencenumber %d is sent.\n",length,SendingsideSeqNum); 
		pulseaudio_endrecord();
	}   
}
/*This thread receive the audio packet and plays the packet*/
void * Thread2Receive()
{
	size_t length;
	int error;

	while(length!=recvlength)
	{
		pulseaudioPlayingHandle();      
		length = recvfrom(srvr_sockt, &ReceivedPacket, sizeof(ReceivedPacket),0,(struct sockaddr *)&address, &addrlen); 
		printf("length of the packet received %ld and sequencenumber recvd-> %d. \n",length,ReceivedPacket.sequencenumber);
		if(ReceivingSeqNum <= ReceivedPacket.sequencenumber)
		{
			ReceivingSeqNum = ReceivedPacket.sequencenumber + 1;
			if (pa_simple_write(playing, ReceivedPacket.buffer, bufsize, &error) < 0)
			{
				printf(": pa_simple_write() failed: %s\n", pa_strerror(error));
				break;
			}

		} 
		pulseaudioplay_end();
	}   
}


int main()
{
	/* Creating socket file descriptor UDP */
	if ((srvr_sockt = socket(AF_INET, SOCK_DGRAM, 0)) == 0) // raw
	{
		printf("creating a Socket failed\n");
		return 0;
	}

	/* Specifying port and address of server */
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	/* Binding to the socket created above */
	if (bind(srvr_sockt, (struct sockaddr *)&address, sizeof(address))<0)
	{
		printf("bind failed\n");
		close(srvr_sockt);
		return 0;
	}

	//chat begins
	TransmissionPacket.version = 2;/*version field*/
	TransmissionPacket.padding = 0;/*indicates for extra padding bits at the end of the packet*/
	TransmissionPacket.extension = 0;/*indicates for any extension header*/
	TransmissionPacket.csrcCount = 0;/*0 since there is no other contributing source other than audio in the packet*/
	TransmissionPacket.marker = 0;/* provides boundary in data stream*/
	TransmissionPacket.payloadtype = 11;/*payload type is the format of data type for example in this case used frequency is 44100hz*/
	TransmissionPacket.SSRC = 2;/*source of the real time stream*/

	recvfrom(srvr_sockt, &ReceivedPacket, sizeof(ReceivedPacket), 0,(struct sockaddr *)&address, &addrlen);
	pthread_create(&ReceiveThread,NULL,Thread2Receive,NULL);
	printf("please start talking....!!\n"); 
	pthread_create(&SendThread,NULL,Thread1Send,NULL);
	pthread_join(ReceiveThread,NULL);
	pthread_join(SendThread,NULL);   


	while(1);
	close(srvr_sockt);
	finish:
		pulseaudioplay_end();
	return 0;
}
