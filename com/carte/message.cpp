#include "WProgram.h"
#include "message.h"

void initSerialLink(){
	Serial.begin(SERIAL_BAUD);
}

void readIncomingData(){	
	static int* buffer = (int*)malloc(16*sizeof(int));
	static int bufferIndex = 0;
/*
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
			case EOF: analyzeMessage(bufferIndex, buffer); break;
			default:
				buffer[bufferIndex] = data;
			        bufferIndex++;
			break;
		}
	}
		

}

void analyzeMessage(int bufferIndex, int* buffer){
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
			Serial.println("_______HELP__________");
			Serial.print("time: ");Serial.println(millis());
			Serial.print("Hey !! ");Serial.println();
		break;
		default: break;
	}
}


