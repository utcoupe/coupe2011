#include "tourelle.h"

Stepper motor1(2);

int x = 225, y = 17, angleR = 90;

int resolution = 10;
int right = motor1.position + resolution;
int left = motor1.position - resolution;
int gauche = 0, droite = 0;

void setupTourelle() {
	Serial.begin(9600);           // set up Serial library at 9600 bps
	//Serial.println("Stepper");
	resolution=10;
	right=motor1.position+resolution;
	left=motor1.position-resolution;
	gauche = 0;
	droite = 0;
    while(Serial.available() != 0){
		Serial.read();
    }
    getDistance(PIN_PING_GAUCHE);
    delay(1000);
    getDistance(PIN_PING_GAUCHE);
}

void update_pos(int xUpdate, int yUpdate, int angleRUpdate)
{
	x = xUpdate;
	y = yUpdate;
	angleR = angleRUpdate;
}

bool cibleValide(int angleTTest, int distance)
{
	bool valide = true;
  // Modulo 1000 pour le cas où la tourelle fait un tour, 18 et 50 le rapport de réduction des angre
	int angle = angleR + ((18*angleTTest/50) % 1000); 
	int A = distance * cos(angle) + x, B = distance * sin(angle) + y;

	if(A > (300 - MARGE_BORD) || A < MARGE_BORD)
		valide = false;

	if(B > (210 - MARGE_BORD) || B < MARGE_BORD)
		valide = false;
	
	return valide;
}

	
void recherche() {
	//if (motor1.dir == TURNRIGHT) {
		//if (motor1.position >= right){
			//motor1.step = INCREMENT;  	      
			//motor1.dir = TURNLEFT;
			//resolution+=10;
			//right=motor1.position+resolution;
			//left=motor1.position-resolution;
		//}else {
			//motor1.step = INCREMENT;  
		//}
	//}else {
		//if (motor1.position <= left){
			//motor1.step = INCREMENT;  	      
			//motor1.dir = TURNRIGHT;
			//resolution+=10;
			//right=motor1.position+resolution;
			//left=motor1.position-resolution;
		//}else {
			//motor1.step = INCREMENT;  
		//}
	//}
	motor1.steps = 500;
}

/*
void run(ModeTourr mode)
{
	switch (mode)
	{
		case Right:
			break;
			
		case Left:
			break;
		
		case Recherche:
			recherche();
			break;
			
		case None:
			break;
	}
}
*/

int testPing(int pin){
	int tmp = microsecondsToCentimeters(getDistance(pin));
	if(tmp <= MAXDIST)
		return tmp;
	else return 0;
}

void loopTourelle() {

  //Serial.println("ici");
  /*
  if(Serial.available()>0){Serial.println("ici");
	int val=Serial.read();
	if(val=='q'){
	Serial.println("gauche");
		motor1.step+=50;
		motor1.dir=TURNLEFT;
	}else if (val=='d'){	
	Serial.println("droite"); 
		motor1.step+=50;
		motor1.dir=TURNRIGHT;
	}else if(val=='s'){
		Serial.println(microsecondsToCentimeters(getDistance(PIN_PING_GAUCHE)),DEC);
		delay(200);
		Serial.println(microsecondsToCentimeters(getDistance(PIN_PING_DROITE)),DEC);
		Serial.println(motor1.position,DEC);
	}else if(val=='e'){
		Serial.println(microsecondsToCentimeters(getDistance(PIN_PING_DROITE)),DEC);
	}else if(val=='a'){
		Serial.println(microsecondsToCentimeters(getDistance(PIN_PING_GAUCHE)),DEC);
	}else if(val=='z'){
	*/

	int distance = 0;
	static long long oldMillis = 0;
	static ModeTourr pingTurn = Right;
	
	if ((millis() - oldMillis) > TEMPO)
	{
		oldMillis = millis();
		if (pingTurn == Right) {
			delay(40);
			gauche = testPing(PIN_PING_GAUCHE);
			pingTurn = Left;
		} else {
			delay(40);
			droite = testPing(PIN_PING_DROITE);
			pingTurn = Right;
		}
		
		bool objGauche = (gauche > 0);
		bool objDroite = (droite > 0);

		/*
		 * Un objet est détecté de chaque côté mais à des distances 
		 * différentes.
		 */
		if (objDroite && objGauche 
		    && (abs(droite - gauche) > DIFFMAX))
		{
			/*Serial.print("deux objets différents repérés\n");
			Serial.println(droite,DEC);
			Serial.println(gauche,DEC);*/
			
			if (droite < gauche)
      {
			  motor1.dir = TURNRIGHT;
			  motor1.steps = 100;
      }
			else
			{
			  motor1.dir = TURNLEFT;
			  motor1.steps = 100;
      }
		}
		
		/*
		 * Un objet est détecté de chaque côté à des distances
		 * équivalentes.
		 */
		else if (objDroite && objGauche)
		{
			distance = (gauche + droite) / 2;
			if (cibleValide(motor1.position, distance))
			{
				/*Serial.print("deux : ");
				Serial.println(distance,DEC);*/
			  motor1.steps = 0;
			}
		}
		
		/*
		 *  Un objet est détecté à gauche.
		 */
		else if (objGauche) 
		{
			//Serial.print("left : ");
		  motor1.dir = TURNLEFT;
		  motor1.steps = 100;
			//Serial.println(distance,DEC);
		}
		
		/*
		 * Un objet est détecté à droite
		 */
		else if (objDroite)
		{
			//Serial.print("right : ");
		  motor1.dir = TURNRIGHT;
		  motor1.steps = 100;
			//Serial.println(distance,DEC);
		}
		
		/*
		 * Aucun objet détecté
		 */
		else
      recherche();
	}
	motor1.run();
 }




/*#include "tourelle.h"

void initTourelle()
{
  // initialisation du servomoteur sur la patte numérique 9
  serv.attach(SERVO_PIN);
  // position de départ
  serv.write(position); 
}

void turn(int direction)
{
  if (direction == RIGHT) 
    {
      if (position >= RIGHT)
	{
	  position -= INCREMENT;    
	  sens = LEFT;
	} 
      else {
	position += INCREMENT;
      }
    } 
  else {
    if (position <= LEFT)
      {
	position += INCREMENT;
	sens = RIGHT;
      } 
    else {
      position -= INCREMENT;
    }
  }
	
  serv.write(position); 
}

bool sensorTest(int valeur){
  if(valeur>=150 && valeur <=450) return true;
  return false;
}

void recherche(){
  bool find=false;
  int resolution=20;
  int right=position+resolution;
  int left=position-resolution;

  while(find==false){
    if((millis()-oldMillis)>TEMPS_LATENCE){
      oldMillis=millis();
      if (sens == RIGHT) 
	{
	  if (position >= right)
	    {
	      position -= INCREMENT;    
	      sens = LEFT;
	      resolution+=10;
	      right=position+resolution;
              left=position-resolution;
	      if(left<=0)left=0;
	    } 
	  else {
	    position += INCREMENT;
	  }
	} 
      else {
	if (position <= left)
	  {
	    position += INCREMENT;
	    sens = RIGHT;
	    resolution+=10;
	    right=position+resolution;
            left=position-resolution;
	    if(left<=0)left=0;
	  } 
	else {
	  position -= INCREMENT;
	}
      }

      // envoyer la nouvelle consigne au servo
      serv.write(position);
      // lire les capteurs
      int sharpLeft = analogRead(LEFT_SENSOR_PIN);
      int sharpRight = analogRead(RIGHT_SENSOR_PIN);
			
      if(sensorTest(sharpLeft)==true || sensorTest(sharpRight)==true){
		find=true;
      }
    }
  }
}

void tourelleRun()
{
  oldMillis=millis();
  if((millis()-oldMillis)>TEMPS_LATENCE){
    oldMillis=millis();
    //boucle
    // lire les capteurs
    int sensorLeft = getDistance(LEFT_SENSOR_PIN);
    int sensorRight = getDistance(RIGHT_SENSOR_PIN);
    if(sensorTest(sensorLeft)==true && sensorTest(sensorRight)==true){
		if(sensorLeft>sensorRight)
			distance=sensorRight;
		else
			distance=sensorLeft;
    }else if(sensorTest(sensorLeft)==true){
      turn(LEFT);
      distance=sensorLeft;
    }else if(sensorTest(sensorRight)==true){
      turn(RIGHT); // peut etre blocant attention
      distance=sensorRight;
    }else if (sensorTest(sensorLeft)==false && sensorTest(sensorRight)==false){
      recherche(); //blocant a modifier
    }
  }
}
*/

