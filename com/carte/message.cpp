#include "WProgram.h"
#include "message.h"
#include "getters.h"



void initSerialLink(){
	Serial.begin(SERIAL_BAUD);
}

void readIncomingData(){	
	static int buffer[16];
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
			case '<': bufferIndex = 0; break;
			case '>': analyzeMessage(bufferIndex, buffer); break;
			default:
				buffer[bufferIndex] = data;
			        bufferIndex++;
			break;
		}
	}
		

}

int checksum(int size, int* buffer) {
	int i, check=0;
	for(i=0; i<size-1; i++) {
		check+=buffer[i];	
	}
	return check % 128;
}

void analyzeMessage(int bufferIndex, int* buffer){
	int check = checksum(bufferIndex, buffer);
	Serial.println("_______DEBUG__________");
			Serial.print("time: ");Serial.println(millis());
			Serial.println();
			Serial.println("1. Checksum : ");
			Serial.print("calculate : ");Serial.println(check);
			Serial.print("get : ");Serial.println(buffer[bufferIndex-1]);
			Serial.println();
	// Si le checksum est pas bon, on stop net
	if(check != buffer[bufferIndex-1]) {
		return;
	}
	
	
	// On analyse le message en fonction de son type
	switch(buffer[0]){
		case 'S':
			Serial.println("_______SHARP__________");
			getSharp();
		break;
		case '?':
			Serial.println("_______HELP__________");
			Serial.print("time: ");Serial.println(millis());
			Serial.print("Hey !! ");Serial.println();
		break;
		default: break;
	}
}


