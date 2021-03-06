#include "command.h"

/* Analyse le message et effectue les actions associees
 * 	<> id : l'identifiant associe au message
 * 	<> header : le type de message (en-tete)
 * 	<> args : le tableau d'entier contenant les arguments
 * */

// from, id_cmd, *args, sizeArgs
void cmd(int id, int header, int *args, int size){
    /* On analyse le message en fonction de son type */
    switch(header)
    {
	case Q_IDENT:
	{
	    sendMessage(id, ID_OTHERS, (char*)"others");
	    break;
	}
	case Q_PING:
	{
	    sendMessage(id, (char*)"Pong");
	    break;
	}
	case Q_SHARP:
	{
	    if(size<1)
		sendMessage(id, E_INVALID_PARAMETERS_NUMBERS);
	    else
		sendMessage(id, getSharp(args[0]));

	    break;
	}
	/*case Q_TSHARP:{
		sendMessage(id, setTriggerSharp(id, args[0],args[1]));
		break;
	}*/
	case Q_MS:
	{
	    if(size<1)
		sendMessage(id, E_INVALID_PARAMETERS_NUMBERS);
	    else
		sendMessage(id, getMicroSwitch(args[0]));

	    break;
	}
	/*case Q_PINCE:
	{
	    if(size<2)
		sendMessage(id, E_INVALID_PARAMETERS_NUMBERS);
	    else
	    {
		sendMessage(id, setPinceState(args[0],args[1]));
		setPinceState(args[0],args[1]);
	    }

	    break;
	}*/
	case Q_SETPOSITION:
	{
	    if(size<2)
		sendMessage(id, E_INVALID_PARAMETERS_NUMBERS);
	    else
		sendMessage(id, setPincePosition(id, args[0],args[1]));

	    break;
	}
	case Q_LED:
	{
	    if(size<1)
		sendMessage(id, E_INVALID_PARAMETERS_NUMBERS);
	    else
		sendMessage(id, setLED(args[0]));

	    break;
	}
	case Q_PION:
	{
	    if(size<1)
		sendMessage(id, E_INVALID_PARAMETERS_NUMBERS);
	    else
		sendMessage(id, getPion(args[0]));

	    break;
	}
	case Q_CODEUR:
	{
	    //face
	    if(size<1)
	    sendMessage(id, E_INVALID_PARAMETERS_NUMBERS);

	    else
	    {
		if(args[0]==AVANT)
		    sendMessage(id, value_right_enc);
		else if(args[0]==ARRIERE)
		    sendMessage(id, value_left_enc);
		else
		    sendMessage(id, -1);
	    }
	    break;
	}
	case Q_RESET:
	{
	    initPinceControl();
	    //initTourelle();
	    initSensor();//en dernier a cause du jack
	    sendMessage(id, 1);
	    break;
	}
	case Q_RECALAGE:
	{
	    if(size<1)
			sendMessage(id, E_INVALID_PARAMETERS_NUMBERS);

	    else
	    {
		sendMessage(id, pinceRecal(id, args[0]));
	    }

	    break;
	}
	case Q_COLOR:
	{
	    sendMessage(id, getColor());
	    break;
	}
	case Q_ULTRAPING:
	{
	    if(size<1)
			sendMessage(id, E_INVALID_PARAMETERS_NUMBERS);
		sendMessage(id, 1);
			
		updatePosition(args[0]);
		/*
	    if(args[0]==FACEAV)
	    {
			pingArMessageID=-42;
			//pingAvMessageID=id;
			sendMessage(id, 1);
	    }
	    else if(args[0]==FACEAR)
	    {
			pingAvMessageID=-42;
			//pingArMessageID=id;
			sendMessage(id, 1);
	    }
	    else if(args[0]==-1)
	    {
			pingAvMessageID=-42;
			pingArMessageID=-42;
			sendMessage(id, 1);
	    }
	    else
	    {
			sendMessage(id, E_INVALID_PARAMETERS_TYPE);
	    }*/
	    break;
	}
	case Q_GETULTRAPING:
	{
	    if(args[0]==AVANT)
	    {
			sendMessage(id,microsecondsToCentimeters(getDistance(PIN_PING_AV)));
	    }
	    else if(args[0]==ARRIERE)
	    {
			sendMessage(id,microsecondsToCentimeters(getDistance(PIN_PING_AR)));
	    }
	    else
	    {
		sendMessage(id, E_INVALID_PARAMETERS_TYPE);
	    }
	    break;
	}
	default:
	{
	    sendMessage(id,-1);
	    break;
	}
    }
}
