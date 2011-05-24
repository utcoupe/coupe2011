#ifndef TOURELLE_H
#define TOURELLE_H

#include "AFMotor.h"
#include "ping.h"
#include <Servo.h>
#include <WProgram.h>
#include "ping.h"


#define NB_PAS 200
#define STYLE SINGLE
#define SPEED 300000
#define MAXDIST 300 // cm
#define TEMPO 100 // ms
#define INCREMENT 10
#define DIFFMAX 10
#define MARGE_BORD 10

#define TURNRIGHT BACKWARD
#define TURNLEFT FORWARD

#define PIN_PING_GAUCHE 26
#define PIN_PING_DROITE 22

enum ModeTourr 
{
	Right = 1,
	Left,
	Recherche,
	Echappement,
	None
};

class Stepper : public AF_Stepper
{
public:
	int steps;
	int dir;
	int style;
	int position;
	
	Stepper(int ID) : 
		AF_Stepper(NB_PAS, ID)
	{
		setSpeed(SPEED);
		steps = 0;
		dir = TURNLEFT;
		style = STYLE;
		position = 0;
		release();
	}
	
	void run()
	{
		if(steps > 0)
		{
			step(steps, dir, style);
			steps = 0;
			if(dir == TURNLEFT)
				position += steps;
			else
				position -= steps;
		}
	}
};

void initTourelle();
void turn(int direction);
bool sensorTest(int valeur);
void recherche();
void tourelleRun();
void loopTourelle();
int testPing();
void run();
void recherche();
void setupTourelle();
bool cibleValide();
void update_pos();

#endif
