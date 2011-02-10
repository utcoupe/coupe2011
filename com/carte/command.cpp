#include "WProgram.h"
#include "command.h"
#include "message.h"

// Message est le tableau de message
// m est le message
void cmd(int c, int* message)
{
	int i;
	// On analyse le message en fonction de son type
	switch(c){
		case PING: // Ping (renvoit time : pong)
			sendMessage(c, (char*)"Pong");
		break;

		case IDENTIFICATION: // Identification
			sendMessage(c, (char*)"autre");
		break;
		case SHARP:
			int v;
			v = getSharp(message[0]);
			if (v < 0)
				error(c);
			else
				sendMessage(c, v);
		break;
		/*case 'L':
			ledOn();
			sendMessage(c, (char*)"Led ON");
		break;
		case 'l':
			ledOff();
			sendMessage(c, (char*)"Led OFF");
		break;
		case 't':
			sendMessage(c, millis());
		break;*/
		default:
			error(c);
		break;
	}
}

int getSharp(int id)
{
  	int sensorValue = 0;
  	switch (id)
  	{
		case 0:
			sensorValue = analogRead(A0);
			break;
		case 1:
			sensorValue = analogRead(A1);
			break;
		case 2:
			sensorValue = analogRead(A2);
			break;
		case 3:
			sensorValue = analogRead(A3);
			break;
		default:
			sensorValue = -1;
			break;
	}
	return sensorValue;
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

void error(char c)
{
	sendMessage(c, (char*)"-1");
}


