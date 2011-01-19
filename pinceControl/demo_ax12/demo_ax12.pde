/*
 * 
 * Sketch by Pablo Gindel : http://www.pablogindel.com/2010/01/biblioteca-de-arduino-para-ax-12/
 * Arthur (19/01/2010) : passage du serial software vers Serial, et de Serial vers Serial1 (pour Arduino Mega2560).
 * Donc, on branche les servos sur RX+TX de Serial1, et on peut continuer à utiliser l'USB normalement.
 *
 */
 
#include "ax12.h"
/* Si on utilise le Serial fourni par Arduino, le fichier HardwareSerialBis.h va être inclus,
   ce qui provoque un conflit avec la librairie AX-12, qui utilise son propre ISR sur UART1.
   SerialBis.cpp est donc identique au HardwareSerialBis.cpp d'Arduino, sauf qu'il n'y a pas de
   SerialBis1. Tout doit être renommé, pour que le fichier d'origine ne soit pas inclus. */
#include "SerialBis.h"

void setup(){
	ax12Init(1000000);
	SerialBis.begin(115200);  

	writeData(14, AX_TORQUE_ENABLE, 1, 1); // 14 is the motor ID
}

void loop() {

	int error = ping(14);
	SerialBis.print("ping ID: "); SerialBis.print(status_id, DEC);
	SerialBis.print(" int.error: "); SerialBis.print(error, DEC);
	SerialBis.print(" status.error: "); SerialBis.println(status_error, DEC);
	SerialBis.println(" ");

	error = readData(14, AX_PRESENT_TEMPERATURE, 1);
	SerialBis.print("temperatura: "); SerialBis.print(status_data, DEC);
	SerialBis.print(" int.error: "); SerialBis.print(error, DEC);
	SerialBis.print(" status.error: "); SerialBis.println(status_error, DEC);
	SerialBis.println(" ");

	error = writeData(14, AX_GOAL_POSITION_L, 2, int(random(1024)));
	SerialBis.print("goal position ID: "); SerialBis.print(status_id, DEC);
	SerialBis.print(" int.error: "); SerialBis.print(error, DEC);
	SerialBis.print(" status.error: "); SerialBis.println(status_error, DEC);
	SerialBis.println(" ");

	delay(200);

	error = readData(14, AX_PRESENT_POSITION_L, 2);
	SerialBis.print("posicion: "); SerialBis.print(status_data, DEC);
	SerialBis.print(" int.error: "); SerialBis.print(error, DEC);
	SerialBis.print(" status.error: "); SerialBis.println(status_error, DEC);
	SerialBis.println(" ");
}

