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
	double data_1; /*speed, angle ou x*/
	double data_2; /* y */
} Goal;

typedef struct {
	Goal* goal;
	int in;
	int out;
} Fifo;


void initGoals();
void pushGoal(int,double,double);
void popGoal();
void clearGoals();

#endif /* FIFO_H_ */
