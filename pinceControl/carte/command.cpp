#include "command.h"

/* Analyse le message et effectue les actions associees
 * 	<> id : l'identifiant associe au message
 * 	<> header : le type de message (en-tete)
 * 	<> args : le tableau d'entier contenant les arguments
 * */

// from, id_cmd, *args, sizeArgs
void cmd(int id, int header, int *args, int nb){              
	/* On analyse le message en fonction de son type */
	switch(header){
		case Q_PING:{//ok
			sendMessage(id, (char*)"Pong");
			break;
		}
		case Q_IDENT:{//ok
			sendMessage(id, (char*)"pince");
			break;
		}
		case Q_SHARP:{//ok
			sendMessage(id, getSharp(args[0]));
			break;
		}
		case Q_TSHARP:{
			sendMessage(id, setTriggerSharp(id, args[0],args[1]));
			break;
		}
		case Q_MS:{//ok
			sendMessage(id, getMicroSwitch(args[0]));
			break;
		}
		case Q_TMS:{
			sendMessage(id, setTriggerMS(id, args[0],args[1]));
			break;
		}
		case Q_PINCE:{
			sendMessage(id, setPinceState(args[0],args[1]));
			break;
		}
		case Q_PPOSITION:{
			sendMessage(id, setPincePosition(id, args[0],args[1]));
			break;
		}
		case Q_LOAD:{
			sendMessage(id, presentLoad(args[0]));
			break;
		}
		case Q_JACK:{
			sendMessage(id, jack(id));
			break;
		}
		case Q_LED:{
			sendMessage(id, setLED(args[0]));
			break;
		}
		case Q_PION:{
			sendMessage(id, getPion(args[0]));
			break;
		}
		case Q_MOTOR:{
			sendMessage(id, -42);
			break;
		}
		default:{
			sendMessage(id,-1);
			break;
		}
	}
}
