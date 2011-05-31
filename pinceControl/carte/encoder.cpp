#include "encoder.h"

int state_AR_pinA;
int state_AR_pinB;
int state_AV_pinA;
int state_AV_pinB;

long value_AR_enc;//AR left
long value_AV_enc;//AV right
long goal_position_AV,goal_position_AR;

void arretMsgAV(){
	setAVPWM(0x00);
	if(msg_position_AV!=-1){
		sendMessage(msg_position_AV,2);
		msg_position_AV=-1;
	}
}

void arretMsgAR(){
	setARPWM(0x00);
	if(msg_position_AR!=-1){
		sendMessage(msg_position_AR,2);
		msg_position_AR=-1;
	}
}


void testAV()
{
	if (goal_position_AV < 0)
		return;
	if (value_AV_enc < goal_position_AV-MARGE_MARCHE)
		setAVPWM(PWM_MOVE);
	else if (value_AV_enc < goal_position_AV-MARGE_MAINTIENT)
		setAVPWM(PWM_MAINTIENT);
	else if (value_AV_enc > goal_position_AV+MARGE_MARCHE)
		setAVPWM(-PWM_MOVE);
	else if (value_AV_enc > goal_position_AV+MARGE_MAINTIENT)
		setAVPWM(-PWM_MAINTIENT);
	else //arret
		arretMsgAV();
		
	if (digitalRead(PIN_MS_RECAL_AV_HAUT) == HIGH)
	{
		value_AV_enc = POSITION_MAX;
		//sendMessage(-101, (char*)"ar haut");
		//arretMsgAV();

	}else if (digitalRead(PIN_MS_RECAL_AV_BAS) == HIGH)
	{
		value_AV_enc = 0;
		//sendMessage(-101, (char*)"ar haut");
		//arretMsgAV();
	}
		
}

void testAR()
{
	if (goal_position_AR < 0)
		return;
	if (value_AR_enc < goal_position_AR-MARGE_MARCHE)
		setARPWM(PWM_MOVE);
	else if (value_AR_enc < goal_position_AR-MARGE_MAINTIENT)
		setARPWM(PWM_MAINTIENT);
	else if (value_AR_enc > goal_position_AR+MARGE_MARCHE)
		setARPWM(-PWM_MOVE);
	else if (value_AR_enc > goal_position_AR+MARGE_MAINTIENT)
		setARPWM(-PWM_MAINTIENT);
	else //arret
		arretMsgAR();
		
	if (digitalRead(PIN_MS_RECAL_AR_HAUT) == HIGH)
	{
		value_AR_enc = POSITION_MAX;
		//sendMessage(-101, (char*)"ar haut");
		//arretMsgAR();

	}else if (digitalRead(PIN_MS_RECAL_AR_BAS) == HIGH)
	{
		value_AR_enc = 0;
		//sendMessage(-101, (char*)"ar haut");
		//arretMsgAR();
	}
}

/**
 * Test si les ascenseurs bloquent
 * Envoie le message E_BLOCK si un ascneseur bloque
 */
void encoderSafe()
{
	/*static int last_left_enc_value = 0;
	//static int last_right_enc_value = 0;
	
	if (goal_position_AR >= 0
		and ((value_AR_enc < goal_position_AR-MARGE_MAINTIENT)
		or (value_AR_enc > goal_position_AR+MARGE_MAINTIENT)))
	{
		if (abs(last_left_enc_value - value_AR_enc) < 10)
		{
			goal_position_AV = -1;
			setARPWM(0x00);
			sendMessage(E_BLOCK,E_BLOCK);
		}
	}*/
	
	/*if (goal_position_AV >= 0
		and ((value_AV_enc < goal_position_AV-MARGE_MAINTIENT)
		or (value_AV_enc > goal_position_AV+MARGE_MAINTIENT)))
	{
		if (abs(last_right_enc_value - value_AV_enc) < 10)
		{
			goal_position_AR = -1;
			setAVPWM(0x00);
			sendMessage(E_BLOCK,E_BLOCK);
		}
	}*/
}

void initEncoders(){
	value_AR_enc = 0;
	value_AV_enc = 0;

	pinMode(PIN_AR_A,INPUT);
	pinMode(PIN_AR_B,INPUT);
	pinMode(PIN_AV_A,INPUT);
	pinMode(PIN_AV_B,INPUT);
	
	pinMode(PIN_MS_RECAL_AV_HAUT,INPUT);
	pinMode(PIN_MS_RECAL_AV_BAS,INPUT);
	pinMode(PIN_MS_RECAL_AR_HAUT,INPUT);
	pinMode(PIN_MS_RECAL_AR_BAS,INPUT);

	digitalWrite(PIN_AR_A, HIGH);
	digitalWrite(PIN_AR_B, HIGH);
	digitalWrite(PIN_AV_A, HIGH);
	digitalWrite(PIN_AV_B, HIGH);

	state_AR_pinA = digitalRead(PIN_AR_A);
	state_AR_pinB = digitalRead(PIN_AR_B);
	state_AV_pinA = digitalRead(PIN_AV_A);
	state_AV_pinB = digitalRead(PIN_AV_B);

	attachInterrupt(INTERRUPT_AR_A,valueChangeOnEncoderARPinA,CHANGE);
	attachInterrupt(INTERRUPT_AV_B,valueChangeOnEncoderARPinB,CHANGE);
	attachInterrupt(INTERRUPT_AV_A,valueChangeOnEncoderAVPinA,CHANGE);
	attachInterrupt(INTERRUPT_AV_B,valueChangeOnEncoderAVPinB,CHANGE);
	/*attachInterrupt(INTERRUPT_LEFT_A,valueChangeOnMSRecalARHaut,CHANGE);
	attachInterrupt(INTERRUPT_AV_B,valueChangeOnMSRecalARBas,CHANGE);
	attachInterrupt(INTERRUPT_RIGHT_A,valueChangeOnMSRecalAVHaut,CHANGE);
	attachInterrupt(INTERRUPT_RIGHT_B,valueChangeOnMSRecalAVBas,CHANGE);*/
}


void valueChangeOnEncoderARPinA(){
	int new_state = digitalRead(PIN_AR_A);

	if(new_state == HIGH)
		if(state_AR_pinB == HIGH)
			value_AR_enc--;
		else
			value_AR_enc++;

	else
		if(state_AR_pinB == HIGH)
			value_AR_enc++;
		else
			value_AR_enc--;

	state_AR_pinA = new_state;
	testAR();
}

void valueChangeOnEncoderARPinB(){
	int new_state = digitalRead(PIN_AR_B);

	if(new_state == HIGH)
		if(state_AR_pinA == HIGH)
			value_AR_enc++;
		else
			value_AR_enc--;

	else
		if(state_AR_pinA == HIGH)
			value_AR_enc--;
		else
			value_AR_enc++;

	state_AR_pinB = new_state;
}

void valueChangeOnEncoderAVPinA(){
	int new_state = digitalRead(PIN_AV_A);

	if(new_state == HIGH)
		if(state_AV_pinB == HIGH)
			value_AV_enc--;
		else
			value_AV_enc++;

	else
		if(state_AV_pinB == HIGH)
			value_AV_enc++;
		else
			value_AV_enc--;

	state_AV_pinA = new_state;
	testAV();
}

void valueChangeOnEncoderAVPinB(){
	int new_state = digitalRead(PIN_AV_B);

	if(new_state == HIGH)
		if(state_AV_pinA == HIGH)
			value_AV_enc++;
		else
			value_AV_enc--;

	else
		if(state_AV_pinA == HIGH)
			value_AV_enc--;
		else
			value_AV_enc++;

	state_AV_pinB = new_state;
}


/*void valueChangeOnMSRecalAVBas()
{
	delay(500);
	
	if (digitalRead(PIN_MS_RECAL_AV_BAS) == HIGH)
	{
		value_AV_enc = 0;
		sendMessage(-101, (char*)"av bas");
		setAVPWM(0x00);
		if(msg_position_AV != -1){
			sendMessage(msg_position_AV,2);
			msg_position_AV=-1;
		}
	}
}


void valueChangeOnMSRecalAVHaut()
{
	delay(500);
	
	if (digitalRead(PIN_MS_RECAL_AV_HAUT) == HIGH)
	{
		value_AV_enc = POSITION_MAX;
		sendMessage(-101, (char*)"av haut");
		setAVPWM(0x00);
		if(msg_position_AV != -1){
			sendMessage(msg_position_AV,2);
			msg_position_AV=-1;
		}
	}
}


void valueChangeOnMSRecalARBas()
{
	delay(500);
	
	if (digitalRead(PIN_MS_RECAL_AR_BAS) == HIGH)
	{
		value_AR_enc = 0;
		sendMessage(-101, (char*)"ar bas");
		setARPWM(0x00);
		if(msg_position_AR != -1){
			sendMessage(msg_position_AR,2);
			msg_position_AR=-1;
		}
	}
}


void valueChangeOnMSRecalARHaut()
{
	delay(500);
	
	if (digitalRead(PIN_MS_RECAL_AR_HAUT) == HIGH)
	{
		value_AR_enc = POSITION_MAX;
		sendMessage(-101, (char*)"ar haut");
		setARPWM(0x00);
		if(msg_position_AR != -1){
			sendMessage(msg_position_AR,2);
			msg_position_AR=-1;
		}
	}
}*/
