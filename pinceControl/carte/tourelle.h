#ifndef TOURELLE_H
#define TOURELLE_H

#include <Servo.h>
#include <WProgram.h>

//params
#define LEFT  5
#define RIGHT 175
#define INCREMENT  1
#define ANALOG_SHARP_R  9
#define ANALOG_SHARP_L  8
#define SERVO_PIN  9
#define TEMPS_LATENCE 50

//variables
static long position = LEFT;
static int sens = RIGHT;
static long distance = 0;
static long long oldMillis=millis();
static Servo serv;

void initTourelle();
void turn(int direction);
bool testSharp(int valeur);
void recherche();
void tourelleRun();

#endif
