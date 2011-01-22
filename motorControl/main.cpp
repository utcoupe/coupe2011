/*
 * main.cpp
 *
 *  Created on: 13 janv. 2011
 *      Author: HoHen
 */

#include <math.h>
#include <stdint.h>
#include "wiring.h"
#include "WProgram.h"
#include "parameters.h"

#include "encoder.h"
#include "robotstate.h"
#include "pwm.h"
#include "fifo.h"
#include "message.h"
#include "control.h"

unsigned long index = 0;
int value_pwm_right = 0;
int value_pwm_left = 0;

void setup(){
	/*Initialise la file des buts a atteindre*/
	initGoals();
	/*Active les pwm*/
	initPWM();
	/*Initialise le r�gulateur*/
	initController();
	/*Active les interruptions sur les encodeurs*/
	initEncoders();
	/*Definit la position initiale du robot*/
	initRobotState();
	/*Active la liaison s�rie*/
	initSerialLink();
}

void loop(){
	/* zone libre */
	pinMode(13,OUTPUT);
	if(index%200>100)
		 digitalWrite(13, HIGH);
	else
		 digitalWrite(13, LOW);
	/* fin zone de programmation libre */

	/*lecture des ordres*/
	/*syntaxe d'un message : <data> */
	readIncomingData();


	/*recuperation du but suivant (vitesse, angle ou position) */
	if(current_goal.isReached)
		popGoal(); /* va changer la valeur de current_goal */


	/*calcul des sorties*/

	if(!current_goal.isReached){
		if(current_goal.type == TYPE_SPEED)
			speedControl(&value_pwm_left,&value_pwm_right);
		else if(current_goal.type == TYPE_ANGLE)
			angleControl(&value_pwm_left,&value_pwm_right);
		else
			positionControl(&value_pwm_left,&value_pwm_right);
	}

	/*ecriture de la sortie*/
	setLeftPWM(value_pwm_left);
	setRightPWM(value_pwm_right);

	/*modele d'evolution*/
	computeRobotState();

	/*verification de l'�tat des consignes, si elles sont atteintes ou non*/


	/*envoyer un caractere sur le port serie pour test*/
	Serial.println(value_left_enc);
	//index++;
}




int main(void){
	init();

	setup();

	for (;;)
		loop();

	return 0;
}

