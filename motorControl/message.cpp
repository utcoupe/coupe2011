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
	Serial.begin(9600);
	buffer = (int*)malloc(16*sizeof(int));
}

void readIncomingData(){
	int available = Serial.available();
	for(int i = 0; i < available; i ++)
		buffer[0] += Serial.read();

}

void analyzeMessage(){

}


