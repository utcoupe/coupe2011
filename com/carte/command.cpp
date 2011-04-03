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
			sendMessage(id_from, id_cmd, (char*)"Pong");
		break;

		case Q_IDENT: // Identification
			sendMessage(id_from, id_cmd, (char*)"autre");
		break;
		default:
			sendMessage(id_from, id_cmd, INVALID_CMD);
		break;
	}
}

void ledOn()
{
	digitalWrite(12, HIGH);
}

void ledOff()
{
	digitalWrite(12, LOW);
}

