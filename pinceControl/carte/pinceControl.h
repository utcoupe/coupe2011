#ifndef PINCECONTROL_H_
#define PINCECONTROL_H_

#include "AFMotor.h"
#include "message.h"
#include "pwm.h"


//Pinces
#define HAUT FORWARD
#define BAS BACKWARD
#define IDAVG		2	//avant gauche
#define IDAVD		3	//avant droit
#define IDARG		7	//arriere gauche
#define IDARD		1	//arriere droit
#define POSITIONFG	1024-700	//fermé Gauche
#define POSITIONSG	1024-600	//serré
#define POSITIONOG	1024-400	//ouvert
#define POSITIONFD	1024-300	//fermé Droit
#define POSITIONSD	1024-400	//serré
#define POSITIONOD	1024-600	//ouvert
#define CONTROLPIN 	40
#define BAUDRATE 	115200
/*cf protocole*/
#define PINCEAV 	0
#define PINCEAR 	1
#define PINCE_F	 	0
#define PINCE_S	 	1
#define PINCE_O	 	2

//deplacement verticale
#define POSITION_MAX 10000
#define PWM_MOVE 		250 // PWM pour se déplacer rapidement
#define PWM_MAINTIENT	100 // PWM pour le maintient en position
#define MARGE_MARCHE	500 // Tolérance avant de ralentir
#define MARGE_MAINTIENT	50 // Tolerance de maintient en position pour la pince
#define SEUIL_LOAD	0x50
#define PIN_MS_RECAL_AV		30 //microswitch pour recalage
#define PIN_MS_RECAL_AR		37

extern int msg_position_AV,msg_position_AR;

#include "encoder.h"

void initPinceControl();
//int setPinceState(unsigned char index,unsigned char etat);
int setPincePosition(unsigned int id,unsigned char,unsigned int pos);
int pinceRecal(unsigned char face);

#endif
