#include "WProgram.h"
#include "getters.h"


void cmd(int name)
{
	switch (name)
	{
		case 0: // identification
			identification();
			break;
		case 1: // sharp
			getSharp();
			break;
		case 2: //ledOn
			ledOn();
			break;
		case 3:
			ledOff();
			break;
		default:
			indexError();
			break;
	}
}

void identification()
{
	Serial.println(3);
}

void getSharp()
{
  	int sensorValue = analogRead(A0);
	Serial.print(sensorValue);
	Serial.print('\n');
}

void indexError()
{
	Serial.println(-1);
}

void ledOn()
{
	digitalWrite(12, HIGH);
}

void ledOff()
{
	digitalWrite(12, LOW);
}
