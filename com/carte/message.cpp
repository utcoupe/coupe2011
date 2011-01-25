#include "WProgram.h"
#include "message.h"
#include "getters.h"


void initSerialLink(){
	Serial.begin(SERIAL_BAUD);
}

void readIncomingData(){	
	static unsigned char buffer[100];
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

void analyzeMessage(int bufferIndex, unsigned char* buffer){
	int i, j, lasti=0;
	int message[50];
	int m = 0;
	message[0] = buffer[0];
	message[1] = 0;
	m=1;
	lasti=2;
	for (i=2; i<bufferIndex; i++) {
		if(buffer[i]==' ' or i==(bufferIndex-1)) {
			for(j=lasti; j<i; j++) {
				message[m] = (buffer[j]-48)+(message[m]*10);
			}
			m++;
			message[m] = 0;
			lasti = i + 1;
		}
	}


	// On analyse le message en fonction de son type
	switch(message[0]){
		case 'E':
			for(i=0;i<bufferIndex; i++) {
				Serial.print(i);
				Serial.print(" : ");
				Serial.print(buffer[i]);
				Serial.print("§");
			}
			for (i=1;i<m;i++) {
				Serial.print("§message ");Serial.print(i);Serial.print(" ");Serial.print(message[i]); 
			}
				Serial.print("\n");
		break;
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
			Serial.print("time: ");Serial.print(millis());
			Serial.print("Hey !! ");Serial.print("\n");
		break;
		default:
			Serial.print(-1);Serial.print("\n");
		break;
	}
}


