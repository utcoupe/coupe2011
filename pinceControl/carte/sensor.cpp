#include "sensor.h"

void initSensor(){
	pinMode(PIN_MS_AV, INPUT);  
	pinMode(PIN_MS_AR, INPUT);
	for(int i=0;i<NB_SHARP;i++){
		trigerSharp[i].pin=-1;
	}
	for(int i=0;i<NB_MS;i++){
		trigerMS[i].pin=-1;
	}
	//waitJack();
}

int jack(int id){
	JackMessageID=id;
	sendMessage(id,1);
}

int waitJack(){
	while(JackMessageID==0){
		readIncomingData();
	}
	//attend le branchement d'un jack
	int msg[2];
	while(digitalRead(PIN_JACK_BLEU)!=HIGH || digitalRead(PIN_JACK_BLEU)!=HIGH){
		if(digitalRead(PIN_JACK_BLEU)==HIGH){
			msg[0]=SEND_COLOR;
			msg[1]=BLEU;
			sendMessage(0,msg,2);
		}
		else if(digitalRead(PIN_JACK_ROUGE)==HIGH){
			msg[0]=SEND_COLOR;
			msg[1]=ROUGE;
			sendMessage(0,msg,2);
		}
	}
	//attend le debranchement du jack
	if(msg[1]==BLEU){
		while(digitalRead(PIN_JACK_BLEU)!=LOW){}
		sendMessage(1,SEND_JACK);
		return 1;
	}else if(msg[1]==ROUGE){
		while(digitalRead(PIN_JACK_ROUGE)!=LOW){}
		sendMessage(1,SEND_JACK);
		return 1;
	}
	return 0;
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
	for(int i=0;i<NB_SHARP;i++){
		if(trigerSharp[i].pin==pin){
			sendMessage(trigerSharp[i].messageId,0);
			trigerSharp[i].value=ref;
			trigerSharp[i].messageId=id;
			return 1;
		}
		if(trigerSharp[i].pin==-1){
			trigerSharp[i].pin=pin;
			trigerSharp[i].value=ref;
			trigerSharp[i].messageId=id;
			return 1;
		}
			return trigerSharp[i].pin;
	}
	return 0;
}

void removeTriggerSharp(unsigned char index){
		for(int j=0;j<NB_SHARP;j++){
			if(trigerSharp[j].pin==-1){
				trigerSharp[index].pin=trigerSharp[j-1].pin;
				trigerSharp[index].value=trigerSharp[j-1].value;
				trigerSharp[index].messageId=trigerSharp[j-1].messageId;
				trigerSharp[j-1].pin=-1;
				return;
			}
		}
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
			return -1;
	}
}

int setTriggerMS(unsigned int id, unsigned char pin,bool ref){
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
		for(int j=0;j<NB_MS;j++){
			if(trigerMS[j].pin==-1){
				trigerMS[index].pin=trigerMS[j-1].pin;
				trigerMS[index].value=trigerMS[j-1].value;
				trigerMS[index].messageId=trigerMS[j-1].messageId;
				trigerMS[j-1].pin=-1;
				return;
			}
		}
		trigerMS[index].pin=trigerMS[NB_SHARP-1].pin;
		trigerMS[index].value=trigerMS[NB_SHARP-1].value;
		trigerMS[index].messageId=trigerMS[NB_SHARP-1].messageId;
		trigerMS[NB_SHARP-1].pin=-1;
}

void sensorTrigger(){
	//sharp
	for(int i=0;i<NB_SHARP;i++){
		if(trigerSharp[i].pin==-1)break;
		if(getSharp(trigerSharp[i].pin)<=trigerSharp[i].value){
			sendMessage(trigerSharp[i].messageId,1);
			removeTriggerSharp(i);
		}
	}
	//microswitch
	for(int i=0;i<NB_MS;i++){
		if(trigerSharp[i].pin==-1)break;
		if(getMicroSwitch(trigerMS[i].pin)==trigerMS[i].value){
			sendMessage(trigerMS[i].messageId,1);
			removeTriggerMS(i);
		}
	}
}



