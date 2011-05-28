#include "command.h"

void setup(){
	initSerialLink();
	initPinceControl();
	//initTourelle();//pas utilisé
	initSensor();
	setupTourelle();
}

void loop(){
	readIncomingData();
	encoderSafe();
	sensorTrigger();
	//tourelleRun();//pas utilisé
	loopTourelle();
}


