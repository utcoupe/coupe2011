#include "command.h"

/* Analyse le message et effectue les actions associees
 * 	<> id : l'identifiant associe au message
 * 	<> header : le type de message (en-tete)
 * 	<> args : le tableau d'entier contenant les arguments
 * */
void cmd(int id, int header, int* args){              
	/* On analyse le message en fonction de son type */
	switch(header){
		case Q_PING:{
			sendMessage(id, (char*)"Pong");
			break;
		}
		case Q_IDENT:{
			sendMessage(id, (char*)"autre");
			break;
		}
		case Q_SHARP:{
			sendMessage(id, getSharp(args[0]));
			break;
		}
		case Q_TRIGGERSHARP:{
			sendMessage(id, setTriggerSharp(id, args[0],args[1]));
			break;
		}
		case Q_MS:{
			sendMessage(id, getMicroSwitch(args[0]));
			break;
		}
		case Q_TRIGGERMS:{
			sendMessage(id, setTriggerMS(id, args[0],args[1]));
			break;
		}
		case Q_STEPPER:{
			sendMessage(id, setStepper(id, args[0],args[1]));
			break;
		}
		case Q_PINCE:{
			sendMessage(id, setPinceState(args[0],args[1]));
			break;
		}
		default:{
			sendMessage(id,-1);
			break;
		}
	}
}
