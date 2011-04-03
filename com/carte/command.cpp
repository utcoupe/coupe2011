#include "WProgram.h"
#include "command.h"
#include "message.h"

// Message est le tableau de message
// m est le message
void cmd(int id, int id_cmd, int *args, int sizeArgs)
{
	// On analyse le message en fonction de son type
	switch(id_cmd)
	{
		case Q_PING: // Ping (renvoit pong)
			sendMessage(id, (char*)"Pong");
		break;

		case Q_IDENT: // Identification
			sendMessage(id, (char*)"autre");
		break;
		default:
			sendMessage(id, INVALID_CMD);
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

