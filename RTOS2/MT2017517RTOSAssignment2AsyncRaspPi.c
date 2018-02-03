#include<wiringPi.h>
#include<stdio.h>

int main()
{
	int PastData = 0, PresentData = 0;

	wiringPiSetupPhys(); 
	pinMode(37, INPUT);  // Msb
	pinMode(35, INPUT);
	pinMode(33, INPUT);
	pinMode(31, INPUT);  // Lsb

	while(1)
	{  
		PresentData = 0;
		PresentData = digitalRead(37);//Msb
		PresentData = PresentData << 1;
		PresentData = (PresentData | digitalRead(35));
		PresentData = PresentData << 1;
		PresentData = (PresentData | digitalRead(33));
		PresentData = PresentData << 1;
		PresentData = (PresentData | digitalRead(31));//lsb
		if(PresentData != PastData)
		{
			printf("%d\n", PresentData);
			PastData = PresentData;   
		}
		delay(500);
	}
	return 0;
}