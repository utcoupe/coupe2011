#include "command.h"

void setup()
{
	initSerialLink();
	initPinceControl();
	//initTourelle(); //pas utilisé
	initEncoders();
	initSensor();
	//setupTourelle();
}

void loop()
{
	readIncomingData();
	encoderSafe();
	sensorTrigger();
	//tourelleRun(); //pas utilisé
	//loopTourelle();
	testAV();
	testAR();
}


