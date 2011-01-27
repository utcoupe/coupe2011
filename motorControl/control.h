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

#define PHASE_1 1
#define PHASE_2 2
#define PHASE_3 3
#define PHASE_4 4

typedef struct CurrentGoal {
	int type;
	int speed;
	int periode;
	int x;
	int y;
	double angle;
	bool isReached;
	bool isCanceled;
	bool isPaused;
	int phase;
} CurrentGoal;

extern CurrentGoal current_goal;

void initController();

void speedControl(int*,int*);
void angleControl(int*,int*);
void positionControlCurviligne(int*,int*);
void positionControl(int*,int*);
double moduloPI(double Nb);
void computeRobotState();

int computePID(double,int,int,int);

#endif /* CONTROL_H_ */
