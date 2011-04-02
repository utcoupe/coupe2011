#include "pinceControl.h"

void initPinceControl(){
	Ax12.begin(&Serial1 , BAUDRATE, CONTROLPIN);
	goal_position_AV=-1;
	goal_position_AR=-1;
	initPWM();
	setARPWM(0x00);
	setAVPWM(0x00);
	initEncoders();
}

int presentLoad(unsigned char idPince){
	if(idPince==PINCEAV){
		if((Ax12.readPresentLoad(IDAVG)>SEUIL_LOAD) && (Ax12.readPresentLoad(IDAVD)>SEUIL_LOAD))
			return 2;
		return 1;
	}else if(idPince==PINCEAR){
		if((Ax12.readPresentLoad(IDARG)>SEUIL_LOAD) && (Ax12.readPresentLoad(IDARD)>SEUIL_LOAD))
			return 2;
		return 1;
	}else{
		return 0;//erreur
	}
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

int setPincePosition(unsigned int id,unsigned char index,unsigned int pos){
	switch(index){
		case 0 : 
		setAVPWM(PWM_VALUE);
		goal_position_AV=pos;
		msg_position_AV=id;
		break;
		case 1 : 
		setARPWM(PWM_VALUE);
		goal_position_AR=pos;
		msg_position_AR=id;
		break;
		default : return 0;
	}
}

