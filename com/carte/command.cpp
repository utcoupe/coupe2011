#include "WProgram.h"
#include "command.h"
#include "message.h"

// Message est le tableau de message
// m est le message
void cmd(int c, float* message, int m)
{
	int i;
	// On analyse le message en fonction de son type
	switch(c){
		/*
			DEBUG ZONE

			Ces fonctions permettent entre autre le debug...
		*/
		case 'E': // Echo (renvoit le message qu'elle a reçu)
			Serial.print("E,");
			for (i=1;i<m;i++) {
				Serial.print("§message ");Serial.print(i);Serial.print(" ");Serial.print(message[i]); 
			}
				Serial.print("\n");
		break;
		case 'P': // Ping (renvoit time : pong)
			sendMessage(c, "Pong");
		break;

		case 'I': // Identification
			sendMessage(c, "Test Card");
		break;
		case 'S':
			//sendMessage(c, getSharp());
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

int getSharp()
{
  	int sensorValue = analogRead(A0);
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
