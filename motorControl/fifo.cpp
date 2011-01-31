/*
 * fifo.c
 *
 *  Created on: 16 janv. 2011
 *      Author: HoHen
 */

#include "fifo.h"
#include "math.h"

Fifo goals;

void initGoals(){
	goals.goal = (Goal*)malloc(sizeof(Goal)*SIZE);
	goals.in = 0;
	goals.out = 0;

	/*pour tester le robot
	pushGoalOrientation(3.14,255); //angle,vitesse
	pushGoalPosition(10000,9000,255); //x,y,vitesse
	pushGoalSpeed(2.0,10000); //vitesse,periode
	*/
}

void pushGoalPosition(double x, double y, double speed){
	if((goals.in+1)%SIZE != goals.out){
		Goal* incGoal = goals.goal+goals.in;
		incGoal->type = TYPE_POSITION;
		incGoal->data_1 = x;
		incGoal->data_2 = y;
		incGoal->data_3 = speed;
		goals.in = (goals.in+1)%SIZE;
	}
}

void pushGoalOrientation(double angle, double speed){
	if((goals.in+1)%SIZE != goals.out){
		Goal* incGoal = goals.goal+goals.in;
		incGoal->type = TYPE_ANGLE;
		incGoal->data_1 = angle;
		incGoal->data_2 = speed;
		goals.in = (goals.in+1)%SIZE;
	}
}

void pushGoalSpeed(double speed, double period){
	if((goals.in+1)%SIZE != goals.out){
		Goal* incGoal = goals.goal+goals.in;
		incGoal->type = TYPE_SPEED;
		incGoal->data_1 = speed;
		incGoal->data_2 = period;
		goals.in = (goals.in+1)%SIZE;
	}
}

void pushGoalPwm(double speed, double period){
	if((goals.in+1)%SIZE != goals.out){
		Goal* incGoal = goals.goal+goals.in;
		incGoal->type = TYPE_PWM;
		incGoal->data_1 = speed;
		incGoal->data_2 = period;
		goals.in = (goals.in+1)%SIZE;
	}
}

void pushGoalManualCalibration(int type,double value){
	if((goals.in+1)%SIZE != goals.out){
		Goal* incGoal = goals.goal+goals.in;
		incGoal->type = type;
		incGoal->data_1 = value;
		goals.in = (goals.in+1)%SIZE;
	}
}

void pushGoalDelay(double value){
	if((goals.in+1)%SIZE != goals.out){
		Goal* incGoal = goals.goal+goals.in;
		incGoal->type = TYPE_DELAY;
		incGoal->data_1 = value;
		goals.in = (goals.in+1)%SIZE;
	}
}

void pushGoalAutoCalibration(){
	/* phase 1 : sortir de la zone de depart */
	pushGoalPosition(10000,0,130);
	/* phase 2 : tourner d'un angle PI/2 */
	pushGoalOrientation(M_PI/2,150);
	/* phase 3 : reculer pendant 4s */
	pushGoalPwm(-100,4000);
	/* phase 4 : fixer X et angle */
	pushGoalManualCalibration(TYPE_CALIB_X,0); //TODO preciser la valeur de X (c'est pas vraiment 0)
	pushGoalManualCalibration(TYPE_CALIB_ANGLE,M_PI/2);
	/* phase 5 : avancer un peu pour pouvoir tourner */
	pushGoalPosition(0,2000,130); //TODO preciser la valeur de X (c'est pas vraiment 0)
	/* phase 6 : tourner d'un angle -PI/2 */
	pushGoalOrientation(-M_PI/2,150);
	/* phase 7 : reculer pendant 4s */
	pushGoalPwm(-100,4000);
	/* phase 8 : fixer Y (et peut-etre speed, a voir si c'est utile) */
	pushGoalManualCalibration(TYPE_CALIB_Y,0); //TODO preciser la valeur de Y (c'est pas vraiment 0)
}




void popGoal(){
	if(goals.in!=goals.out){
		current_goal.isReached = false;
		current_goal.phase = PHASE_1;
		Goal* outGoal = goals.goal+goals.out;
		current_goal.type = outGoal->type;
		switch (outGoal->type) {
		case TYPE_SPEED:
			current_goal.speed = outGoal->data_1;
			current_goal.period = outGoal->data_2;
			break;
		case TYPE_ANGLE:
			current_goal.angle = outGoal->data_1;
			current_goal.speed = outGoal->data_2;
			break;
		case TYPE_POSITION:
			current_goal.x = outGoal->data_1;
			current_goal.y = outGoal->data_2;
			current_goal.speed = outGoal->data_3;
			break;
		case TYPE_PWM:
			current_goal.speed = outGoal->data_1;
			current_goal.period = outGoal->data_2;
			break;
		case TYPE_DELAY:
			current_goal.period = outGoal->data_1;
			break;
		case TYPE_CALIB_X:
			current_goal.x = outGoal->data_1;
			break;
		case TYPE_CALIB_Y:
			current_goal.y = outGoal->data_1;
			break;
		case TYPE_CALIB_ANGLE:
			current_goal.angle = outGoal->data_1;
			break;
		default:
			break;
		}
		goals.out = (goals.out+1)%SIZE;
	}
}

void clearGoals(){
	goals.in = 0;
	goals.out = 0;
}

bool fifoIsEmpty(){
	return goals.in==goals.out;
}
