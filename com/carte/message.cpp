#include "WProgram.h"
#include "message.h"
#include "command.h"



using namespace std;


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

///
/// Envoie un int
///
void sendMessage(char cmd, int i)
{
	Serial.print(cmd);
	Serial.print(",");
	Serial.println(i);
}
///
/// Envoie un tableau d'int
///
void sendMessage(char cmd, int *tabi, int size)
{
	Serial.print(cmd);
	Serial.print(",");
	for (int i=0; i<size-1; ++i)
	{
		Serial.print(tabi[i]);
		Serial.print(' ');
	}
	Serial.println(tabi[size-1]);
}
///
/// Envoie un string
///
void sendMessage(unsigned char cmd, char* str)
{
	Serial.print(cmd);
	Serial.print(",");
	Serial.println(str);
}
///
/// Envoie des strings et des int
/// aucune protection, il faut au moins envoyer une chaine et un int
///
void sendMessage(unsigned char cmd, char** tabs, int nbStr, int *tabi, int nbInt)
{
	Serial.print(cmd);
	Serial.print(",");
	
	for (int i=0; i<nbStr; ++i)
	{
		Serial.print(tabs[i]);
		Serial.print(' ');
	}
	
	for (int i=0; i<nbInt-1; ++i)
	{
		Serial.print(tabi[i]);
		Serial.print(' ');
	}
	Serial.println(tabi[nbInt-1]);
}
///
/// Envoie des int et des strings
/// aucune protection, il faut au moins envoyer une chaine et un int
///
void sendMessage(unsigned char cmd, int* tabi, int nbInt, char** tabs, int nbStr)
{
	Serial.print(cmd);
	Serial.print(",");
	
	for (int i=0; i<nbInt; ++i)
	{
		Serial.print(tabi[i]);
		Serial.print(' ');
	}
	
	for (int i=0; i<nbStr-1; ++i)
	{
		Serial.print(tabs[i]);
		Serial.print(' ');
	}
	Serial.println(tabs[nbStr-1]);
}


void analyzeMessage(int bufferIndex, unsigned char* buffer){
	int i, j, lasti=0;
	float message[50];
	int m = 0, neg = 1;
	message[0] = buffer[0];
	message[1] = 0;
	m=1;
	lasti=2;
	for (i=2; i<=bufferIndex; i++) {
		if(buffer[i]==' ' or i==(bufferIndex)) {
			for(j=lasti; j<i; j++) {
				if(buffer[j]=='-') { 
					neg = -1; 
				} else {
					message[m] = ((float) buffer[j]-'0')+(message[m]*10);
				}
			}
			if (neg<0) { message[m] *= -1; }
			m++;
			message[m] = 0;
			neg = 1;
			lasti = i + 1;
		}
	}
	// CALL command.cpp (Don't forget this file is generic for all our arduino board)
	cmd(buffer[0], message, m);

}


