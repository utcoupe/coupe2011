#include "WProgram.h"
#include "command.h"
#include "message.h"

// Message est le tableau de message
// m est le message
void cmd(int id_from, int id_cmd, int *args, int sizeArgs)
{
	// On analyse le message en fonction de son type
	switch(id_cmd)
	{
		case Q_PING: // Ping (renvoit time : pong)
			sendMessage(id_from, (char*)"Pong");
		break;

		case Q_IDENT: // Identification
			sendMessage(id_from, (char*)"autre");
		break;
		default:
			error(id_from);
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
const int NB_SHARPS = 15;
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
		if (sharps[i].to_check)
			// si la valeur dépasse le seuil
			if (analogRead(sharps[i].pin) > sharps[i].seuil)
			{
				sendMessage(sharps[i].id_msg, 1); // envoie que le seuil a été dépassé
				break;
			}
}


