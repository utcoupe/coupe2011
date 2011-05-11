#include "command.h"

void setup(){	
	initSerialLink();
	initPinceControl();
	//initTourelle();
	initSensor();
}

void loop(){
	readIncomingData();
	sensorTrigger();
	//tourelleRun();
	
}


