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

bool testSharp(int valeur){
  if(valeur>=150 && valeur <=450) return true;
  return false;
}

void recherche(){
  bool find=false;
  int resolution=10;
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
      int sharpLeft = analogRead(ANALOG_SHARP_L);
      int sharpRight = analogRead(ANALOG_SHARP_R);
			
      if(testSharp(sharpLeft)==true || testSharp(sharpRight)==true){
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
    int sharpLeft = analogRead(ANALOG_SHARP_L);
    int sharpRight = analogRead(ANALOG_SHARP_R);
    if(testSharp(sharpLeft)==true && testSharp(sharpRight)==true){
      distance=(sharpLeft+sharpRight)/2;
    }else if(testSharp(sharpLeft)==true){
      turn(LEFT);
      distance=sharpLeft;
    }else if(testSharp(sharpRight)==true){
      turn(RIGHT); // peut etre blocant attention
      distance=sharpRight;
    }else if (testSharp(sharpLeft)==false && testSharp(sharpRight)==false){
      recherche(); //blocant a modifier
    }
  }
}


