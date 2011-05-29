#include "tourelle.h"

Tourelle tourrelle(225, 17, 0);

/*
 * Global
 */
int setupTourelle()
{
	return tourelle.setup();
}

void loopTourelle()
{
	tourelle.loop();
}

void update_pos()
{
	tourelle.update_pos();
}

/*
 * Stepper
 */
Stepper::run()
{
	if (!stopped)
	{
		onestep(_dir, _style);
		if (dir == TURNLEFT)
			++_position;
		else
			--_position;
		--_steps;
	}
}

Stepper::Stepper : 
	AF_Stepper(NB_PAS, ID)
{
	setSpeed(SPEED);
	_steps = 0;
	_dir = TURNLEFT;
	_style = STYLE;
	_position = 0;
	release();
}

/*
 * Ping
 */
Ping::Ping(int pin, int maxDist) :
	_pin(pin),
	_maxDist(maxDist)
{
} 

int Ping::sendPing()
{
	int tmp = microsecondsToCentimeters(getDistance(_pin));
	if(tmp <= _maxDist)
		return tmp;
	else 
		return 0;
}

/*
 * Tourelle
 */
 
/*
obj_coord[0] = obj_x;
obj_coord[1] = obj_y;
sendMessage(Q_OBJECT_DETECTED, obj_coord, 2);
*/

Tourelle::Tourelle(int robot_pos_x, int robot_pos_y, int robot_angle)
	_robot_pos_x(robot_pos_x),
	_robot_pos_y(robot_pos_y),
	_robot_angle(robot_angle),
	_distanceGauche(DISTANCE_UNDEFINED),
	_distanceDroite(DISTANCE_UNDEFINED),
	_obj_x(COORD_UNDEFINED),
	_obj_y(COORD_UNDEFINED),
	_mode(None),
	_motor(2),
	_leftPing(PIN_PING_GAUCHE, MAXDIST),
	_rightPing(PIN_PING_DROITE, MAXDIST),
	_millitime(0),
	_pingTurn(RightPing)
{
}

int Tourelle::setup()
{
	Serial.begin(9600);           // set up Serial library at 9600 bps
	Serial.println("Stepper");
	long long startTime = millis();
	while(Serial.available() != 0)
	{
		Serial.read();
		if ((millis() - startTime) > SETUP_TIMEOUT)
		{
			sendMessage(E_TOURELLE_SETUP_FAIL, "Erreur setupTourelle().");
			return E_TOURELLE_SETUP_FAIL;
		}
    }
    _leftPing.sendPing();
    delay(1000);
    _rightPing.sendPing();
    return 0;
}

void Tourelle::update_pos(int xUpdate, int yUpdate, int angleRUpdate)
{
	robot_pos_x = xUpdate;
	robot_pos_y = xUpdate;
	robot_angle = angleRUpdate;
}

TargetCoord Tourelle::findTargetCoord(int angleT, int distance)
{
	TargetCoord targetCoord;
	
	// Modulo 360 pour le cas où la tourelle fait un tour
	int angleTDeg = floor(0.36 * angleT);
	int angleTDegMod;
	if (angleTDeg < 0)
		angleTDegMod = -(abs(angleTDeg) % 360);
	else
		angleTDegMod = angleTDeg % 360;
	int angleDeg = robot_angle + angleTDegMod;
	float angleRad = angleDeg * 0.0175; 
	targetCoord.x = floor(distance * cos(angleRad)) + robot_pos_x;
	targetCoord.y = floor(distance * sin(angleRad)) + robot_pos_y;
	
	return targetCoord;
}

bool Tourelle::targetInRange(TargetCoord targetCoord)
{
	if(targetCoord.x > (300 - MARGE_BORD) || targetCoord.x < MARGE_BORD)
	{
		Serial.println("*****************************************");
		Serial.print("En dehors limites (largeur, ");
		Serial.print(distance, DEC);
		Serial.println(").");
		Serial.println("*****************************************");
		return false;
	}

	else if (targetCoord.y > (210 - MARGE_BORD) || targetCoord.y < MARGE_BORD)
	{
		Serial.println("*****************************************");
		Serial.print("En dehors limites (hauteur, ");
		Serial.print(distance, DEC);
		Serial.println(").");
		Serial.println("*****************************************");
		return true;
	}
	
	return true;
}

void Tourelle::loop() 
{	
	measureDistances();
	
	if (_distanceGauche != DISTANCE_UNDEFINED && _distanceDroite != DISTANCE_UNDEFINED)
	{
		chooseMode();
		setMotorParameters();
		_distanceGauche = DISTANCE_UNDEFINED;
		_distanceDroite = DISTANCE_UNDEFINED;
	}
	motor.run();
 }

void Tourelle::measureDistances()
{
	if ((millis() - _millitime) > TEMPO)
	{
		_millitime = millis();
		if (_pingTurn == RightPing)
		{
			_distanceGauche = leftPing.sendPing();
			pingTurn = LeftPing;
			Serial.println("RIGHT_PING");
		} 
		else 
		{
			_distanceDroite = rightPing.sendPing();
			_pingTurn = RightPing;
			Serial.println("LEFT_PING");
		}
	}
}

void Tourelle::chooseMode()
{
	TargetCoord leftTargetCoord;
	TargetCoord rightTargetCoord;
	
	Serial.print("gauche = ");
	Serial.println(_distanceGauche, DEC);
	Serial.print("droite = ");
	Serial.println(_distanceDroite, DEC);
	Serial.print("position = ");
	Serial.println(motor.position, DEC);
	Serial.print("\n");
	
	// Objet détecté à gauche
	bool objGaucheValide = _distanceGauche > 0 && targetInRange(findTargetCoord(motor.position, _distanceGauche));
	bool objDroiteValide = _distanceDroite > 0 && targetInRange(findTargetCoord(motor.position, _distanceGauche));

	// Si un objet valide à droite et que l'objet à gauche est valide mais celui de droite est plus près, ou bien l'objet gauche invalide.
	if (objDroiteValide && ((objGaucheValide && (_distanceDroite < _distanceGauche)) || !objGaucheValide))
		_mode = TurnRight;
	
	// Sinon si l'objet gauche était valide
	else if (objGaucheValide)
		motor1.steps = 0;
	
	/*
	 *  Un objet est détecté à gauche.
	 */
	else if (objGauche)
	{
		motor1.dir = TURNLEFT;
	}
	
	/*
	 * Un objet est détecté à droite
	 */
	else if (objDroite)
	{
		motor1.dir = TURNRIGHT;
		motor1.steps = 10;
	}
}

void Tourelle::setMotorParameters()
{
}
