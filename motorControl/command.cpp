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

/* Analyse le message et effectue les actions associees
 * 	<> id : l'identifiant associe au message
 * 	<> header : le type de message (en-tete)
 * 	<> args : le tableau d'entier contenant les arguments
 * */
void cmd(int id, int header, int* args){
                        
	/* On analyse le message en fonction de son type */
	switch(header){

		case Q_POSITION:
		{
			int x_mm = robot_state.x*ENC_TICKS_TO_MM;
			int y_mm = robot_state.y*ENC_TICKS_TO_MM;
			int a_deg = robot_state.angle*180.0 / M_PI;
			int tab[] = {x_mm,y_mm,a_deg};
			sendMessage(id,tab,3);
	        break;
		}

		case Q_MODIF_GOAL_ABS:
		{
			current_goal.type = TYPE_POSITION;
			current_goal.isReached = false;
			current_goal.x = (double)args[0]*ENC_MM_TO_TICKS;
			current_goal.y = (double)args[1]*ENC_MM_TO_TICKS;
			current_goal.speed = args[2];
			sendMessage(id, 1);
			break;
		}

		case Q_MODIF_GOAL_REL:
		{
			double co = cos(robot_state.angle);
			double si = sin(robot_state.angle);

			current_goal.type = TYPE_POSITION;
			current_goal.isReached = false;
			current_goal.x = (args[0]*co-args[1]*si)*18+robot_state.x;
			current_goal.y = (args[0]*si+args[1]*co)*18+robot_state.y;
			current_goal.speed = args[2];
			sendMessage(id, 1);
			break;
		}

		case Q_GOAL_ABS:
		{
			pushGoalPosition(id,(double)args[0]*ENC_MM_TO_TICKS, (double)args[1]*ENC_MM_TO_TICKS, (double)args[2]);
			sendMessage(id, 1);
			break;
		}

		case Q_GOAL_REL:
		{
			double co = cos(robot_state.angle);
			double si = sin(robot_state.angle);

			pushGoalPosition(id,((double)args[0]*co-(double)args[1]*si)*18+robot_state.x, ((double)args[0]*si+(double)args[1]*co)*18+robot_state.y, (double)args[2]);
			sendMessage(id, 1);
			break;
		}

		case Q_ANGLE_ABS:
		{
			double angle = args[0]/180.0 * M_PI;
			pushGoalOrientation(id,angle,args[1]);
			sendMessage(id, 1);
			break;
		}

		case Q_ANGLE_REL:
		{
			double angle = moduloPI(args[0]/180.0 * M_PI + robot_state.angle);
			pushGoalOrientation(id,angle,args[1]);
			sendMessage(id, 1);
			break;
		}

		case Q_DELAY:
		{
			pushGoalDelay(args[0]);
			sendMessage(id, 1);
			break;
		}

		case Q_PWM:
		{
			pushGoalPwm(args[0],args[1]);
			sendMessage(id, 1);
			break;
		}

		case Q_PAUSE: /* comme pause */
		{
			current_goal.isPaused = true;
			sendMessage(id, 1);
			break;
		}

		case Q_RESUME: /* comme resume */
		{
			current_goal.isPaused = false;
			sendMessage(id, 1);
			break;
		}

		case Q_STOP: /* comme stop */
		{
			clearGoals();
			current_goal.isCanceled = true;
			sendMessage(id, 1);
			break;
		}

		case Q_MANUAL_CALIB : //TODO a eclater en calibration manuel de l'angle ,de x et de y
		{
			pushGoalManualCalibration(TYPE_CALIB_X, args[0]);
			pushGoalManualCalibration(TYPE_CALIB_Y, args[1]);
			pushGoalManualCalibration(TYPE_CALIB_ANGLE, args[2]);
			sendMessage(id, 1);
			break;
		}

		case Q_AUTO_CALIB :
		{
			if(args[0] == 0){
				pushGoalAutoCalibration(id,true);
			}
			else{
				pushGoalAutoCalibration(id,false);
			}

			sendMessage(id, 1);
			break;
		}

		case Q_PING:
		{
			sendMessage(id, "Pong");
			break;
		}

		case Q_IDENT: /* Identification */
		{
			sendMessage(id, "asserv");
			break;
		}

		case Q_DEBUG : //TODO a degager quand tout marche
		{
			Serial.print("?,_________________§");
			Serial.print("uptime: ");Serial.print(millis());
			Serial.print("§angle: ");Serial.print(robot_state.angle, DEC);
			Serial.print("§speed: ");Serial.print(robot_state.speed*ENC_TICKS_TO_MM, DEC);
			Serial.print("§speedt: ");Serial.print(robot_state.speed, DEC);
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
			sendMessage(id,0);
			break;
		}
	}
}
