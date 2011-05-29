#ifndef TOURELLE_H
#define TOURELLE_H

#include "AFMotor.h"
#include "ping.h"
#include "message.h"
#include <Servo.h>
#include <WProgram.h>


#define NB_PAS 200
#define STYLE SINGLE
#define SPEED 300000
#define MAXDIST 300 // cm
#define TEMPO 50 // ms
#define INCREMENT 10
#define DIFFMAX 10 
#define MARGE_BORD 10
#define SETUP_TIMEOUT 1000

#define DISTANCE_OUTOFRANGE -1
#define DISTANCE_UNDEFINED -2

#define COORD_UNDEFINED -3

#define TURNRIGHT BACKWARD
#define TURNLEFT FORWARD

#define PIN_PING_GAUCHE 26
#define PIN_PING_DROITE 22

class Stepper : public AF_Stepper
{
public:
	int _steps;
	int _dir;
	int _style;
	int _position;
	
	Stepper(int ID);
	void run();
};

class Ping
{
private:
	int _pin;
	int _maxDist;
	
public:
	Ping (int pin, int maxDist);
	int sendPing();
}

class Tourelle
{
private:
	Stepper _motor;
	Ping _leftPing;
	Ping _rightPing;
	TourelleMode _mode;
	int _robot_pos_x;
	int _robot_pos_y;
	int _robot_angle;
	int _distanceGauche;
	int _distanceDroite;
	long long _millitime;
	PingTurn _pingTurn;
	
	enum TourelleMode 
	{
		TurnRight = 1,
		TurnLeft,
		Recherche,
		Echappement,
		None
	};
	
	enum PingTurn
	{
		RightPing,
		LeftPing
	};
	
	struct TargetCoord
	{
		int x;
		int y;
	};
	
	TargetCoord findTargetCoord(int angleTourelle, int distance);
	bool targetInRange(TargetCoord targetCoord);
	void measureDistances();
	void chooseMode();
	void setMotorParameters();

public:
	Tourelle(int robot_pos_x, int robot_pos_y, int robot_angle);
	int setup();
	void loop();
	void update_pos();
}

int setupTourelle();
void loopTourelle();
void update_pos();

#endif
