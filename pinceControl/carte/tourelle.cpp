#include "tourelle.h"

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


