#include "WProgram.h"
#include "command.h"
#include "parameters.h"
#include "robotstate.h"
#include "control.h"
#include "fifo.h"
#include "pwm.h"
#include "control.h"
#include "encoder.h"
#include "message.h"

/* Analyse le message parse auparavant et effectue les actions associees
 * 	<> c : l'entete du message (indique le type de message)
 * 	<> message : le tableau d'entier contenant les arguments
 * */
void cmd(int header, int* args){
                        
	/* On analyse le message en fonction de son type */
	switch(header){
		case 'i' :
		{
			pushGoalManualCalibration(TYPE_CALIB_X, 0);
			pushGoalManualCalibration(TYPE_CALIB_Y, 0);
			pushGoalManualCalibration(TYPE_CALIB_ANGLE, 0);
			break;
		}

		case 'x' :
		{
			pushGoalAutoCalibration();
			sendMessage(header, "Calibration de l'odometrie en cours...");
			break;
		}

		case 'P': // Ping
		{
			sendMessage(header, "Pong");
			break;
		}

		case 'I': // Identification
		{
			sendMessage(header, "asserv");
			break;
		}

		case T_QUERY:
		{
	        Serial.print(SOF);
			Serial.print(robot_state.x, DEC);
			Serial.print(robot_state.y, DEC);
			Serial.print(robot_state.angle, DEC);
	        Serial.print(EOF);
	        break;
		}

		case 'M':
		{
			current_goal.type = TYPE_POSITION;
			current_goal.isReached = false;
			current_goal.x = args[0]*18;
			current_goal.y = args[1]*18;
			current_goal.speed = args[2];
			sendMessage(header, "Modification absolue...");
			break;
		}

		case 'm':
		{
			double co = cos(robot_state.angle);
			double si = sin(robot_state.angle);

			current_goal.type = TYPE_POSITION;
			current_goal.isReached = false;
			current_goal.x = (args[0]*co-args[1]*si)*18+robot_state.x;
			current_goal.y = (args[0]*si+args[1]*co)*18+robot_state.y;
			current_goal.speed = args[2];
			sendMessage(header, "Modification relative...");
			break;
		}

		case 'G':
		{
			pushGoalPosition(args[0]*18, args[1]*18, args[2]);
			sendMessage(header, "Asserv en position absolue...");
			break;
		}

		case 'g':
		{
			double co = cos(robot_state.angle);
			double si = sin(robot_state.angle);

			pushGoalPosition((args[0]*co-args[1]*si)*18+robot_state.x, (args[0]*si+args[1]*co)*18+robot_state.y, args[2]);
			sendMessage(header, "Asserv en position relative...");
			break;
		}

		case 'v':
		{
			pushGoalSpeed(args[0],args[1]); /* TODO un doute sur l'ordre d'envoi des arguments */
			sendMessage(header, "Asserv en vitesse...");
			break;
		}

		case 'A':
		{
			double angle = args[0]/180.0 * M_PI;
			pushGoalOrientation(angle,args[1]);
			sendMessage(header, "Asserv en orientation absolue...");
			break;
		}

		case 'a':
		{
			double angle = moduloPI(args[0]/180.0 * M_PI + robot_state.angle);
			pushGoalOrientation(angle,args[1]);
			sendMessage(header, "Asserv en orientation relative...");
			break;
		}

		case 'd':
		{
			pushGoalDelay(args[0]);
			sendMessage(header, "En attente");
			break;
		}

		case 'w':
		{
			pushGoalPwm(args[0],args[1]);
			sendMessage(header, "Application de la pwm...");
			break;
		}

		case 'p': /* comme pause */
		{
			current_goal.isPaused = true;
			sendMessage(header, "En pause...");
			break;
		}

		case 'r': /* comme resume */
		{
			current_goal.isPaused = false;
			sendMessage(header, "C'est reparti...");
			break;
		}

		case 's': /* comme stop */
		{
			clearGoals();
			current_goal.isCanceled = true;
			sendMessage(header, "Arret d'urgence...");
			break;
		}

		case 'k': /* position actuelle */
		{
			Serial.print("k,");
			Serial.print(robot_state.x*ENC_TICKS_TO_MM);
			Serial.print(" ");
			Serial.print(robot_state.y*ENC_TICKS_TO_MM);
			Serial.print(" ");
			Serial.print(robot_state.angle*180/M_PI);
			Serial.println();
			break;
		}

		case '?':
		{
			Serial.print("?,_________________§");
			Serial.print("uptime: ");Serial.print(millis());
			Serial.print("§angle: ");Serial.print(robot_state.angle, DEC);
			Serial.print("§speed: ");Serial.print(robot_state.speed*ENC_TICKS_TO_MM, DEC);
			Serial.print("§x: ");Serial.print(robot_state.x*ENC_TICKS_TO_MM, DEC);
			Serial.print("§xt: ");Serial.print(robot_state.x, DEC);

			Serial.print("§y: ");Serial.print(robot_state.y*ENC_TICKS_TO_MM, DEC);
			Serial.print("§yt: ");Serial.print(robot_state.y, DEC);

			Serial.print("§encL: ");Serial.print(value_left_enc);
			Serial.print("§encR: ");Serial.println(value_right_enc);
			break;
		}

		default:
		{
			sendMessage(header,"Command not found");
			break;
		}
	}
}
