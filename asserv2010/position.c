#include "ALTypes.h"
#include "math.h"
#include "quadratureEncoder.h"
#include "config.h"
#include "position.h"

void calcul_position(void)
{
  robot * Robot;  
  robot * RobotLast; 
  JointConfig* jointGauche;
  JointConfig* jointDroite;
  // Get config 
  jointGauche = oGetJointConfig(GAUCHE);
  jointDroite = oGetJointConfig(DROITE);
  Robot = getRobot();
  RobotLast = getRobotLast();  
	jointDroite->position = getSoftwarePosQEI2();
	jointGauche->position = getSoftwarePosQEI();
	
	jointDroite->speed = 	jointDroite->position  - jointDroite->lastPosition;
	jointGauche->speed =  jointGauche->position  - jointGauche->lastPosition;
	
//      vitesse_rot=(float)(read32(DROITE,RVCourante)+read32(GAUCHE,RVCourante));

	//Calcul de la vitesse ponctuelle
	Robot->speed = ( 	jointDroite->speed + 	jointGauche->speed ) /2.0;
	
  //Robot->acceleration = Robot->speed - RobotLast->speed;

	//calcul de l'orientation
	Robot->orientation = Robot->orientation_initiale + (float)( jointGauche->position - jointDroite->position)/Robot->entraxe;
    // modulo PI
	while (Robot->orientation >  PI ) Robot->orientation -= PIx2;
	while (Robot->orientation < -PI ) Robot->orientation += PIx2;

 //calcul du deplacement
 Robot->position.x += Robot->speed*cosf(Robot->orientation);
 Robot->position.y += -Robot->speed*sinf(Robot->orientation);
 
 updateJoints();
 updateLastRobot();
 
}
