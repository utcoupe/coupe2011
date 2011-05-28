#ifndef TOURELLE_H
#define TOURELLE_H

#include "AFMotor.h"
#include "ping.h"
#include "message.h"
#include <Servo.h>
#include <WProgram.h>


#define NB_PAS 200
#define STYLE SINGLE
#define SPEED 30000
#define MAXDIST 300 // cm
#define TEMPO 500 // ms
#define INCREMENT 10
#define DIFFMAX 10
#define MARGE_BORD 10
#define SETUP_TIMEOUT 1000

#define DISTANCE_OUTOFRANGE -1
#define DISTANCE_UNDEFINED -2

#define TURNRIGHT BACKWARD
#define TURNLEFT FORWARD

#define PIN_PING_GAUCHE 24
#define PIN_PING_DROITE 22

#define RIGHT_PING 30
#define LEFT_PING 31

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
			if (dir == TURNLEFT)
				position += steps;
			else
				position -= steps;
			steps = 0;
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
int setupTourelle();
bool cibleValide();
void updatePos();

#endif
