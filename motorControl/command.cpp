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
		case 'x':
		{
			pushGoalOrientation(1.5,200);
			Serial.print("x,");
			Serial.print(message[0]);
			Serial.print("§");
			Serial.print(message[1]);
			Serial.print("§");
			Serial.print(message[2]);
			Serial.println();
			break;
		}

		case 'P': // Ping
		{
			sendMessage(c, "Pong");
			break;
		}

		case 'I': // Identification
		{
			sendMessage(c, "Asserv Board");
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
			current_goal.x = message[0]*18;
			current_goal.y = message[1]*18;
			current_goal.speed = message[2];
			sendMessage(c, "Modification absolue...");
			break;
		}

		case 'm':
		{
			double co = cos(robot_state.angle);
			double si = sin(robot_state.angle);

			current_goal.type = TYPE_POSITION;
			current_goal.isReached = false;
			current_goal.x = (message[0]*co-message[1]*si)*18+robot_state.x;
			current_goal.y = (message[0]*si+message[1]*co)*18+robot_state.y;
			current_goal.speed = message[2];
			sendMessage(c, "Modification relative...");
			break;
		}

		case 'G':
		{
			pushGoalPosition(message[0]*18, message[1]*18, (double)message[2]);
			sendMessage(c, "Asserv en position absolue...");
			break;
		}

		case 'g':
		{
			double co = cos(robot_state.angle);
			double si = sin(robot_state.angle);

			pushGoalPosition((message[0]*co-message[1]*si)*18+robot_state.x, (message[0]*si+message[1]*co)*18+robot_state.y, (double)message[2]);
			/*Serial.print("§X : "); Serial.print(message[0]*co-message[1]*si+robot_state.x*ENC_TICKS_TO_MM);
			Serial.print("§Y : "); Serial.print(message[1]*co+message[1]*si+robot_state.y*ENC_TICKS_TO_MM);
			Serial.print("§ ...");Serial.println()*/
			sendMessage(c, "Asserv en position relative...");
			break;
		}

		case 's':
		{
			pushGoalSpeed(message[0],message[1]); /* TODO un doute sur l'ordre d'envoi des arguments */
			sendMessage(c, "Asserv en vitesse...");
			break;
		}

		case 'A':
		{
			double angle = message[0]/180.0 * M_PI;
			pushGoalOrientation(angle,message[1]);
			//sendMessage(c, angle);
			sendMessage(c, "Asserv en orientation absolue...");
			break;
		}

		case 'a':
		{
			double angle = moduloPI(message[0]/180.0 * M_PI + robot_state.angle);
			pushGoalOrientation(angle,message[1]);
			//sendMessage(c, moduloPI((double)message[0]/180 * M_PI + robot_state.angle));
			sendMessage(c, "Asserv en orientation relative...");
			break;
		}

		case 'd':
		{
			pushGoalDelay(message[0]);
			sendMessage(c, "En attente");
			break;
		}

		case 'r':
		{
			clearGoals();
			current_goal.isCanceled = true;
			sendMessage(c, "Arret d'urgence...");
			break;
		}

		case 'p':
		{
			Serial.print("p,");
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
			Serial.print("§y: ");Serial.print(robot_state.y*ENC_TICKS_TO_MM, DEC);
			/*Serial.print("pwmL: ");Serial.println(value_pwm_left);
			Serial.print("pwmR: ");Serial.println(value_pwm_right);*/
			Serial.print("§encL: ");Serial.print(value_left_enc);
			Serial.print("§encR: ");Serial.println(value_right_enc);
			break;
		}

		default:
		{
			sendMessage(c,"Command not found");
			break;
		}
	}
}
