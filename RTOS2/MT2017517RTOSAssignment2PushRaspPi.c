#include<wiringPi.h>
#include<stdio.h>


void ReadData(void)
{
	int data = 0;

	data = digitalRead(37);//msb
	data = data << 1;
	data = (data | digitalRead(35));
	data = data << 1;
	data = (data | digitalRead(33));
	data = data << 1;
	data = (data | digitalRead(31));//lsb

	printf("%d\n", data);
}

int main()
{
	wiringPiSetupPhys(); //
	pinMode(37, INPUT);  // MSb
	pinMode(35, INPUT);
	pinMode(33, INPUT);
	pinMode(31, INPUT);  // LSb

	pinMode(40, INPUT);  // Control signal

	wiringPiISR(40, INT_EDGE_RISING, ReadData); // Enabling interrupt

	while(1);
	return 0;
}