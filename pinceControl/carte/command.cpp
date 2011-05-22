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
		sendMessage(id, (char*)"pince");
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
	case Q_TMS:
	{
	    if(size<2)
		sendMessage(id, E_INVALID_PARAMETERS_NUMBERS);
	    else
		sendMessage(id, setTriggerMS(id, args[0],args[1]));

	    break;
	}
	case Q_PINCE:
	{
	    if(size<2)
		sendMessage(id, E_INVALID_PARAMETERS_NUMBERS);
	    else
	    {
		sendMessage(id, setPinceState(args[0],args[1]));
		setPinceState(args[0],args[1]);
	    }

	    break;
	}
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
	case Q_MOTOR:
	{
	    if(size<1)
			sendMessage(id, E_INVALID_PARAMETERS_NUMBERS);
	    else
			sendMessage(id, setPincePWM(1,args[0],1000));

	    break;
	}
	case Q_CODEUR:
	{
	    //face
	    if(size<1)
	    sendMessage(id, E_INVALID_PARAMETERS_NUMBERS);

	    else
	    {
		if(args[0]==PINCEAV)
		    sendMessage(id, value_right_enc);
		else if(args[0]==PINCEAR)
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
		sendMessage(id, 1);
		sendMessage(id, pinceRecal(args[0]));
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
	    if(args[0]==FACEAV){
		pingAvMessageID=id;
		sendMessage(id, 1);
	    }else if(args[0]==FACEAR){
		pingArMessageID=id;
		sendMessage(id, 1);
	    }else if(args[0]==-1){
		pingAvMessageID=-42;
		sendMessage(id, 1);
	    }else if(args[0]==-2){
		pingArMessageID=-42;
		sendMessage(id, 1);
	    }else{
		sendMessage(id, E_INVALID_TYPE_PARAMETERS);
	    }
	    break;
	}
	case Q_GETULTRAPING:
	{
	    if(args[0]==FACEAV){
		    sendMessage(id,microsecondsToCentimeters(getDistance(PIN_PING_AV)));
	    }else if(args[0]==FACEAR){
		    sendMessage(id,microsecondsToCentimeters(getDistance(PIN_PING_AR)));
	    }else{
		    sendMessage(id, E_INVALID_TYPE_PARAMETERS);
	    }
	    break;
	}
	default:{
	    sendMessage(id,-1);
	    break;
	}
    }
}
