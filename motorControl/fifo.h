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
	int type; /*1,2,3 selon le type d'asserv*/
	int id; /* identifiant de la demande, utile pour envoyer une confirmation a la strategie, -1 si pas besoin de message */
	double data_1; /*speed (pour but en vitesse) ou x ou angle*/
	double data_2; /* y ou speed (pour but en angle) ou period*/
	double data_3; /* speed (pour but en position)*/
} Goal;

typedef struct {
	Goal* goal;
	int in;
	int out;
} Fifo;


void initGoals();

void pushGoalPosition(int,double,double,double);
void pushGoalOrientation(int,double,double);
void pushGoalSpeed(int,double,double);
void pushGoalAutoCalibration(int,bool);
void pushGoalManualCalibration(int,double);
void pushGoalDelay(double);
void pushGoalPwm(double,double);

void popGoal();
void clearGoals();
bool fifoIsEmpty();

#endif /* FIFO_H_ */
