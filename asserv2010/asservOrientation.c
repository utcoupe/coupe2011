#include "ALTypes.h"
#include "math.h"
#include "trajectory.h"
#include "config.h"
#include "math.h"


float calculEcart_rot(point pA)
{
	float deltaX,deltaY,ecart_rot,consigne_rot;
  robot * Robot;
 
  Robot = getRobot();
  
	deltaX = (pA.x-Robot->position.x);
	deltaY = (pA.y-Robot->position.y);

	if((deltaX==0)&&(deltaY==0)) return 0;

	consigne_rot= - atan2f(deltaY ,deltaX);	
  
  if (Robot->speedMax < 0 )
  {
    	ecart_rot=consigne_rot - (Robot->orientation + PI);
  }  
  else
  {
    	ecart_rot=consigne_rot - Robot->orientation;
  }  
	//modulo de l'orientation
	while (ecart_rot >  PI) ecart_rot-=PIx2;
	while (ecart_rot < -PI) ecart_rot+=PIx2;
 
  return ecart_rot;
}
