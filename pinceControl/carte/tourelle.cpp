#include "tourelle.h"
#include "sensor.h"

Stepper motor1(1);
int faceTourelleActuel;
int faceTourelleDemande;
bool tourelleActive;
int gauche, droite;

int setupTourelle() {
	gauche = 1337;
	droite = 1337;	
	long long startTime = millis();
	while(Serial.available() != 0){
		Serial.read();
		if ((millis() - startTime) > SETUP_TIMEOUT)
		{
			sendMessage(E_TOURELLE_SETUP_FAIL, "Erreur setupTourelle().");
			return E_TOURELLE_SETUP_FAIL;
		}
    }
    
    getDistance(PIN_PING_GAUCHE);
    delay(50);
    getDistance(PIN_PING_GAUCHE);
    faceTourelleActuel=FACEAV;
    faceTourelleDemande=FACEAV;
    tourelleActive=false;
    return 0;
}

int testPing(int pin){
	int tmp = microsecondsToCentimeters(getDistance(pin));
	if(tmp <= MAXDIST)
		return tmp;
	else return DISTANCE_OUTOFRANGE;
}

void updatePosition(int faceUpdate)
{
	if(faceUpdate!=-1)
	{
		tourelleActive=true;
		faceTourelleDemande=faceUpdate;
	}
	
	else 
	{
		tourelleActive=false;
		if(faceTourelleActuel==FACEAR)
		{
			motor1.dir = TURNRIGHT;
			motor1.steps = 500;
			motor1.run();
			faceTourelleActuel=FACEAV;
		}
	}
}

void loopTourelle() 
{	
	static long long startTime=0;
	static bool pingTurnGauche=true;
	if(tourelleActive)
	{
		if ((millis() - startTime) > TEMPO)
		{
			startTime=millis();
			if(pingTurnGauche)
			{
				gauche = testPing(PIN_PING_GAUCHE);	
				pingTurnGauche=false;
			}
			else
			{
				droite = testPing(PIN_PING_DROITE);
				pingTurnGauche=true;
			}
		}
	
		if(faceTourelleActuel==FACEAV && faceTourelleDemande==FACEAR)
		{
			motor1.dir = TURNLEFT;
			motor1.steps = 500;
			motor1.run();
			faceTourelleActuel=FACEAR;
		}
		else if(faceTourelleActuel==FACEAR && faceTourelleDemande==FACEAV)
		{
			motor1.dir = TURNRIGHT;
			motor1.steps = 500;
			motor1.run();
			faceTourelleActuel=FACEAV;
		}
		else if(gauche < 40 || droite < 40)
		{
			sendMessage(W_PING_AV, min(gauche, droite) );
		}
	}
 }


