#include "pinceControl.h"
//#include "servoControl.h"

int msg_position_AV,msg_position_AR;

void initPinceControl(){
	goal_position_AV=-1;
	goal_position_AR=-1;
	initPWM();
	//initServo();
}

int pinceRecal(unsigned char face){
	pinMode(PIN_MS_RECAL_AV, OUTPUT);
	digitalWrite(PIN_MS_RECAL_AV, LOW);
	pinMode(PIN_MS_RECAL_AR, OUTPUT);
	digitalWrite(PIN_MS_RECAL_AR, LOW);
	pinMode(PIN_MS_RECAL_AV,INPUT);
	pinMode(PIN_MS_RECAL_AR,INPUT);
	goal_position_AV=-1;
	goal_position_AR=-1;
	if(face==PINCEAV){
		setAVPWM(-PWM_MAINTIENT);
		while(digitalRead(PIN_MS_RECAL_AV)!=HIGH){
			delay(40);
		}
		setAVPWM(0x00);
		initEncoders();
		return 2;
	}
	if(face==PINCEAR){
		setARPWM(-PWM_MAINTIENT);
		while(digitalRead(PIN_MS_RECAL_AR)!=HIGH){
			delay(40);
		}
		setARPWM(0x00);
		initEncoders();
		return 2;
	}
	return -42;
}

/** Utilisation des servos
 */
/*
int setPinceState(unsigned char index,unsigned char etat){
	return setServoState(index, etat);
}*/

int setPincePosition(unsigned int id, unsigned char index,unsigned int pos){
	if(pos >= POSITION_MAX) return 0; //erreur
	switch(index){
		case 0 :
			setAVPWM(PWM_MOVE);
			goal_position_AV=pos;
			msg_position_AV=id;
			return 1;
		case 1 : 
			setARPWM(PWM_MOVE);
			goal_position_AR=pos;
			msg_position_AR=id;
			return 1;
		default : 
			return 0;//erreur
	}
}

