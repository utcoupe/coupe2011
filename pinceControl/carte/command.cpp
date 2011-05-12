#include "command.h"

/* Analyse le message et effectue les actions associees
 * 	<> id : l'identifiant associe au message
 * 	<> header : le type de message (en-tete)
 * 	<> args : le tableau d'entier contenant les arguments
 * */

// from, id_cmd, *args, sizeArgs
void cmd(int id, int header, int *args, int size){
	/* On analyse le message en fonction de son type */
	switch(header){
		case Q_IDENT:{//ok
			sendMessage(id, (char*)"pince");
			break;
		}
		case Q_PING:{//ok
			sendMessage(id, (char*)"Pong");
			break;
		}
		case Q_SHARP:{//ok
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
		case Q_MS:{//ok
		    if(size<1)
		        sendMessage(id, E_INVALID_PARAMETERS_NUMBERS);

		    else
                sendMessage(id, getMicroSwitch(args[0]));

			break;
		}
		case Q_TMS:{
		    if(size<2)
		        sendMessage(id, E_INVALID_PARAMETERS_NUMBERS);

		    else
                sendMessage(id, setTriggerMS(id, args[0],args[1]));

			break;
		}
		case Q_PINCE:{ //ok
		    if(size<2)
		        sendMessage(id, E_INVALID_PARAMETERS_NUMBERS);
		    else
		    {
                sendMessage(id, setPinceState(args[0],args[1]));
                setPinceState(args[0],args[1]);
			}

			break;
		}
		case Q_SETPOSITION:{//ok
		    if(size<2)
		        sendMessage(id, E_INVALID_PARAMETERS_NUMBERS);

		    else
                sendMessage(id, setPincePosition(id, args[0],args[1]));

			break;
		}
		case Q_LOAD:{
		    if(size<1)
		        sendMessage(id, E_INVALID_PARAMETERS_NUMBERS);

		    else
                sendMessage(id, presentLoad(args[0]));

			break;
		}
		case Q_JACK:{
			sendMessage(id, 1);
			sendMessage(id, waitJack());
			break;
		}
		case Q_LED:{
		    if(size<1)
		        sendMessage(id, E_INVALID_PARAMETERS_NUMBERS);

		    else
                sendMessage(id, setLED(args[0]));

			break;
		}
		case Q_PION:{
		    if(size<1)
		        sendMessage(id, E_INVALID_PARAMETERS_NUMBERS);

		    else
                sendMessage(id, getPion(args[0]));

			break;
		}
		case Q_MOTOR:{
		    if(size<1)
		        sendMessage(id, E_INVALID_PARAMETERS_NUMBERS);

		    else
                sendMessage(id, setPincePWM(1,args[0],1000));

			break;
		}
		case Q_TXAX12:{
			//id ax12_read_cmd value
			if(size<3)
		        sendMessage(id, E_INVALID_PARAMETERS_NUMBERS);

		    else
                sendMessage(id, Ax12.write(args[0],args[1],args[2]));

			break;
		}
		case Q_RXAX12:{
			//id ax12_read_cmd
			if(size<2)
		        sendMessage(id, E_INVALID_PARAMETERS_NUMBERS);

		    else
                sendMessage(id, Ax12.read(args[0],args[1]));

			break;
		}
		case Q_CODEUR:{//ok
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
		case Q_RESET:{ //ok
			initPinceControl();
			//initTourelle();
			initSensor();//en dernier a cause du jack
			sendMessage(id, 1);
			break;
		}
		case Q_RECALAGE:{
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
		case Q_PING:
		{
			if(args[0]==FACEAV){
				pingAvMessageID=id;
				sendMessage(id, 1);
			}
			if(args[0]==FACEAR){
				pingAvrMessageID=id;
				sendMessage(id, 1);
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
