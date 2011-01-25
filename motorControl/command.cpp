#include "WProgram.h"
#include "command.h"
#include "parameters.h"
#include "robotstate.h"
#include "control.h"
#include "fifo.h"
#include "pwm.h"
#include "control.h"
#include "encoder.h"

// Message est le tableau de message
// m est le message
void cmd(int c, float* message, int m)
{
	int i;
	// On analyse le message en fonction de son type
	switch(c){
		/*
			DEBUG ZONE

			Ces fonctions permettent entre autre le debug...
		*/
		case 'E': // Echo (renvoit le message qu'elle a reçu)
			for (i=1;i<m;i++) {
				Serial.print("§message ");Serial.print(i);Serial.print(" ");Serial.print(message[i]); 
			}
				Serial.print("\n");
		break;
		case 'P': // Ping (renvoit time : pong)
			Serial.print(millis());Serial.println(" : Pong");
		break;
		case 'I': // Identification
			identification();
		break;

		/* A gérer :
		#define T_PAUSE 4
		#define T_RESET 6
		#define T_ALIGN 7
		#define T_SETX 9
		#define T_SETY 10
		*/ 
		case T_QUERY:
	        Serial.print(SOF);
			Serial.print(robot_state.x, DEC);
			Serial.print(robot_state.y, DEC);
			Serial.print(robot_state.angle, DEC);
			//Serial.print(checksum);
	                    Serial.print(EOF);
		break;
		//case T_POSITION:
		case 'g':
			pushGoal(TYPE_POSITION, message[1], message[2], message[3]);
			//x , y , ratio vitesse max [0-200]
			Serial.println("Youha !!");
		break;
		//case T_SPEED:
		case 's':
			pushGoal(TYPE_SPEED, message[1], message[2], 0);
			Serial.println("Tout droit...");
		break;
		//case T_ANGLE:
		case 'a':
			pushGoal(TYPE_ANGLE, message[1], message[2], message[3]/360 * 2*M_PI); // x,y,angle
			Serial.print(message[3]);Serial.print(" -> ");Serial.print(message[3]/360.0 * 2*M_PI);Serial.println("Tourne cocotte!!");
		break;
		//case T_RESET:
		case 'r':
			clearGoals();
			Serial.println("J'ai plus de but dans la vie..");
		break;
		case '?':
			Serial.print("_________________§");
			Serial.print("time: ");Serial.print(millis());
			Serial.print("§angle: ");Serial.print(robot_state.angle, DEC);
			Serial.print("§speed: ");Serial.print(robot_state.speed*ENC_TICKS_TO_MM, DEC);
			Serial.print("§x: ");Serial.print(robot_state.x*ENC_TICKS_TO_MM, DEC);
			Serial.print("§y: ");Serial.print(robot_state.y*ENC_TICKS_TO_MM, DEC);
			/*Serial.print("pwmL: ");Serial.println(value_pwm_left);
			Serial.print("pwmR: ");Serial.println(value_pwm_right);*/
			Serial.print("§encL: ");Serial.print(value_left_enc);
			Serial.print("§encR: ");Serial.println(value_right_enc);
		break;
		// If not found
		default:
			Serial.println("Command not found");
		break;
	}
}

void identification()
{
	Serial.println("Asserv Board");
}
