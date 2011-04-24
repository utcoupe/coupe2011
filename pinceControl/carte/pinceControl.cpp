#include "pinceControl.h"

void initPinceControl(){
	Ax12.begin(&Serial2 , BAUDRATE, CONTROLPIN);
	goal_position_AV=-1;
	goal_position_AR=-1;
	initPWM();
	//initEncoders();
	pinceRecal(PINCEAV);
	//pinceRecal(PINCEAR);
}

int pinceRecal(unsigned char face){
	pinMode(PIN_MS_AV, OUTPUT);
	digitalWrite(PIN_MS_AV, LOW);
	pinMode(PIN_MS_AR, OUTPUT);
	digitalWrite(PIN_MS_AR, LOW);
	pinMode(PIN_MS_AV,INPUT);
	pinMode(PIN_MS_AR,INPUT);
	goal_position_AV=-1;
	goal_position_AR=-1;
	if(face==PINCEAV){
		setAVPWM(-PWM_VALUE);
		while(digitalRead(PIN_MS_AV)!=HIGH){
			delay(40);
		}
		setAVPWM(0x00);
		initEncoders();
		return 1;
	}
	if(face==PINCEAR){
		setARPWM(-PWM_VALUE);
		while(digitalRead(PIN_MS_AR)!=HIGH){
			delay(40);
		}
		setARPWM(0x00);
		initEncoders();
		return 1;
	}
	return 0;
}

int setPincePWM(unsigned char face,int pwm,int tempo){
	if(pwm > 255 || pwm < -255) return 0;
	if(face==PINCEAV){
		goal_position_AV=-1;
		setAVPWM(pwm);
		delay(tempo);
		setAVPWM(0x00);
		return 2;
	}
	if(face==PINCEAR){
		goal_position_AR=-1;
		setARPWM(pwm);
		delay(tempo);
		setARPWM(0x00);
		return 2;
	}
	return 0;
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
	if(pos >= POSITION_MAX) return 0; //erreur
	switch(index){
		case 0 : 
		setAVPWM(PWM_VALUE);
		goal_position_AV=pos;
		msg_position_AV=id;
		return 1;
		case 1 : 
		setARPWM(PWM_VALUE);
		goal_position_AR=pos;
		msg_position_AR=id;
		return 1;
		default : 
		return 0;//erreur
	}
}

