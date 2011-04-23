#include "command.h"

void setup(){	
	initSerialLink();
	initPinceControl();
	//initTourelle();
	initSensor();//en dernier a cause du jack
}

void loop(){
	readIncomingData();
	sensorTrigger();
	//tourelleRun();
	
}


