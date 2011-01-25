#include "WProgram.h"
#include "command.h"

// Message est le tableau de message
// m est le message
void cmd(int* message, int m)
{
	int i;
	// On analyse le message en fonction de son type
	switch(message[0]){
		/*
			DEBUG ZONE

			Ces fonctions permettent entre autre le debug...
		*/
		case 'E': // Echo (renvoit le message qu'elle a reçu)
			for (i=1;i<m;i++) {
				Serial.print("§message ");Serial.print(i);Serial.print(" ");Serial.print(message[i]); 
			}
				Serial.print("\n");
		break;
		case 'P': // Ping (renvoit time : pong)
			Serial.print(millis());Serial.println(" : Pong");
		break;

		case 'I': // Identification
			identification();
		break;

		case 'S':
			getSharp();
		break;
		case 'L':
			ledOn();
		break;
		case 'l':
			ledOff();
		break;
		case '?':
			Serial.print("time: ");Serial.print(millis());
			Serial.print("Hey !! ");Serial.print("\n");
		break;
		default:
			Serial.print(-1);Serial.print("\n");
		break;
	}
}

void identification()
{
	Serial.println("Test card");
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
