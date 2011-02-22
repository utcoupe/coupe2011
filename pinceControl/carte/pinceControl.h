#ifndef PINCECONTROL_H_
#define PINCECONTROL_H_

#include "Ax12.h"
#include "AFMotor.h"
#include "message.h"

//stepper
#define STEPPER_AV 1
#define STTEPER_AR 2
#define NB_PAS 200
#define STYLE DOUBLE
#define SPEED 20000
/* info :
#define FORWARD 1
#define BACKWARD 2
* */
#define HAUT 1
#define BAS 2

//Pinces
#define IDAVG		1	//avant gauche
#define IDAVD		2	//avant droit
#define IDARG		3	//arriere gauche
#define IDARD		4	//arriere droit
#define POSITIONFG	800	//fermé Gauche
#define POSITIONSG	600	//serré
#define POSITIONOG	400	//ouvert
#define POSITIONFD	200	//fermé Droit
#define POSITIONSD	400	//serré
#define POSITIONOD	600	//ouvert
#define CONTROLPIN 	40
#define BAUDRATE 	115200
#define PINCEAV 	0
#define PINCEAR 	1
#define PINCE_F	 	0
#define PINCE_S	 	1
#define PINCE_O	 	2

class Stepper : public AF_Stepper{
	public:
	int step,dir,style;
	unsigned int messageId;
	Stepper(int ID) : AF_Stepper(NB_PAS, ID) {
		setSpeed(SPEED);
		step=0;dir=FORWARD;style=STYLE;
		messageId=0;
		release();
	}
	void run(){
		if(step > 0){
		onestep(dir,style);
		step--;
		if(step == 0)
			sendMessage(messageId,1);
		}	
		delay(10);
	}

};

static Ax12Class Ax12;
static Stepper motor1(STEPPER_AV);
static Stepper motor2(STTEPER_AR);

void initPinceControl();
int setPinceState(unsigned char index,unsigned char etat);
int setStepper(unsigned int id,unsigned char index,int step);
void pinceContolRun();

#endif
