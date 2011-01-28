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
int value_pwm_left = 0;
int value_pwm_right = 0;

void setup(){
	/*Initialise la file des buts a atteindre*/
	//initGoals();
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
	// Cedric: c'est ce qui se passe actuellement, les pwm sont nulles lorsque le but est atteint, et ne change pas tant qu'il n'y a pas de nouveau but
	//if(current_goal.isReached)
	//	popGoal(); /* va changer la valeur de current_goal */


	/*calcul des sorties*/

	//if(!current_goal.isReached){
	//	if(current_goal.type == TYPE_SPEED)
	//		speedControl(&value_pwm_left,&value_pwm_right);
	//	else if(current_goal.type == TYPE_ANGLE)
	//		angleControl(&value_pwm_left,&value_pwm_right);
	//	else
	//		positionControl(&value_pwm_left,&value_pwm_right);
	//}

	/*ecriture de la sortie*/
	if(abs(value_pwm_left)<60 && value_pwm_left != 0) {
		if(value_pwm_left>0) {
			value_pwm_left = 60;
		} else {
			value_pwm_left = -60;
		}
	}

	setLeftPWM(value_pwm_left);
        if(abs(value_pwm_right)<60 && value_pwm_right != 0) {
                if(value_pwm_right>0) {
                        value_pwm_right = 60;
                } else {
                        value_pwm_right = -60;
                }
        }


	setRightPWM(value_pwm_right);

	/*modele d'evolution*/
	computeRobotState();
	
	// On éteint la del
	digitalWrite(16, LOW);
	
	// On attend le temps qu'il faut pour boucler
	delayMicroseconds(DUREE_CYCLE*1000-(micros()-timeStart));
}


