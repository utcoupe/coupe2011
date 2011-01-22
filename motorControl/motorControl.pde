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
int value_pwm_left = 0;
int value_pwm_right = 0;

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

	/*verification de l'ÔøΩtat des consignes, si elles sont atteintes ou non*/
	//checkCurrentGoal();
	
	/*envoyer un caractere sur le port serie pour test*/
	/*if(index == 500){
          Serial.println("_________________");
          Serial.print("time: ");Serial.println(millis());
          Serial.print("angle: ");Serial.println(robot_state.angle);
          Serial.print("speed: ");Serial.println(robot_state.speed_left);
          Serial.print("x: ");Serial.println(robot_state.x);
          Serial.print("y: ");Serial.println(robot_state.y);
          Serial.print("pwmL: ");Serial.println(value_pwm_left);
	  Serial.print("pwmR: ");Serial.println(value_pwm_right);
          Serial.print("encL: ");Serial.println(value_left_enc);
	  Serial.print("encR: ");Serial.println(value_right_enc);
          index=0;
        }else{
          index ++;
        }*/
}





