#include "tourelle.h"

Stepper motor1(2);

int x = 225, y = 17, angleR = 0;

int resolution = 10;
int right = motor1.position + resolution;
int left = motor1.position - resolution;
int gauche = DISTANCE_UNDEFINED, droite = DISTANCE_UNDEFINED;


int setupTourelle() {
	Serial.begin(9600);           // set up Serial library at 9600 bps
	Serial.println("Stepper");
	resolution=10;
	right=motor1.position+resolution;
	left=motor1.position-resolution;
	gauche = 0;
	droite = 0;	
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
    delay(500);
    getDistance(PIN_PING_GAUCHE);
    return 0;
}

void updatePos(int xUpdate, int yUpdate, int angleRUpdate)
{
	x = xUpdate;
	y = yUpdate;
	angleR = angleRUpdate;
}

int traiterCible (int angleT, int distance)
{
	int result = distance;
	int obj_coord[2];
	if (distance != 0)
	{
	    // Modulo 360 pour le cas où la tourelle fait un tour
	    int angleTDeg = floor(0.36 * angleT);
	    int angleTDegMod;
	    if (angleTDeg < 0)
			angleTDegMod = -(abs(angleTDeg) % 360);
		else
			angleTDegMod = angleTDeg % 360;
		int angleDeg = angleR + angleTDegMod;
		float angleRad = angleDeg * 0.0175; 
		int obj_x = floor(distance * cos(angleRad)) + x;
		int obj_y = floor(distance * sin(angleRad)) + y;

		if(obj_x > (300 - MARGE_BORD) || obj_x < MARGE_BORD)
		{
			result = DISTANCE_OUTOFRANGE;
			/*Serial.println("*****************************************");
			Serial.print("En dehors limites (largeur, ");
			Serial.print(distance, DEC);
			Serial.println(").");
			Serial.println("*****************************************");*/
		}

		else if (obj_y > (210 - MARGE_BORD) || obj_y < MARGE_BORD)
		{
			result = DISTANCE_OUTOFRANGE;
			/*Serial.println("*****************************************");
			Serial.print("En dehors limites (hauteur, ");
			Serial.print(distance, DEC);
			Serial.println(").");
			Serial.println("*****************************************");*/
		}
		
		else
		{
			obj_coord[0] = obj_x;
			obj_coord[1] = obj_y;
			sendMessage(Q_OBJECT_DETECTED, obj_coord, 2);
		}
	}
		
	return result;
}

int testPing(int pin){
	int tmp = microsecondsToCentimeters(getDistance(pin));
	if(tmp <= MAXDIST)
		return tmp;
	else return 0;
}

void loopTourelle() 
{
	int distance = 0;
	static long long oldMillis = 0;
	static int pingTurn = RIGHT_PING;
	
	if ((millis() - oldMillis) > TEMPO)
	{
		oldMillis = millis();
		if (pingTurn == RIGHT_PING) {
			gauche = traiterCible(motor1.position, testPing(PIN_PING_GAUCHE));
			pingTurn = LEFT_PING;
			//Serial.println("RIGHT_PING");
		} else {
			droite = traiterCible(motor1.position, testPing(PIN_PING_DROITE));
			pingTurn = RIGHT_PING;
			//Serial.println("LEFT_PING");
		}
		
		if (gauche != DISTANCE_UNDEFINED && droite != DISTANCE_UNDEFINED)
		{
			/*Serial.print("gauche = ");
			Serial.println(gauche, DEC);
			Serial.print("droite = ");
			Serial.println(droite, DEC);
			Serial.print("position = ");
			Serial.println(motor1.position, DEC);
			Serial.print("\n");*/
			
			bool objGauche = (gauche > 0);
			bool objDroite = (droite > 0);

			/*
			 * Un objet est détecté de chaque côté mais à des distances 
			 * différentes.
			 */
			if (objDroite && objGauche && (abs(droite - gauche) > DIFFMAX))
			{
				if (droite < gauche)
				{
					motor1.dir = TURNRIGHT;
					motor1.steps = 50;
				}
				else
				{
					motor1.dir = TURNLEFT;
					motor1.steps = 50;
				}
			}
			
			/*
			 * Un objet est détecté de chaque côté à des distances
			 * équivalentes.
			 */
			else if (objDroite && objGauche)
			{
				distance = (gauche + droite) / 2;
			}
			
			/*
			 *  Un objet est détecté à gauche.
			 */
			else if (objGauche) 
			{
				motor1.dir = TURNLEFT;
				motor1.steps = 50;
			}
			
			/*
			 * Un objet est détecté à droite
			 */
			else if (objDroite)
			{
				motor1.dir = TURNRIGHT;
				motor1.steps = 50;
			}
			
			/*
			 * Aucun objet détecté
			 */
			else
			{
				motor1.steps = 100;
			}
			
			motor1.run();
		}
	}
 }


