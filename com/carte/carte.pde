#include "blink.h"
/*
  Petit test !
 */



void setup() {
 	Serial.begin(115200); 
	pinMode(12, OUTPUT); // Rouge   
	pinMode(13, OUTPUT); // Vert
}

void loop() {
  	int sensorValue = analogRead(A0);
  	Serial.println(sensorValue, DEC);
	blinkgreen();
}
