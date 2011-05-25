#include "command.h"

void setup(){
	initSerialLink();
	initPinceControl();
	initEncoders();
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


