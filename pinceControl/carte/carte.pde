#include "command.h"

void setup(){	
	initSerialLink();
	initPinceControl();
	//initTourelle();
	initSensor();
	//setupTourelle();
}

void loop(){
	readIncomingData();
	sensorTrigger();
	//tourelleRun();
	//loopTourelle();
	
}


