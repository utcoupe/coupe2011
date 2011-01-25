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

void analyzeMessage(int bufferIndex, int* buffer){
	
	// On analyse le message en fonction de son type
	switch(buffer[0]){
		case 'S':
			getSharp();
		break;
		case 'L':
			ledOn();
		break;
		case 'l':
			ledOff();
		break;
		case '?':
			Serial.print("time: ");Serial.println(millis());
			Serial.print("Hey !! ");Serial.println();
		break;
		default:
			Serial.println(-1);
		break;
	}
}


