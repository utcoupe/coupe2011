#include "ALTypes.h"
#include "math.h"
#include "config.h"
#include "asservOrientation.h"
#include "asservDistance.h"
#include "trajectory.h"

#define ROTATION_HACK -40000

float gRotationTheorique=0;


uInt16 insidePoint(point pA)
{
  if ((pA.x < DIMENSION_X*COUNT_MILLIMETER) && (pA.y < DIMENSION_Y*COUNT_MILLIMETER))
  {
    if ((pA.x >= 0) && (pA.y >= 0))
    {
      return 1;
    }  
  }  
  return 0;
}  

uInt16 orientationHack(void)
{
  robot * Robot;
 // RECUPERATION DES PARAMETRE DU ROBOT
  Robot = getRobot();

  if ((Robot->nextTarget.x == ROTATION_HACK) && (Robot->nextTarget.y == ROTATION_HACK))
  {
    return 1;
  }  
  else
  {
    return 0;
  }  
}  

void    nextTargetPointManagement(float pEcart_d, uInt16 pPlanificationAsked)
{
  float rotationDeBase;
  robot * Robot;
  float ecart_d;
  static uInt16 pointDePassageForce = 0;
 // RECUPERATION DES PARAMETRE DU ROBOT
  Robot = getRobot();
  ecart_d =calculConsigne_d(Robot->target);
  // DETECTION D UN ORDRE DE RECALAGE BORDURE
  if (orientationHack())
  {
    pointDePassageForce = 0;
    return;
  }  
  if ((!insidePoint(Robot->target)) && (!insidePoint(Robot->nextTarget)))
  {
    if ( Robot->rotationMode != NO_PWM )
    {
      if (Robot->rotationMode != ROTATION_DISABLE)
      {
        Robot->rotationMode = ROTATION_DISABLE;
      }
    }  
    return;
  }   
	// DETECTION DE L ARRIVE A DESTINATION   ou d'une replanification sauvage de trajectoire
	else if  ((pPlanificationAsked) || (Robot->target.x != Robot->nextTarget.x) || (Robot->target.y != Robot->nextTarget.y) )
	{
  	if (( fabs(ecart_d) < Robot->entraxe/2 ) || pPlanificationAsked )
    {
      if(pPlanificationAsked )
      {
        pointDePassageForce = 0;
      }  
      if ((fabs(ecart_d) > toCount(20)) && pointDePassageForce)
    	{
      	return; 
      }	
      else
      {
        pointDePassageForce = 0;
      }

      if (!pPlanificationAsked)
      {
  	    rotationDeBase = calculEcart_rot(Robot->nextTarget);
  	    ecart_d =calculConsigne_d(Robot->nextTarget);
  	  }
  	  else
  	  {
    	  rotationDeBase = calculEcart_rot(Robot->target); 
    	}
  	  
  	  if ((fabsf(rotationDeBase) > PI/2) || (fabs(ecart_d) < Robot->entraxe) ||  (!insidePoint(Robot->nextTarget)))
  	  {
    	  // After a rotation in place speed is null
    	  if (Robot->speedConsigne == 0)
    	  {
          if (!pPlanificationAsked)
          {
            Robot->target = Robot->nextTarget;
          }
          else if( fabs(ecart_d) < Robot->entraxe )
    	    {
      	      pointDePassageForce = 1;
    	    }
          Robot->rotationMode = ROTATION_IN_PLACE;
          Robot->speedConsigne = 0;
          Robot->speedConsigneRot = 0;
          gRotationTheorique = rotationDeBase;
          // check that the new orientation point have really be loaded and that it is not just a planification
          if(!insidePoint(Robot->nextTarget) &&  (Robot->target.x == Robot->nextTarget.x) && (Robot->target.y == Robot->nextTarget.y) )
          {
            // sacage nextTarget so target and nexttarget are different so dont enter in relacage mode
            point p;
            p.x = ROTATION_HACK;
            p.y = ROTATION_HACK;
            Robot->nextTarget = p;
          } 
    	  } 
    	  else if (pPlanificationAsked)
    	  {
      	  Robot->rotationMode = ROTATION_IN_PLACE;
          gRotationTheorique = rotationDeBase; 
          Robot->speedConsigneRot = 0;
          Robot->speedConsigne = 0;
      	} 
  	  }
  	  else
  	  {
        Robot->rotationMode = ROTATION_PIVOT;
        if (rotationDeBase > 0)
        {
          Robot->speedConsigneRot = (fabs(Robot->speedConsigne) * PI)/(Robot->entraxe);
        }
        else
        {
          Robot->speedConsigneRot = -(fabs(Robot->speedConsigne) * PI)/(Robot->entraxe);
        } 
        gRotationTheorique = rotationDeBase; 
        if (!pPlanificationAsked)
    	  {
    	    Robot->target = Robot->nextTarget;
    	  }
    	  if( fabs(ecart_d) < Robot->entraxe/2 )
    	  {
      	    pointDePassageForce = 1;
    	  } 
    	}
    }
  }  
}


