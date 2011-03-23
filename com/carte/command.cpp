#include "WProgram.h"
#include "command.h"
#include "message.h"

// Message est le tableau de message
// m est le message
void cmd(int id_msg, int* message)
{
	int i;
	// On analyse le message en fonction de son type
	switch(c){
		case PING: // Ping (renvoit time : pong)
			sendMessage(id_msg, (char*)"Pong");
		break;

		case IDENTIFICATION: // Identification
			sendMessage(id_msg, (char*)"autre");
		break;
		case SHARP:
			int v;
			v = getSharp(message[0]);
			if (v < 0)
				error(id_msg);
			else
				sendMessage(id_msg, v);
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
			error(id_msg);
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

void error(int id_msg)
{
	sendMessage(id_msg, (char*)"-1");
}


typedef struct Sharp
{
	int pin;
	bool to_check;
	int seuil;
	int id_msg;
}Sharp;
const NB_SHARPS = 15;
Sharp sharps[NB_SHARPS];

/**
 * @todo remplir la fonction
 * remplit la valeur pin des sharps du tableau
 * 
 * @return none
 */
void init_sharps()
{

}

/**
 * passe à true la valeur check du sharp à surveiller
 *
 * @return none
 */
void surveiller(int id_msg, char id_sharp)
{
	sharps[id_sharp].id_msg = id_msg;
	sharps[id_sharp].to_check = true;
}

/**
 * @todo voir si la foncion n'st pas trop lente, si c'est le cas, la "casser"
 * 
 * Check si la valeur des sharps dépasse le seuil autorisé
 *
 * @return none
 */
void check_sharps()
{
	for (unsigned int i=0; i< NB_SHARPS; ++i)
		// si il est à observer
		if (sharps[i].to.check)
			// si la valeur dépasse le seuil
			if (analogRead(analogInPin) > sharps[i].seuil)
			{
				sendMessage(sharps[i].id, 1); // envoie que le seuil a été dépassé
				break;
			}
}


