#include "command.h"

void setup(){
	initSerialLink();
	initPinceControl();
<<<<<<< HEAD
	//initTourelle();//pas utilisé
=======
	initEncoders();
	//initTourelle();
>>>>>>> 03c484c19eeb201edca2a5396761d221996ab58a
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


