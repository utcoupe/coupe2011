#include "tourelle.h"



Stepper motor1(2);

int x,y,angleR;

int resolution=10;
int right=motor1.position+resolution;
int left=motor1.position-resolution;
int gauche = 0,droite = 0;

void setupTourelle() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("Stepper");
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
	x=xUpdate;
	y=yUpdate;
	angleR=angleRUpdate;
}

bool cibleValide(int angleTTest, int distance)
{
	bool valide=true;
	int angle=angleR+((18*angleTTest/50)%1000); //modulo 1000 pour le cas où la tourelle fait un tour, 18 et 50 le rapport de réduction des angre
	int A=distance*cos(angle)+x, B=distance*sin(angle)+y;

	if(A>(3000-MARGE_BORD) || A<MARGE_BORD)
		valide=false;

	if(B>(2100-MARGE_BORD) || B<MARGE_BORD)
		valide=false;
	
	return valide;
}

	

void recherche(){
	if (motor1.dir == TURNRIGHT) {
		if (motor1.position >= right){
			motor1.step = INCREMENT;  	      
			motor1.dir = TURNLEFT;
			resolution+=10;
			right=motor1.position+resolution;
			left=motor1.position-resolution;
		}else {
			motor1.step = INCREMENT;  
		}
	}else {
		if (motor1.position <= left){
			motor1.step = INCREMENT;  	      
			motor1.dir = TURNRIGHT;
			resolution+=10;
			right=motor1.position+resolution;
			left=motor1.position-resolution;
		}else {
			motor1.step = INCREMENT;  
		}
	}
}

void run(int mode){
	motor1.run();
	if(mode == RIGHT){
		motor1.dir = TURNRIGHT;
		motor1.step=10;
	}else if (mode == LEFT){
		motor1.dir = TURNLEFT;
		motor1.step=10;
	}else if (mode == RECHERCHE){
		recherche();
	}else if(mode ==NONE){
		motor1.step=0;
	}else{
		motor1.step=10;
	}
	motor1.run();
}

int testPing(int pin){
	int tmp = microsecondsToCentimeters(getDistance(pin));
	if(tmp <= MAXDIST)
		return tmp;
	else return 0;
}

void loopTourelle() {
  motor1.run();
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
		
		
		int turn = NONE;
		int distance = 0;
		static long long oldMillis=0;
		static int pingTurn = RIGHT;
			//boucle principale
			//run(turn);
			if((millis()-oldMillis)>TEMPO){
				oldMillis=millis();
				if(pingTurn == RIGHT){
					motor1.step=0;delay(40);
					gauche = testPing(PIN_PING_GAUCHE);
					pingTurn = LEFT;
				}else{
					motor1.step=0;delay(40);
					droite = testPing(PIN_PING_DROITE);
					pingTurn = RIGHT;
				}
				
				

				if (droite>0 && gauche>0 &&((droite-gauche)>DIFFMAX || (droite-gauche)<-DIFFMAX))
				{
					/*Serial.print("deux objets différents repérés\n");
					Serial.println(droite,DEC);
					Serial.println(gauche,DEC);*/
					
					if(droite<gauche)
						turn=RIGHT;
						
					else
						turn=LEFT;
					
				}
				else if(droite > 0 && gauche > 0){
					distance=(gauche+droite)/2;
					if(cibleValide(motor1.position, distance))
					{
						/*Serial.print("deux : ");
						Serial.println(distance,DEC);*/
						turn=NONE;
					}
					else// si la tourelle a détecté un élement exterieur à la table
					{
						motor1.step+=150;
						if(motor1.dir==TURNLEFT)
							motor1.dir=TURNRIGHT;
						else
							motor1.dir=TURNLEFT;
					}
				}else if(gauche > 0){
					//Serial.print("left : ");
					turn=LEFT;
					distance=gauche;
					//Serial.println(distance,DEC);
				}else if(droite > 0){
					//Serial.print("right : ");
					turn=RIGHT;
					distance=droite;
					//Serial.println(distance,DEC);
				}else{
					turn=RECHERCHE;
				}
			}
			run(turn);
	//}
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

