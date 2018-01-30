/*
RTOS Assignment :1
Author: A V Santhosh Kumar
Roll Number: MT2017517
Prequisite: Please keep the Samples.csv file in same folder as this program
Description: Following Program
       1.reads values from Samples.csv file and writes it to raw.temp without any filtering
       2.reads values from Samples.csv file and filters the data and writes it to filtered.temp
       3.plots raw.temp and filtered.temp
Filter Used: Butterworth Filter Second Order
PLotting Tool: GNU PLot

 */

/*header files*/
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#define NUM_COMMANDS 4


int main()
{
	char * commandsForGnuplot1[] = {"set title \"Raw Ecg\"","set xlabel \"time\"",
			"set ylabel \"Amplitude\"", "plot [100:600][400:580] 'raw.temp' with l"};//Commands for gnuplot raw
	char * commandsForGnuplot2[] = {"set title \"Filtered Ecg\"","set xlabel \"time\"",
			"set ylabel \"Amplitude\"", "plot [100:600][400:580] 'filtered.temp' with l"};//Commands for gnuplot  filtered

	float xvals[3];/*This is an Array which Stores latest 3 raw Ecg values xvals[0] being the latest*/
	float yvals[3];/*This is an Array which Stores latest 3 filtered values yvals[0] being the latest*/
	int iSampleCount=0;/*Sample Count*/
	float fc;/*cut off frequecny*/
	float fs;/*Sampling Frequency*/
        printf("please enter fc\n");
        scanf("%f",&fc);
        printf("please enter fs\n");
        scanf("%f",&fs);
	float i =(fc*3.14)/fs;/*Calculating the Angle part of tan*/
	float angle=tan(i);/*Tan of i*/
	float sqr=pow(angle,2);
	float divisor=1+angle*1.414+sqr;/*Common Divisor of Values in Filter Equation*/


	char val[10];/*THis is the value which is read from Sample.csv file*/
	FILE* fInFile = fopen("Samples.csv", "r");//Samples.csv is the values csv file, this file should be kept in same folder of this program
	FILE * temp1 = fopen("raw.temp", "w");//raw.temp file holds the values of Ecg Samples read from the Samples.csv.
	FILE * temp2 = fopen("filtered.temp", "w");//filtered.temp file holds the values of Ecg Samples read from the Samples.csv.


	/*Reading the Sample.csv and sending data to data.temp file*/
	while(fgetc(fInFile)!=EOF){
		fscanf(fInFile,  "%s", val);
		fprintf(temp1, "%d %f \n", iSampleCount, atof(val));
		iSampleCount++;
	}

	/*Reading the Sample.csv and filtering the corresponding values*/
	rewind(fInFile);
	iSampleCount=0;
	while(fgetc(fInFile)!=EOF){
		fscanf(fInFile,  "%s", val);

		if(iSampleCount<2){
			fprintf(temp2, "%d %f \n", iSampleCount, atof(val));
			xvals[2-iSampleCount]=atof(val);
			yvals[2-iSampleCount]=atof(val);
			iSampleCount++;
		}else{
			xvals[0]=atof(val);
			yvals[0]=(sqr*xvals[0]+2*sqr*xvals[1]+sqr*xvals[2]-(2*sqr-2)*yvals[1]-(1-1.414*angle+sqr)*yvals[2])/(1+angle*1.414+sqr);/*ButterWorth Filter Formula*/
			fprintf(temp2, "%d %f \n", iSampleCount, yvals[0]);
			yvals[2]=yvals[1];
			yvals[1]=yvals[0];
			xvals[2]=xvals[1];
			xvals[1]=xvals[0];
			iSampleCount++;
		}
	}
	//Plotting starts here
	/*Opens an interface that one can use to send commands as if they were typing into the
         gnuplot command line.  "The -persistent" keeps the plot open even after our
         C program terminates.
	 */
	FILE * gnuplotPipe1 = popen ("gnuplot -persistent", "w");/*window 1*/
	FILE * gnuplotPipe2 = popen ("gnuplot -persistent", "w");/*window 2*/

	for (int iCounter=0; iCounter < NUM_COMMANDS; iCounter++)
	{
		fprintf(gnuplotPipe1, "%s \n", commandsForGnuplot1[iCounter]); //Send commands to gnuplot one by one.
	}
	for (int iCounter=0; iCounter < NUM_COMMANDS; iCounter++)
	{
		fprintf(gnuplotPipe2, "%s \n", commandsForGnuplot2[iCounter]);
	}
	fclose(fInFile);
	fclose(temp1);
	fclose(temp2);
	fclose(gnuplotPipe1);
	return 0;
}
