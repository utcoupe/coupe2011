#include "ALTypes.h"
#include "math.h"
#include "trajectory.h"
#include "config.h"
#include "math.h"

#define DECELERATION 1
#define ACCELERATION 1

float distance(point * pA, point * pB)
{	
  float temp,temp2;
  	// pythagore décomposé
	temp = fabsf(pA->x - pB->x);
	temp *= temp;
	temp2 = fabsf(pA->y - pB->y);
	temp2 *= temp2;
	
	return sqrt(temp + temp2);
}

float calculConsigne_d(point t)
{
  float ecart_d;
  robot * Robot;
 
  Robot = getRobot();

  ecart_d = distance(&Robot->position, &t);
  
  // for reverse way
  if (Robot->speedMax < 0)
  {
    ecart_d*=-1;
  }  

  return ecart_d;
    
}


