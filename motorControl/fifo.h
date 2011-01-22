/*
 * fifo.h
 *
 *  Created on: 16 janv. 2011
 *      Author: HoHen
 */

#ifndef FIFO_H_
#define FIFO_H_

#include "parameters.h"
#include "control.h"


typedef struct {
	int type;
	int data_1; /*speed ou x*/
	int data_2; /* y */
	double data_3; /* angle */
} Goal;

typedef struct {
	Goal* goal;
	int in;
	int out;
} Fifo;


void initGoals();
void pushGoal(int,int,int,double);
void popGoal();
void clearGoals();

#endif /* FIFO_H_ */
