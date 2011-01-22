#include "WProgram.h"
#include "parameters.h"
#include <math.h>

#include "encoder.h"
#include "robotstate.h"
#include "pwm.h"
#include "fifo.h"
#include "message.h"
#include "control.h"


unsigned long index = 0;
unsigned long timeStart = 0;

void setup(){
	/*Initialise la file des buts a atteindre*/
	initGoals();
	/*Active les pwm*/
	initPWM();
	/*Initialise le regulateur*/
	initController();
	/*Active les interruptions sur les encodeurs*/
	initEncoders();
	/*Definit la position initiale du robot*/
	initRobotState();
	/*Active la liaison série*/
	initSerialLink();

	// LED qui n'en est pas une
	pinMode(16,OUTPUT);
}

void loop(){
	// On note le temps de début
	timeStart = micros();

	/* zone libre */
	// La del est allumée pendant le traitement
	digitalWrite(16, HIGH);
	/* fin zone de programmation libre */

	/*lecture des ordres*/
	readIncomingData();

	/*recuperation du but suivant (vitesse, angle ou position) */
	// Arthur: Ne pas le faire s'il n'y a pas de nouveau point, mais maintenir la position
	if(current_goal.isReached){
		popGoal(); /* va changer la valeur de current_goal */
		reinitPID = true; /*reinitialise les valeurs d'integration et de dérivée au prochain appel de computePID */
	}

	/*calcul des sorties*/
	int value_pwm_left = 0;
	int value_pwm_right = 0;

	if(!current_goal.isReached){
		if(current_goal.type == TYPE_SPEED)
			speedControl(current_goal.speed,&value_pwm_left,&value_pwm_right,5000);
		else if(current_goal.type == TYPE_ANGLE)
			angleControl(current_goal.angle,&value_pwm_left,&value_pwm_right);
		else
			positionControl(current_goal.x,current_goal.y,&value_pwm_left,&value_pwm_right);
	}

	/*ecriture de la sortie*/
	setLeftPWM(value_pwm_left);
	setRightPWM(value_pwm_right);

	/*modele d'evolution*/
	computeRobotState();

	/*verification de l'état des consignes, si elles sont atteintes ou non*/
	checkCurrentGoal();
	
	// On éteint la del
	digitalWrite(16, LOW);
	
	// On attend le temps qu'il faut pour boucler (10ms)
	delayMicroseconds(2000-(micros()-timeStart));
}


