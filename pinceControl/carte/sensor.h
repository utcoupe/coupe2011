#ifndef SENSOR_H_
#define SENSOR_H_

#include "WProgram.h"
#include "message.h"

//define microswitch
#define PIN_MS_AV 42
#define PIN_MS_AR 43
#define NB_MS 2
//define jack
#define PIN_JACK_BLEU 44
#define PIN_JACK_ROUGE 45
#define BLEU 1
#define ROUGE 2
//define sharp
#define NB_SHARP 10
//define protocole
#define SEND_COLOR 20
#define SEND_JACK 21


struct TriggerSharp{
	unsigned int messageId;
	int pin;
	unsigned int value;
};

struct TriggerMS{
	unsigned int messageId;
	int pin;
	bool value;
};


static TriggerSharp trigerSharp[NB_SHARP];
static TriggerMS trigerMS[NB_MS];

void initSensor();
int waitJack();
int getSharp(unsigned char pin);
int setTriggerSharp(unsigned int id, unsigned char pin,unsigned int ref);
void removeTriggerSharp(unsigned char index);
int getMicroSwitch(unsigned char pin);
int setTriggerMS(unsigned int id, unsigned char pin,bool ref);
void removeTriggerMS(unsigned int index);
void sensorTrigger();

#endif
