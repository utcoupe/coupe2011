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
	pushGoal(TYPE_SPEED,50,0);
	pushGoal(TYPE_ANGLE,3.14,0);
	pushGoal(TYPE_POSITION,1000,1000);
}

void pushGoal(int type,double data_1,double data_2){
	if((goals.in+1)%SIZE != goals.out){
		Goal* incGoal = goals.goal+goals.in;
		incGoal->type = type;
		incGoal->data_1 = data_1;
		incGoal->data_2 = data_2;
		goals.in = (goals.in+1)%SIZE;
	}


}

void popGoal(){
	if(goals.in!=goals.out){
		current_goal.isReached = false;
		Goal* outGoal = goals.goal+goals.out;
		current_goal.type = outGoal->type;
		switch (outGoal->type) {
		case TYPE_SPEED:
			current_goal.speed = outGoal->data_1;
			break;
		case TYPE_ANGLE:
			current_goal.angle = outGoal->data_1;
			break;
		case TYPE_POSITION:
			current_goal.x = outGoal->data_1;
			current_goal.y = outGoal->data_2;
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
