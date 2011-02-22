#ifndef SENSOR_H_
#define SENSOR_H_

#include "WProgram.h"
#include "message.h"

#define PIN_MS_AV 42
#define PIN_MS_AR 43
#define NB_SHARP 10
#define NB_MS 2


struct TriggerSharp{
	unsigned int messageId;
	unsigned char pin;
	unsigned int value;
};

struct TriggerMS{
	unsigned int messageId;
	unsigned char pin;
	bool value;
};


static TriggerSharp trigerSharp[NB_SHARP];
static TriggerMS trigerMS[NB_MS];

void initSensor();
int getSharp(unsigned char pin);
int setTriggerSharp(unsigned int id, unsigned char pin,unsigned int ref);
void removeTriggerSharp(unsigned char index);
int getMicroSwitch(unsigned char pin);
int setTriggerMS(unsigned int id, unsigned char pin,bool ref);
void removeTriggerMS(unsigned int index);
void sensorTrigger();

#endif
