/*
 * message.cpp
 *
 *  Created on: 13 janv. 2011
 *      Author: HoHen
 */

#include "WProgram.h"
#include "message.h"
#include "robotstate.h"
#include "fifo.h"
#include "encoder.h"

int* buffer;
int bufferIndex = 0;

void initSerialLink(){
	Serial.begin(9600);
	buffer = (int*)malloc(16*sizeof(int));
}

void readIncomingData(){
/*	// Cédric, faut que tu m'expliques lui, pourquoi tu fais un 
	// += Serial.read() toujours dans la même case du tableau ?
	// Je pense que j'ai loupé un truc ^^
	int available = Serial.available();
	for(int i = 0; i < available; i ++)
		buffer[0] += Serial.read();
		
		
	A propos du protcole :
	- un message commence par < et se termine par >
	- le format des trames est dans Spec_protocole.pdf
*/
	// S'il y a des données à lire
	int available = Serial.available();
	for(int i = 0; i < available; i ++){
		int data = Serial.read();
		
		switch(data){
			case SOF: bufferIndex = 0; break;
			case EOF: analyzeMessage(); break;
			default:
				buffer[bufferIndex] = data;
			        bufferIndex++;
			break;
		}
	}
		

}

void analyzeMessage(){
	/*
	Contenu du buffer par index
		1			type
		2:index-2	données
		index-1		checksum
	*/
	// Si le checksum est pas bon, on stop net
	// TODO
	
	
	// On analyse le message en fonction de son type
	switch(buffer[0]){
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
		case T_POSITION:
			//pushGoal(TYPE_POSITION, buffer[2] << 8 + buffer[3], buffer[3] << 8 + buffer[4]);
		break;
		case T_SPEED:
			//pushGoal(TYPE_SPEED, buffer[2], 0);
		break;
		case T_ANGLE:
			//pushGoal(TYPE_ANGLE, buffer[2] << 8 + buffer[3], 0);
		break;
		case '?':
			Serial.println("_________________");
			Serial.print("time: ");Serial.println(millis());
			Serial.print("angle: ");Serial.println(robot_state.angle, DEC);
			Serial.print("speed: ");Serial.println(robot_state.speed*ENC_TICKS_TO_MM, DEC);
			Serial.print("x: ");Serial.println(robot_state.x*ENC_TICKS_TO_MM, DEC);
			Serial.print("y: ");Serial.println(robot_state.y*ENC_TICKS_TO_MM, DEC);
			/* Serial.print("pwmL: ");Serial.println(value_pwm_left);
			Serial.print("pwmR: ");Serial.println(value_pwm_right);*/
			Serial.print("encL: ");Serial.println(value_left_enc);
			Serial.print("encR: ");Serial.println(value_right_enc);
		break;
		default: break;
	}
}


