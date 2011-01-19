/*
 * message.cpp
 *
 *  Created on: 13 janv. 2011
 *      Author: HoHen
 */

#include "WProgram.h"
#include "message.h"

int* buffer;

void initSerialLink(){
	Serial.begin(115200);
	buffer = (int*)malloc(16*sizeof(int));
}

void readIncomingData(){
	int available = Serial.available();
	// Euh, faudrait les stocker à part en fait ! Genre buffer[i], en définissant un SIZE_MAX
	for(int i = 0; i < available; i ++)
		buffer[0] += Serial.read();

}

void analyzeMessage(){

}


