/*
 * control.h
 *
 *  Created on: 13 janv. 2011
 *      Author: HoHen
 */

#ifndef CONTROL_H_
#define CONTROL_H_

#include <math.h>
#include "parameters.h"
#include "wiring.h"

typedef struct CurrentGoal {
	int type;
	double speed;
	double x;
	double y;
	double angle;
	bool isReached;
} CurrentGoal;

extern CurrentGoal current_goal;
extern bool reinitPID;

void initController();

void speedControl(int,int*,int*,int);
void angleControl(int,int*,int*);
void positionControl(int,int,int*,int*);

void computeRobotState();

int computePID(double,int,int,int);

void checkCurrentGoal();

#endif /* CONTROL_H_ */
