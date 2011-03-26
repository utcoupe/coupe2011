#ifndef TOURELLE_H
#define TOURELLE_H

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
void tourelleRun();

#endif
