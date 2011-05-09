#ifndef SENSOR_H_
#define SENSOR_H_

#include "WProgram.h"
#include "message.h"


//define microswitch
#define PIN_MS_AV 	24
#define PIN_MS_AR 	25
#define NB_MS 		2

//define jack et couleur
#define PIN_JACK 	50
#define LED_BLEU 	51
#define LED_ROUGE 	47
#define BLEU 		0
#define ROUGE 		1

//interupteur bleu 33
//interupteur rouge 41

// pin couleur
#define PIN_COLOR	51
#define BLUE		0
#define RED			1
//define sharp
#define FACEAV 		0
#define FACEAR 		1
#define MAX_SEUIL 	450 //valeur maximum pour un seuil de sharp ==> distance minimum de detection
#define NB_SHARP 	12
//sharp type 1
#define DIST_MAX1	10 //valeur sharp petit = loin
#define DIST_MIN1	400
//sharp type 2
#define DIST_MAX2	100 //valeur sharp petit = loin
#define DIST_MIN2	400
//numero de pin des sharp 0 <=> A0
//num 1 en bas num 5 en haut
//Face avant
#define SHARP_AV1	0
#define SHARP_AV2D	1 //droite
#define SHARP_AV2G	2 //gauche
#define SHARP_AV3	3
#define SHARP_AV4	4
#define SHARP_AV5	5
//face arriere
#define SHARP_AR1	6
#define SHARP_AR2D	7
#define SHARP_AR2G	8
#define SHARP_AR3	9
#define SHARP_AR4	10
#define SHARP_AR5	11


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
static int JackMessageID=-42;

void initSensor();
int jack(int id);
int waitJack();
int setLED(unsigned char color);
int getPion(unsigned char face);
int getSharp(unsigned char pin);
int setTriggerSharp(unsigned int id, unsigned char pin,unsigned int ref);
void removeTriggerSharp(unsigned char index);
int getMicroSwitch(unsigned char pin);
int setTriggerMS(unsigned int id, unsigned char pin,bool ref);
void removeTriggerMS(unsigned int index);
void sensorTrigger();
int getColor();

#endif
