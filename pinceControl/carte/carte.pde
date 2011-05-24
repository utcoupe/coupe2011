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
	encoderSafe();
	sensorTrigger();
	//tourelleRun();
	//loopTourelle();
}


