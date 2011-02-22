#include "pinceControl.h"

void initPinceControl(){
	Ax12.begin(&Serial1 , BAUDRATE, CONTROLPIN);
}

int setPinceState(unsigned char index,unsigned char etat){
	char droite,gauche;
	if(PINCEAV==index){
		droite=IDAVD;
		gauche=IDAVG;
	}else if(PINCEAR==index){
		droite=IDARD;
		gauche=IDARG;
	}else return 0;
	
	if(PINCE_O==etat){
		Ax12.move(droite,POSITIONOD);
		Ax12.move(gauche,POSITIONOG);
	}else if(PINCE_S==etat){
		Ax12.move(droite,POSITIONSD);
		Ax12.move(gauche,POSITIONSG);
	}else if(PINCE_F==etat){
		Ax12.move(droite,POSITIONFD);
		Ax12.move(gauche,POSITIONFG);
	}else return 0;

	return 1;
}

int setStepper(unsigned int id,unsigned char index,int step){
	Stepper * tmp;
	switch(index){
		case 0 : tmp=&motor1; break;
		case 1 : tmp=&motor2; break;
		default : return 0;
	}
	if(step>0){
		tmp->step=step;
		tmp->dir=HAUT;
		tmp->messageId=id;
	}else if(step<0){
		tmp->step=-step;
		tmp->messageId=id;
	}
	return 1;
}

void pinceContolRun(){
	motor1.run();
	motor2.run();
}
