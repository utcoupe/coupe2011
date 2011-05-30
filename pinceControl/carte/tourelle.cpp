#include "tourelle.h"

static Tourelle tourelle(205, 90, 90);

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

void changerSensTourelle(int sens)
{
	tourelle.setSens(sens);	
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
	step(steps, dir, style);
	if (dir == TURNLEFT)
		position += steps;
	else
		position -= steps;
	steps = 0;
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
	_pingTurn(RightPing),
	_sens(FACEAV)
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

void Tourelle::findTargetCoord(int angleT, int distance, int* target_x, int* target_y)
{
	// Modulo 360 pour le cas où la tourelle fait un tour
	int angleTDeg = floor(0.36 * angleT);
	int angleTDegMod;
	if (angleTDeg < 0)
		angleTDegMod = -(abs(angleTDeg) % 360);
	else
		angleTDegMod = angleTDeg % 360;
	int angleDeg = _robot_angle + angleTDegMod;
	float angleRad = angleDeg * 0.0175; 
	*target_x = floor(distance * cos(angleRad)) + _robot_pos_x;
	*target_y = floor(distance * sin(angleRad)) + _robot_pos_y;
}

bool Tourelle::targetInRange(int target_x, int target_y)
{
	if(target_x > (300 - MARGE_BORD) || target_x < MARGE_BORD)
	{
		return false;
	}

	else if (target_y > (210 - MARGE_BORD) || target_y < MARGE_BORD)
	{
		return false;
	}
	
	return true;
}

void Tourelle::loop() 
{	
	measureDistances();
	
	if (_distanceGauche != DISTANCE_UNDEFINED && _distanceDroite != DISTANCE_UNDEFINED)
	{
		if (_distanceGauche <= DISTANCE_DETECT)
			sendMessage(W_ROBOT_DETECTED, _distanceGauche);
		else if (_distanceDroite <= DISTANCE_DETECT)
			sendMessage(W_ROBOT_DETECTED, _distanceDroite);
		
		//chooseMode();
		//setMotorParameters();
		_distanceGauche = DISTANCE_UNDEFINED;
		_distanceDroite = DISTANCE_UNDEFINED;
		//_motor.run();
	}
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
			Serial.print("gauche = ");
			Serial.println(_distanceGauche, DEC);
		} 
		else 
		{
			_distanceDroite = _rightPing.sendPing();
			_pingTurn = RightPing;
			Serial.print("droite = ");
			Serial.println(_distanceDroite, DEC);
		}
	}
}

void Tourelle::chooseMode()
{
	int leftTarget_x;
	int leftTarget_y;
	int rightTarget_x;
	int rightTarget_y;
	
	// Vérification des distances
	findTargetCoord(_motor.position, _distanceGauche, &leftTarget_x, &leftTarget_y);
	findTargetCoord(_motor.position, _distanceDroite, &rightTarget_x, &rightTarget_y);
	bool objGaucheValide = _distanceGauche!=0 && targetInRange(leftTarget_x, leftTarget_y);
	bool objDroiteValide = _distanceDroite!=0 && targetInRange(rightTarget_x, rightTarget_y);
	
	Serial.print("position = ");
	Serial.println(_motor.position, DEC);

	// Si les deux objets perçus sont valides.
	if (objGaucheValide && objDroiteValide)
	{
		// La différence entre les deux distances est trop grande.
		if (abs(_distanceGauche - _distanceDroite) > DIFFMAX)
		{
			if (_distanceGauche < _distanceDroite)
				_mode = TurnLeft;
			else
				_mode = TurnRight;
		}
		
		// Sinon la différence est inférieure à DIFFMAX, on fixe l'objet.
		else
			_mode = Fixe;
	}
	
	// Si seulement l'objet gauche est valide.
	else if (objGaucheValide)
		_mode = TurnLeft;
	
	// Si seulement l'objet droite est valide.
	else if (objDroiteValide)
		_mode = TurnRight;

	// Si aucun n'est valide.
	else if (!objDroiteValide && !objGaucheValide)
		_mode = Recherche;
		
	// Cas d'erreur.
	else
		_mode = None;
		
	// Debug
	switch (_mode)
	{
		case TurnLeft:
			Serial.print("x = ");
			Serial.println(leftTarget_x, DEC);
			Serial.print("y = ");
			Serial.println(leftTarget_y, DEC);
			Serial.print("distance = ");
			Serial.println(_distanceGauche, DEC);
			break;
		case TurnRight:
			Serial.print("x = ");
			Serial.println(rightTarget_x, DEC);
			Serial.print("y = ");
			Serial.println(rightTarget_y, DEC);
			Serial.print("distance = ");
			Serial.println(_distanceDroite, DEC);
			break;
		case Fixe:	
			Serial.print("x = ");
			Serial.println(rightTarget_x, DEC);
			Serial.print("y = ");
			Serial.println(rightTarget_y, DEC);
			Serial.print("distance = ");
			Serial.println(_distanceDroite, DEC);				
			break;
	}
}

void Tourelle::setMotorParameters()
{
  Serial.print("Mode : ");
  switch (_mode)
  {	  
    case TurnLeft:
	  if(_motor.position>1200)
	  {
		  _motor.steps = 1000;
		  _motor.dir = TURNRIGHT;
	  }
	  else
	  {
		  _motor.steps = 25;
		  _motor.dir = TURNLEFT;
	  }
	  Serial.println("LEFT\n");
      break;

    case TurnRight:
	  if(_motor.position<-1200)
	  {
		  _motor.steps = 1000;
		  _motor.dir = TURNLEFT;
	  }
	  else
	  {
		  _motor.steps = 25;
		  _motor.dir = TURNRIGHT;
	  }
      Serial.println("RIGHT\n");
      break;

    case Fixe:
      _motor.steps = 0;
      Serial.println("FIXE\n");
      break;

    case Recherche:
	  Serial.println("RECHERCHE\n");
      _motor.steps = 75;
      if (_motor.position > 800)
        _motor.dir = TURNRIGHT;
      else if (_motor.position < -800)
        _motor.dir = TURNLEFT;
      break;
   
	case None:
	  Serial.println("NONE\n");
	  _motor.steps = 0;
	  break;

    default:
	  Serial.println("ERROR\n");
      _motor.steps = 0;
      break;
	}
}

void Tourelle::setSens(int sens)
{
	if (sens != _sens)
	{
		_motor.steps = 500;
		_sens = sens;
		if (sens = FACEAV)
			_motor.dir = TURNRIGHT;
		else
			_motor.dir = TURNLEFT;
	}
}
