#include "tourelle.h"
#include "sensor.h"

Stepper motor(2);
int faceTourelleActuel;
int faceTourelleDemande;
bool tourelleActive;

int setupTourelle() {
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
			motor.dir = TURNRIGHT;
			motor.steps = 500;
			motor.run();
			faceTourelleActuel=FACEAV;
		}
	}
}

void loopTourelle() 
{	
	static long long timeLastPing=0;
	static bool pingTurnGauche=true;
	static long long timeLastSend=0;
	int d_gauche = 1337;
	int d_droite = 1337;
	
	if(tourelleActive)
	{
		if ((millis() - timeLastPing) > TEMPO)
		{
			timeLastPing=millis();
			if(pingTurnGauche)
			{
				d_gauche = testPing(PIN_PING_GAUCHE);	
				pingTurnGauche=false;
			}
			else
			{
				d_droite = testPing(PIN_PING_DROITE);
				pingTurnGauche=true;
			}
		}
	
		if(faceTourelleActuel==FACEAV && faceTourelleDemande==FACEAR)
		{
			motor.dir = TURNLEFT;
			motor.steps = 500;
			motor.run();
			faceTourelleActuel=FACEAR;
		}
		else if(faceTourelleActuel==FACEAR && faceTourelleDemande==FACEAV)
		{
			motor.dir = TURNRIGHT;
			motor.steps = 500;
			motor.run();
			faceTourelleActuel=FACEAV;
		}
		else if(millis() - timeLastSend > 200 and (d_gauche < 40 || d_droite < 40))
		{
			timeLastSend = millis();
			sendMessage(W_PING_AV, min(d_gauche, d_droite) );
		}
	}
 }


