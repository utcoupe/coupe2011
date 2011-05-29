#include "tourelle.h"

static Tourelle tourelle(225, 17, 0);

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

void update_pos(int xUpdate, int yUpdate, int angleRUpdate)
{
	tourelle.update_pos(xUpdate, yUpdate, angleRUpdate);
}

/*
 * Stepper
 */
Stepper::Stepper(int ID) : 
	AF_Stepper(NB_PAS, ID)
{
	setSpeed(SPEED);
	steps = 0;
	dir = TURNLEFT;
	style = STYLE;
	position = 0;
	release();
}

void Stepper::run()
{
	onestep(dir, style);
	if (dir == TURNLEFT)
		++position;
	else
		--position;
	--steps;
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

Tourelle::Tourelle(int robot_pos_x, int robot_pos_y, int robot_angle) :
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
	_robot_pos_x = xUpdate;
	_robot_pos_y = xUpdate;
	_robot_angle = angleRUpdate;
}

Tourelle::TargetCoord Tourelle::findTargetCoord(int angleT, int distance)
{
	TargetCoord targetCoord;
	
	// Modulo 360 pour le cas où la tourelle fait un tour
	int angleTDeg = floor(0.36 * angleT);
	int angleTDegMod;
	if (angleTDeg < 0)
		angleTDegMod = -(abs(angleTDeg) % 360);
	else
		angleTDegMod = angleTDeg % 360;
	int angleDeg = _robot_angle + angleTDegMod;
	float angleRad = angleDeg * 0.0175; 
	targetCoord.x = floor(distance * cos(angleRad)) + _robot_pos_x;
	targetCoord.y = floor(distance * sin(angleRad)) + _robot_pos_y;
  targetCoord.distance = distance;
	
	return targetCoord;
}

bool Tourelle::targetInRange(TargetCoord targetCoord)
{
	if(targetCoord.x > (300 - MARGE_BORD) || targetCoord.x < MARGE_BORD)
	{
		Serial.println("*****************************************");
		Serial.print("En dehors limites (largeur, ");
		Serial.print(targetCoord.distance, DEC);
		Serial.println(").");
		Serial.println("*****************************************");
		return false;
	}

	else if (targetCoord.y > (210 - MARGE_BORD) || targetCoord.y < MARGE_BORD)
	{
		Serial.println("*****************************************");
		Serial.print("En dehors limites (hauteur, ");
		Serial.print(targetCoord.distance, DEC);
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
	_motor.run();
 }

void Tourelle::measureDistances()
{
	if ((millis() - _millitime) > TEMPO)
	{
		_millitime = millis();
		if (_pingTurn == RightPing)
		{
			_distanceGauche = _leftPing.sendPing();
			_pingTurn = LeftPing;
			Serial.println("RIGHT_PING");
		} 
		else 
		{
			_distanceDroite = _rightPing.sendPing();
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
	Serial.println(_motor.position, DEC);
	Serial.print("\n");
	
	// Objet détecté à gauche
	bool objGaucheValide = _distanceGauche > 0 && targetInRange(findTargetCoord(_motor.position, _distanceGauche));
	bool objDroiteValide = _distanceDroite > 0 && targetInRange(findTargetCoord(_motor.position, _distanceGauche));

	/*
   * Si un objet valide à droite et que l'objet à gauche est valide mais celui de droite est plus près, ou bien l'objet gauche invalide.
   */
	if (objDroiteValide && 
        ((objGaucheValide && 
          (_distanceDroite < _distanceGauche) && (_distanceGauche - _distanceDroite > DIFFMAX) || !objGaucheValide)))
		_mode = TurnRight;
	
	/*
	 *  Sinon si un objet à gauche est valide et qu'il est plus près de celui de droite.
	 */
	else if (objGaucheValide && (_distanceGauche < _distanceDroite) && (_distanceDroite - _distanceGauche > DIFFMAX))
		_mode = TurnLeft;
	
	/*
   * Sinon si l'objet gauche est valide et que l'objet droite est valide aussi.
   */
	else if (objGaucheValide && objDroiteValide)
		_mode = Fixe;
	
	/*
	 * Sinon on recherche.
	 */
	else
	  _mode = Recherche;
}

void Tourelle::setMotorParameters()
{
  switch (_mode)
  {
    case TurnLeft:
      _motor.steps = 50;
      _motor.dir = TURNLEFT;
      break;

    case TurnRight:
      _motor.steps = 50;
      _motor.dir = TURNRIGHT;
      break;

    case Fixe:
      break;

    case Recherche:
      _motor.steps = 50;
      if (_motor.position > 0)
        _motor.dir = TURNLEFT;
      else
        _motor.dir = TURNRIGHT;
      break;

    default:
      _motor.steps = 0;
      break;
  }
}
