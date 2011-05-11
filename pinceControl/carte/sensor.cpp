#include "sensor.h"

int getPion(unsigned char face){
	int value,value2;
	//test des sharp du bas vers le haut
	if(face == FACEAV){
		//test 1
		value = getSharp(SHARP_AV1);
		if(value>DIST_MAX1 && value<DIST_MIN1){
			//au moin 1 pion
			//test 2 sharp type 2 (value ou value 2)
			value = getSharp(SHARP_AV2G);
			value2 = getSharp(SHARP_AV2D);
			if((value>DIST_MAX2 && value<DIST_MIN2) || (value>DIST_MAX2 && value<DIST_MIN2)){
				//test 3 sharp type 1
				value = getSharp(SHARP_AV3);
				if(value>DIST_MAX1 && value<DIST_MIN1){
					//test 4
					value = getSharp(SHARP_AV4);
					if(value>DIST_MAX1 && value<DIST_MIN1){
						//test 5
						value = getSharp(SHARP_AV5);
						if(value>DIST_MAX1 && value<DIST_MIN1){
									return 5; // tete sur 2 pion
						} else return 4; // tete sur pion
					} else return 3; // tete
				}else return 2; // 2 pion
			}else return 1; // 1 pion seul
		}else return 0; //pas de pion
	}
	
	if(face == FACEAR){
		//test 1
		value = getSharp(SHARP_AR1);
		if(value>DIST_MAX1 && value<DIST_MIN1){
			//au moin 1 pion
			//test 2 sharp type 2 (value ou value 2)
			value = getSharp(SHARP_AR2G);
			value2 = getSharp(SHARP_AR2D);
			if((value>DIST_MAX2 && value<DIST_MIN2) || (value>DIST_MAX2 && value<DIST_MIN2)){
				//test 3 sharp type 1
				value = getSharp(SHARP_AR3);
				if(value>DIST_MAX1 && value<DIST_MIN1){
					//test 4
					value = getSharp(SHARP_AR4);
					if(value>DIST_MAX1 && value<DIST_MIN1){
						//test 5
						value = getSharp(SHARP_AR5);
						if(value>DIST_MAX1 && value<DIST_MIN1){
									return 5; // tete sur 2 pion
						} else return 4; // tete sur pion
					} else return 3; // tete
				}else return 2; // 2 pion
			}else return 1; // 1 pion seul
		}else return 0; //pas de pion
	}
	
	return 6; //erreur
}

void initSensor(){
	pinMode(PIN_MS_AV, INPUT);  
	pinMode(PIN_MS_AR, INPUT);
	pinMode(PIN_COLOR, INPUT);
	pinMode(PIN_JACK, INPUT);
	pinMode(LED_BLEU, OUTPUT);
	pinMode(LED_ROUGE, OUTPUT);
	digitalWrite(LED_BLEU,LOW);
	digitalWrite(LED_ROUGE,LOW);
	for(int i=0;i<NB_SHARP;i++)
		trigerSharp[i].pin=-1;
	for(int i=0;i<NB_MS;i++)
		trigerMS[i].pin=-1;
}

int setLED(unsigned char color){
	if(color == BLEU){
		digitalWrite(LED_BLEU,HIGH);
		digitalWrite(LED_ROUGE,LOW);
		return 1;
	}
	if(color == ROUGE){
		digitalWrite(LED_BLEU,LOW);
		digitalWrite(LED_ROUGE,HIGH);
		return 1;
	}
	
	//erreur donc on allume les 2
	digitalWrite(LED_BLEU,HIGH);
	digitalWrite(LED_ROUGE,HIGH);
	return -42;
}

int waitJack(){
	//attend le branchement d'un jack (il peut etre deja branché)
	while(digitalRead(PIN_JACK)!=HIGH)
		delay(40);
	//attend le debranchement du jack
	while(digitalRead(PIN_JACK)!=LOW)
		delay(40);
	return 2;
}

int getSharp(unsigned char pin)
{
  	switch (pin)
  	{
		case 0:
			return analogRead(A0);
		case 1:
			return analogRead(A1);
		case 2:
			return analogRead(A2);
		case 3:
			return analogRead(A3);
		case 4:
			return analogRead(A4);
		case 5:
			return analogRead(A5);
		case 6:
			return analogRead(A6);
		case 7:
			return analogRead(A7);
		case 8:
			return analogRead(A8);
		case 9:
			return analogRead(A9);
		case 10:
			return analogRead(A10);
		case 11:
			return analogRead(A11);
		default:
			return -1;
	}
}

//déclanché si la valeur es inferieur à ref
int setTriggerSharp(unsigned int id, unsigned char pin,unsigned int ref){
	if(pin > NB_SHARP) return 0; //erreur
	for(int i=0;i<NB_SHARP;i++){
		//remplacement de trigger
		if(trigerSharp[i].pin==pin){
			sendMessage(trigerSharp[i].messageId,0);//annule le precedent trriger
			trigerSharp[i].value=ref;
			trigerSharp[i].messageId=id;
			return 1;
		}
		//nouveau trigger
		if(trigerSharp[i].pin==-1){
			trigerSharp[i].pin=pin;
			trigerSharp[i].value=ref;
			trigerSharp[i].messageId=id;
			return 1;
		}
	}
	return 0;//erreur
}

void removeTriggerSharp(unsigned char index){
		// aucun trigger
		if(trigerSharp[0].pin==-1) return;
		//1 trigger
		if(trigerSharp[1].pin==-1){
			trigerSharp[1].pin=-1;//index == 1 ici j'espere
			return;
		}
		//au moin 2 trigger
		for(int j=2;j<NB_SHARP;j++){
			if(trigerSharp[j].pin==-1){
				//remplace le trigger index par le dernier trigger
				trigerSharp[index].pin=trigerSharp[j-1].pin;
				trigerSharp[index].value=trigerSharp[j-1].value;
				trigerSharp[index].messageId=trigerSharp[j-1].messageId;
				trigerSharp[j-1].pin=-1;
				return;
			}
		}
		//tableau de trigger remplis remplace index par le dernier delete le dernier
		trigerSharp[index].pin=trigerSharp[NB_SHARP-1].pin;
		trigerSharp[index].value=trigerSharp[NB_SHARP-1].value;
		trigerSharp[index].messageId=trigerSharp[NB_SHARP-1].messageId;
		trigerSharp[NB_SHARP-1].pin=-1;
}

int getMicroSwitch(unsigned char pin){
	switch (pin)
  	{
		case 0:
			if(digitalRead(PIN_MS_AV)==HIGH) return 1; else return 0;
		case 1:
			if(digitalRead(PIN_MS_AR)==HIGH) return 1; else return 0;
		default:
			return 2;
	}
}

int setTriggerMS(unsigned int id, unsigned char pin,bool ref){
	if(pin > NB_MS)
		for(int i=0;i<NB_MS;i++){
		if(trigerMS[i].pin==pin){
			sendMessage(trigerMS[i].messageId,0);
			trigerMS[i].value=ref;
			trigerMS[i].messageId=id;
			return 1;
		}
		if(trigerMS[i].pin==-1){
			trigerMS[i].pin=pin;
			trigerMS[i].value=ref;
			trigerMS[i].messageId=id;
			return 1;
		}
	}
	return 0;
}

void removeTriggerMS(unsigned int index){
		// aucun trigger
		if(trigerMS[0].pin==-1) return;
		//1 trigger
		if(trigerMS[1].pin==-1){
			trigerMS[1].pin=-1;//index == 1 ici j'espere
			return;
		}
		//au moin 2 trigger
		for(int j=2;j<NB_MS;j++){
			if(trigerMS[j].pin==-1){
				//remplace le trigger index par le dernier trigger
				trigerMS[index].pin=trigerMS[j-1].pin;
				trigerMS[index].value=trigerMS[j-1].value;
				trigerMS[index].messageId=trigerMS[j-1].messageId;
				trigerMS[j-1].pin=-1;
				return;
			}
		}
		//tableau de trigger remplis remplace index par le dernier delete le dernier
		trigerMS[index].pin=trigerMS[NB_SHARP-1].pin;
		trigerMS[index].value=trigerMS[NB_SHARP-1].value;
		trigerMS[index].messageId=trigerMS[NB_SHARP-1].messageId;
		trigerMS[NB_SHARP-1].pin=-1;
}

void sensorTrigger(){
	//sharp
	for(int i=0;i<NB_SHARP;i++){
		if(trigerSharp[i].pin==-1)break;//fin des trigger
		if(getSharp(trigerSharp[i].pin)<=trigerSharp[i].value){
			sendMessage(trigerSharp[i].messageId,2);
			removeTriggerSharp(i);
		}
	}
	//microswitch
	for(int i=0;i<NB_MS;i++){
		if(trigerSharp[i].pin==-1)break;
		if(getMicroSwitch(trigerMS[i].pin)==trigerMS[i].value){
			sendMessage(trigerMS[i].messageId,2);
			removeTriggerMS(i);
		}
	}
}


int getColor()
{
	int c = digitalRead(PIN_COLOR);
	if (c == LOW)
		return BLEU;
	else
		return ROUGE;
}




