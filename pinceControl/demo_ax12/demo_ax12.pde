#include "ax12.h"

/*
 * 
 * Sketch by Pablo Gindel : http://www.pablogindel.com/2010/01/biblioteca-de-arduino-para-ax-12/
 * Arthur (19/01/2010) : passage du serial software vers Serial, et de Serial vers Serial1 (pour Arduino Mega2560).
 * Donc, on branche les servos sur RX+TX de Serial1, et on peut continuer à utiliser l'USB normalement.
 *
 */

void setup(){
	ax12Init(1000000);
//	Serial.begin(9600);  

	writeData(14, AX_TORQUE_ENABLE, 1, 1); // 14 is the motor ID
}

void loop() {

//	int error = ping(14);
//	Serial.print("ping ID: "); Serial.print(status_id, DEC);
//	Serial.print(" int.error: "); Serial.print(error, DEC);
//	Serial.print(" status.error: "); Serial.println(status_error, DEC);
//	Serial.println(" ");
//
//	error = readData(14, AX_PRESENT_TEMPERATURE, 1);
//	Serial.print("temperatura: "); Serial.print(status_data, DEC);
//	Serial.print(" int.error: "); Serial.print(error, DEC);
//	Serial.print(" status.error: "); Serial.println(status_error, DEC);
//	Serial.println(" ");
//
//	error = writeData(14, AX_GOAL_POSITION_L, 2, int(random(1024)));
//	Serial.print("goal position ID: "); Serial.print(status_id, DEC);
//	Serial.print(" int.error: "); Serial.print(error, DEC);
//	Serial.print(" status.error: "); Serial.println(status_error, DEC);
//	Serial.println(" ");
//
//	delay(200);
//
//	error = readData(14, AX_PRESENT_POSITION_L, 2);
//	Serial.print("posicion: "); Serial.print(status_data, DEC);
//	Serial.print(" int.error: "); Serial.print(error, DEC);
//	Serial.print(" status.error: "); Serial.println(status_error, DEC);
//	Serial.println(" ");
}

