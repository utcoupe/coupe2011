#include "pinceControl.h"
//#include "servoControl.h"

int msg_position_AV,msg_position_AR;

void initPinceControl(){
	goal_position_AV=-1;
	goal_position_AR=-1;
	initPWM();
	//initServo();
}

int pinceRecal(int id, char face)
{
	sendMessage(id, 1);
	pinMode(PIN_MS_RECAL_AV, OUTPUT);
	digitalWrite(PIN_MS_RECAL_AV, LOW);
	pinMode(PIN_MS_RECAL_AR, OUTPUT);
	digitalWrite(PIN_MS_RECAL_AR, LOW);
	pinMode(PIN_MS_RECAL_AV,INPUT);
	pinMode(PIN_MS_RECAL_AR,INPUT);
	goal_position_AV=-1;
	goal_position_AR=-1;
	if(face==AVANT){
		return setPincePosition(id, AVANT, BAS);
		/*setAVPWM(-PWM_MAINTIENT);
		while(digitalRead(PIN_MS_RECAL_AV)!=HIGH){
			delay(40);
		}
		setAVPWM(0x00);
		initEncoders();
		return 2;*/
	}
	if(face==ARRIERE){
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
int setPinceState( char index, char etat){
	return setServoState(index, etat);
}*/

int setPincePosition(int id, char index, int pos){
	if(pos > POSITION_MAX) return 0; //erreur
	int pwm=PWM_MOVE;
	int pos_in_ticks=0;
	switch (pos)
	{
		case HAUT:
			pos_in_ticks = POSITION_MAX;
		break;

		case MIDLE:
			pos_in_ticks = 4000;
		break;

		case BAS:
			pos_in_ticks = 0;
		break;

		default:
			return E_INVALID_PARAMETERS_VALUE;
		break;
	}
	
	switch(index)
	{
		case AVANT:
			goal_position_AV=pos;
			msg_position_AV=id;
			return 1;
		case ARRIERE:
			goal_position_AR=pos;
			msg_position_AR=id;
			testAR();
			return 1;
		default : 
			return 0;//erreur
	}
}

