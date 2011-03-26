#include "command.h"

void setup(){	
	initSerialLink();
	initSensor();
	initPinceControl();
	initTourelle();
}

void loop(){
	readIncomingData();
	sensorTrigger();
	tourelleRun();
	
}


