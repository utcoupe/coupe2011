#include "command.h"

void setup(){	
	delay(1000);
	
	initSerialLink();
	initSensor();
	initPinceControl();
	initTourelle();
	
}

void loop(){
	
	readIncomingData();
	sensorTrigger();
	pinceContolRun();
	tourelleRun();
	
}


