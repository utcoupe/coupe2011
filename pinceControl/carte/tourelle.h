#ifndef TOURELLE_H
#define TOURELLE_H

#include "AFMotor.h"
#include "ping.h"
#include <Servo.h>
#include <WProgram.h>
#include "ping.h"


#define NB_PAS 200
#define STYLE SINGLE
#define SPEED 20000
#define MAXDIST 100 //cm
#define TEMPO 100 //ms
#define INCREMENT 10
#define DIFFMAX 10
#define MARGE_BORD 10

#define RIGHT 1
#define LEFT 2
#define RECHERCHE 3
#define NONE 4
#define TURNRIGHT BACKWARD
#define TURNLEFT FORWARD

#define PIN_PING_GAUCHE 26
#define PIN_PING_DROITE 22



class Stepper : public AF_Stepper{
	public:
	int step,dir,style,position;
	Stepper(int ID) : AF_Stepper(NB_PAS, ID) {
		setSpeed(SPEED);
		step=0;
		dir=TURNLEFT;
		style=STYLE;
		position=0;
		release();
	}
	void run(){
		if(step > 0){
			onestep(dir,style);
delay(2);
onestep(dir,style);
delay(2);
onestep(dir,style);
delay(2);
onestep(dir,style);
delay(2);
onestep(dir,style);
delay(2);
onestep(dir,style);
			step--;
			if(dir==TURNLEFT){
				position++;
			}else{
				position--;
			}
			delay(2);
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



/*
#include <Servo.h>
#include <WProgram.h>
#include "ping.h"

//params
#define LEFT  0
#define RIGHT 1
#define INCREMENT  1
#define RIGHT_SENSOR_PIN  9
#define LEFT_SENSOR_PIN  8
#define SERVO_PIN  9
#define TEMPS_LATENCE 50

//variables
static long position = 0;
static int sens = RIGHT;
static long distance = 0;
static long long oldMillis=millis();
static Servo serv;

void initTourelle();
void turn(int direction);
bool sensorTest(int valeur);
void recherche();
void tourelleRun();*/

#endif
