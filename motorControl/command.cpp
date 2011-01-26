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

// Message est le tableau de message
// m est le message
void cmd(int c, int* message)
{
	int i;
	// On analyse le message en fonction de son type
	switch(c){
		case 'x':
			Serial.print("x,");Serial.print(message[0]);Serial.print("§");Serial.print(message[1]);Serial.print("§");Serial.print(message[2]);Serial.println();
		break;
		case 'P': // Ping
			sendMessage(c, "Pong");
		break;
		case 'I': // Identification
			sendMessage(c, "Asserv Board");
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
		case 'G':
			pushGoal(TYPE_POSITION, message[0]*18, message[1]*18, message[2]);
			//x , y , ratio vitesse max [0-200]
			sendMessage(c, "Go...");
		break;
		case 'g':
			pushGoal(TYPE_POSITION, message[0]*18+robot_state.x*ENC_TICKS_TO_MM, message[1]*18+robot_state.y*ENC_TICKS_TO_MM, message[2]);
			//x , y , ratio vitesse max [0-200]
			sendMessage(c, "Go... relatif");
		break;
		//case T_SPEED:
		case 's':
			pushGoal(TYPE_SPEED, message[0], message[1], 0);
			sendMessage(c, "Right...");
		break;
		//case T_ANGLE:
		case 'A':
			pushGoal(TYPE_ANGLE, message[0], message[1], message[2]/360 * 2*M_PI); // x,y,angle
			sendMessage(c, "Tourne...");
		break;
		case 'a':
			pushGoal(TYPE_ANGLE, message[0], message[1], moduloPI(message[2]/360 * 2*M_PI + robot_state.angle)); // x,y,angle
			sendMessage(c, "Tourne... relatif");
		break;
		//case T_RESET:
		case 'r':
			clearGoals();
			sendMessage(c, "The fifo is empty...");
		break;
		case 'p':
			Serial.print("p,");
			Serial.print(robot_state.x*ENC_TICKS_TO_MM);
			Serial.print(" ");
			Serial.print(robot_state.y*ENC_TICKS_TO_MM);
			Serial.print(" ");
			Serial.print(robot_state.angle*360/(2*M_PI));
			Serial.println();

		case '?':
			Serial.print("?,_________________§");
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
			sendMessage(c,"Command not found");
		break;
	}
}
