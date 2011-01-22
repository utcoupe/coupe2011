#include "ALTypes.h"
#include "math.h"
#include "config.h"
#include "trajectory.h"
#include "asservOrientation.h"
#include "asservDistance.h"

extern float gRotationTheorique;
float distanceTheorique=0;


float calculAccelerationRot (float pEcart,float  pDistanceTheorique)
{
  float localSpeed=0,localDistance=0,t,localDecceleration;
  robot * Robot;

  // useless to do more things
  if ( pDistanceTheorique == 0)
  {
    return pEcart;
  }   
  
  Robot = getRobot();
  localSpeed = Robot->speedConsigneRot;
 // localSpeed = Robot->speed;
  if(pDistanceTheorique > 0)
  {
    localDecceleration = -Robot->accelerationRot;
  }
  else
  {
    localDecceleration = Robot->accelerationRot;
  }

  t = -localSpeed/localDecceleration;
  localDistance = localDecceleration * t*t/2 + localSpeed*t;
  
  if (localDistance > 0 )
  {
    // si on doit ralentir la rotation
    if (localDistance >= pDistanceTheorique) 
    {
     if ( Robot->rotationMode == ROTATION_IN_PLACE ) 
     {
       //phase de freinage
       Robot->speedConsigneRot += localDecceleration;
       if (Robot->speedConsigneRot < 0.0004)
       {
         Robot->speedConsigneRot = 0;
       }
       return pEcart-(pDistanceTheorique - Robot->speedConsigneRot);
     }  
    }
  }
  if (localDistance < 0 )
  {
    if (localDistance <= pDistanceTheorique ) 
    {
     //phase de freinage
     if ( Robot->rotationMode == ROTATION_IN_PLACE ) 
     {
       Robot->speedConsigneRot += localDecceleration;
       if (Robot->speedConsigneRot > -0.0004)
       {
         Robot->speedConsigneRot = 0;
       }
       return pEcart-(pDistanceTheorique - Robot->speedConsigneRot);
      }
    }
  }  
// phase d'acceleration ou de vitesse constante
  if (( Robot->speedConsigneRot < Robot->speedRotMax) && (pDistanceTheorique > 0))
  {
    // phase d'acceleration
    Robot->speedConsigneRot += Robot->accelerationRot;
    // prevent overspeed
    if (Robot->speedConsigneRot > Robot->speedRotMax)
    {
      Robot->speedConsigneRot = Robot->speedRotMax;
    }  
  }
  // for acceleration in reverse way
  else if (( Robot->speedConsigne > -Robot->speedRotMax) && (pDistanceTheorique < 0))
  {
    // phase d'acceleration
    Robot->speedConsigneRot -= Robot->accelerationRot;
    // prevent overspeed
    if ( Robot->speedConsigneRot < -Robot->speedRotMax)
    {
      Robot->speedConsigneRot = -Robot->speedRotMax;
    }  
  } 
  return pEcart-(pDistanceTheorique - Robot->speedConsigneRot);
}


float calculAcceleration (float pEcart,float pDistanceTheorique)
{
  float localSpeed=0,localDistance=0,t,localDecceleration;
  float rotationDeBase;
  robot * Robot;

  // useless to do more things
  if ( pDistanceTheorique == 0)
  {
    return pEcart;
  }  
   
  Robot = getRobot();
  rotationDeBase = calculEcart_rot(Robot->nextTarget);
  localSpeed = Robot->speedConsigne;
 // localSpeed = Robot->speed;
  if(Robot->speedMax > 0)
  {
    localDecceleration = Robot->decceleration;
  }
  else if(Robot->speedMax < 0)
  {
    localDecceleration = -Robot->decceleration;
  }
  else  // speedMax= 0 Brake 
  {
    // a bit incorrect to check deceleration sens with the curent consigne
    if (localSpeed > 0)
    {
     localDecceleration = -10.0;
    }
    else
    {
     localDecceleration = 10.0;
    } 
  }  
  
  // Calcul de la vitesse et du temps nécessaires à l'arrêt
  t = -localSpeed/localDecceleration;
  localDistance = localDecceleration * t*t/2 + localSpeed*t;
  
  if (localDistance > 0 )
  {
    // si le dernier point est atteint
    if (localDistance >= pDistanceTheorique) 
    {
     if (((Robot->target.x == Robot->nextTarget.x) && (Robot->target.y == Robot->nextTarget.y)) || (fabsf(rotationDeBase) > PI/2) ) 
     {
       //phase de freinage
       Robot->speedConsigne += Robot->decceleration;
       if (Robot->speedConsigne < 1)
       {
         Robot->speedConsigne = 0;
       }
       return pEcart-(pDistanceTheorique - Robot->speedConsigne);
     }  
    }
  }
  if (localDistance < 0 )
  {
    if (localDistance <= pDistanceTheorique ) 
    {
     //phase de freinage
     if (((Robot->target.x == Robot->nextTarget.x) && (Robot->target.y == Robot->nextTarget.y)) || (fabsf(rotationDeBase) > PI/2) )
     {
       Robot->speedConsigne -= Robot->decceleration;
       if (Robot->speedConsigne > -1)
       {
         Robot->speedConsigne = 0;
       }
       return pEcart-(pDistanceTheorique - Robot->speedConsigne);
      }
    }
  }  
// phase d'acceleration ou de vitesse constante
  if ( Robot->speedConsigne < Robot->speedMax)
  {
    // phase d'acceleration
    Robot->speedConsigne+=Robot->acceleration;
    // prevent overspeed
    if (Robot->speedConsigne > Robot->speedMax)
    {
      Robot->speedConsigne = Robot->speedMax;
    }  
  }
  // for acceleration in reverse way
  else if ( Robot->speedConsigne > Robot->speedMax)
  {
    // phase d'acceleration
    Robot->speedConsigne-=Robot->acceleration;
    // prevent overspeed
    if ( Robot->speedConsigne < Robot->speedMax)
    {
      Robot->speedConsigne = Robot->speedMax;
    }  
  } 
  return pEcart-(pDistanceTheorique - Robot->speedConsigne);
  
}


float decreaseDistanceTheorique (float pDistanceTheorique)
{
  robot * Robot;
  Robot = getRobot();
  
  
  if (pDistanceTheorique > 0)
  {
    pDistanceTheorique -= Robot->speedConsigne;
    if ( pDistanceTheorique < 1 ) 
    {
      pDistanceTheorique = 0;
      Robot->speedConsigne = 0;
    }   
  }
  else  if (pDistanceTheorique < 0)
  {
    pDistanceTheorique -= Robot->speedConsigne;
    if ( pDistanceTheorique > -1 ) 
    {
      pDistanceTheorique = 0;
      Robot->speedConsigne = 0; 
    } 
  }
   
  return pDistanceTheorique; 
}

float decreaseRotationTheorique (float pRotationTheorique)
{
  robot * Robot;
  Robot = getRobot();
  

  if (pRotationTheorique > 0)
  {
    pRotationTheorique -= Robot->speedConsigneRot;
    if ( pRotationTheorique < 0.002 ) 
    {
      pRotationTheorique = 0;
    }
  }
  else if (pRotationTheorique < 0)
  {
    pRotationTheorique -= Robot->speedConsigneRot;
    if ( pRotationTheorique > -0.002 ) 
    {  
      pRotationTheorique = 0;
    } 
  }
   
  return pRotationTheorique; 
}

void asservissementVitesse(float * ecart_d,float * ecart_rot)
{
  robot * Robot;
  
  static uInt16 lastTimeWasRotation = 0;
  // RECUPERATION DES PARAMETRES DU ROBOT
  Robot = getRobot();
  
  // CALCUL DE LA CONSIGNE DE VITESSE
  //erreur_d = ecart_d - (Robot->timeGiven * Robot->speedConsigne);
  //*ecart_rot -= (Robot->timeGivenRot * Robot->speedConsigneRot);
  *ecart_rot = calculAccelerationRot(*ecart_rot,gRotationTheorique);
  gRotationTheorique = decreaseRotationTheorique(gRotationTheorique);
  // verification du sens de deplacement
  if (*ecart_d * Robot->speedMax > 0)
    {
      //verification du temps imparti pour realiser l'action
      if (gRotationTheorique != 0)
  	{
	  *ecart_d = 0;
	  distanceTheorique = 0;
	  lastTimeWasRotation = 1; 
	}
      else
	{
	  static uInt16 bascule = 0;
	  // if rotation is over, compute the new distance to run
	  if (Robot->rotationMode == ROTATION_DISABLE )
	    {
	      if (bascule == 0)
		{
		  distanceTheorique = calculConsigne_d(Robot->target);
		  bascule = 1;
		}  
	    }
	  else
	    {
	      bascule = 0;
	    }
	  
		if (lastTimeWasRotation)
	    {
	      distanceTheorique = calculConsigne_d(Robot->target);
	      if(Robot->speedMax > 0)
		{ 
		  Robot->speedConsigne = fabs(Robot->speedConsigneRot) * Robot->entraxe / PI;
		}
	      else if (Robot->speedMax < 0)
		{
		  Robot->speedConsigne = -fabs(Robot->speedConsigneRot) * Robot->entraxe / PI;
		}
	      if (Robot->rotationMode == ROTATION_DISABLE )
		{
		  Robot->speedConsigne = 0;
		}  
	      lastTimeWasRotation = 0;
	    }
	  Robot->speedConsigneRot = 0;
	  gRotationTheorique = 0;
	  if (orientationHack())
	    {
	      distanceTheorique = 0;
	      *ecart_d = 0;
	    }   
	  else
	    {
	      *ecart_d = calculAcceleration(*ecart_d,distanceTheorique);
	      distanceTheorique = decreaseDistanceTheorique(distanceTheorique);
	    }  
	}  
    }
}
