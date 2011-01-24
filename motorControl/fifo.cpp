/*
 * fifo.c
 *
 *  Created on: 16 janv. 2011
 *      Author: HoHen
 */

#include "fifo.h"

Fifo goals;

void initGoals(){
	goals.goal = (Goal*)malloc(sizeof(Goal)*SIZE);
	goals.in = 0;
	goals.out = 0;

	/*pour tester le robot*/
//	pushGoal(TYPE_ANGLE,0,0,4.71); // 0, 0, angle
	pushGoal(TYPE_POSITION,3537,1000,100); //x , y , ratio vitesse max [0-200]
//	pushGoal(TYPE_SPEED,2,1000,0); // vitesse (en ticks/ms), timeout, O
}

void pushGoal(int type,int data_1,int data_2,double data_3){
	if((goals.in+1)%SIZE != goals.out){
		Goal* incGoal = goals.goal+goals.in;
		incGoal->type = type;
		incGoal->data_1 = data_1;
		incGoal->data_2 = data_2;
		incGoal->data_3 = data_3;
		goals.in = (goals.in+1)%SIZE;
	}
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
			current_goal.periode = outGoal->data_2;
			break;
		case TYPE_ANGLE:
			current_goal.angle = outGoal->data_3;
			break;
		case TYPE_POSITION:
			current_goal.x = outGoal->data_1;
			current_goal.y = outGoal->data_2;
			current_goal.speed = outGoal->data_3;
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
