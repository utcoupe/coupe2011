/*
 * pwm.cpp
 *
 *  Created on: 13 janv. 2011
 *      Author: HoHen
 */
#include "wiring.h"
#include "pwm.h"

void initPWM(){
	pinMode(PIN_PWM_LEFT_1,OUTPUT);
	pinMode(PIN_PWM_LEFT_2,OUTPUT);
	pinMode(PIN_PWM_RIGHT_1,OUTPUT);
	pinMode(PIN_PWM_RIGHT_2,OUTPUT);
}

void setLeftPWM(int value){
	if(value>0)
		analogWrite(PIN_PWM_LEFT_1,value);
	else if(value<0)
		analogWrite(PIN_PWM_LEFT_2,value);
	else{
		analogWrite(PIN_PWM_LEFT_1,value);
		analogWrite(PIN_PWM_LEFT_2,value);
	}
}

void setRightPWM(int value){
	if(value>0)
		analogWrite(PIN_PWM_RIGHT_1,value);
	else if(value<0)
		analogWrite(PIN_PWM_RIGHT_2,value);
	else{
		analogWrite(PIN_PWM_RIGHT_1,value);
		analogWrite(PIN_PWM_RIGHT_2,value);
	}
}
