#include "ALTypes.h"
#include "math.h"
#include "config.h"
#include "asservDistance.h"
#include "asservOrientation.h"
#include "speedAsserv.h"
#include "PWM.h"

extern float gRotationTheorique;
extern float distanceTheorique;
extern uInt8 noCommandTimeOutFlag;

void detectionCalageBordure (void)
{
  robot * Robot;
  JointConfig* jointGauche;
  JointConfig* jointDroite;
  // Get config 
  jointGauche = oGetJointConfig(GAUCHE);
  jointDroite = oGetJointConfig(DROITE);

 // RECUPERATION DES PARAMETRE DU ROBOT
  Robot = getRobot();
  // detection du callage bordure
  if ( (Robot->rotationMode == ROTATION_DISABLE) &&  (fabs(Robot->speedConsigne) > 5) &&  (fabs(jointDroite->speed) < 2) && (fabs(jointGauche->speed) < 2)  )
  {    
    distanceTheorique = 0;
    Robot->speedConsigne = 0;
    Robot->rotationMode = NO_PWM;
  }
}


float ecretagef (float Vmax, float val)
{
   if (val>=Vmax) return Vmax;
   if (val<=-Vmax) return -Vmax;
   else return val;
}

float balanced_clip(float pMax,float pValue, float * pBalance)
{
	if (pValue > 0)
	{
		if ( pValue > pMax)
		{
			*pBalance  = pValue  - (pMax);
			pValue = pMax;
		}
		else
		{
			*pBalance=0;
		}
	}
	else if(pValue < 0)
	{
		if ( pValue < -pMax)
		{
			*pBalance   = pValue - (-pMax);
			pValue = -pMax;
		}
		else
		{
			*pBalance = 0;
		}
	}
	else
	{
		*pBalance = 0;
	}
	return pValue;
}


void asservissementPosition(float pEcart_d,float pEcart_rot)
{
  float commande_droite,commande_gauche,command,command_rot;
  float balanceDroite=0,balanceGauche=0;
  float ecart_d,ecart_rot;
  static float last_erreur_rot=0,last_erreur_d=0;
  asserv * pid;
  robot * Robot;
 
  // RECUPERATION DES PARAMETRE DU ROBOT
  Robot = getRobot();
  
  // RECUPERATION DES PARAMETTRES DE L ASSERVISSEMENT
  pid = getPID();
  /*
  // GESTION DU MAINTIEN EN POSITION FIXE
  if ( fabs(pEcart_d) < toCount(250))
  {
  if ( fabsf(pEcart_rot) > PI/2)
    {
    pEcart_d *= -1;
    pEcart_rot+= PI;
    while (pEcart_rot >  PI) pEcart_rot-=PIx2;
    while (pEcart_rot < -PI) pEcart_rot+=PIx2;
     last_erreur_rot *=-1;
      last_erreur_d *= -1; 
    }  
    }
*/

  //ecart_rot = calculEcart_rot(Robot->target);
  // ou si speedconsigne == 0 ou pas
  //	if ((fabs(ecart_d) < toCount(20)) && ((Robot->target.x == Robot->nextTarget.x) && (Robot->target.y == Robot->nextTarget.y)))
  if ((gRotationTheorique == 0) &&  (distanceTheorique == 0))
    {
    pEcart_d /= 5;
    // voir s'il ne vaut mieu pas bosser avec ecartrot qui n'a pas été trafiquer
    if ( fabsf(pEcart_rot) > PI/2)
      {      
	pEcart_d *= -1;
	pEcart_rot += PI;
	while (pEcart_rot >  PI) pEcart_rot-=PIx2;
	while (pEcart_rot < -PI) pEcart_rot+=PIx2;
      }    
    }
  // CALCUL PID
  command = ((pEcart_d - last_erreur_d ) * pid->Kd_d + pEcart_d ) * pid->Kp_d;
  command = ecretagef( PWM_MAX, command);
  
  command_rot = ((pEcart_rot - last_erreur_rot ) * pid->Kd_o + pEcart_rot ) * pid->Kp_o;
  command_rot = ecretagef( PWM_MAX, command_rot);
  if (Robot->rotationMode == NO_PWM)
    {
      last_erreur_rot = 0;
      last_erreur_d = 0;
      commande_droite = 0;
      commande_gauche = 0; 
    }  
  else if (Robot->rotationMode == ROTATION_DISABLE)
    {
      // RECALAGE BORDURE SANS ASSERVISSEMENT D ORIENTATION
      command_rot = 0;
      commande_droite = balanced_clip(PWM_MAX,command ,&balanceGauche);
      commande_gauche = balanced_clip(PWM_MAX,command ,&balanceDroite);    
    }  
  else if (gRotationTheorique == 0)
    {
      // SANS ASSERVISSEMENT DE VITESSE DE ROTATION
      commande_droite = balanced_clip(PWM_MAX,command + command_rot,&balanceGauche);
      commande_gauche = balanced_clip(PWM_MAX,command - command_rot,&balanceDroite);
    }
  else if (Robot->rotationMode == ROTATION_IN_PLACE)
    {
      // AVEC ASSERVISSEMENT DE VITESSE DE ROTATION SUR PLACE
      commande_droite = balanced_clip(PWM_MAX,   command_rot,&balanceGauche);
      commande_gauche = balanced_clip(PWM_MAX, - command_rot,&balanceDroite);
    }
  else if (Robot->rotationMode == ROTATION_PIVOT)
    {
      // AVEC ASSERVISSEMENT DE VITESSE DE ROTATION EN PIVOT AUTOUR D UNE ROUE
      if ( Robot->speedMax*command_rot > 0)
	{
	  commande_droite = balanced_clip(PWM_MAX,command_rot,&balanceGauche);
	  commande_gauche = balanced_clip(PWM_MAX,0,&balanceDroite);
	}
      else
	{
	  commande_droite = balanced_clip(PWM_MAX,0,&balanceGauche);
	  commande_gauche = balanced_clip(PWM_MAX,-command_rot,&balanceDroite);
	}  
    }  
  
  
  //commande_droite -= balanceDroite;
  //commande_gauche -= balanceGauche;
  
  commande_droite = ecretagef( PWM_MAX, commande_droite - balanceDroite);
  commande_gauche = ecretagef( PWM_MAX, commande_gauche - balanceGauche);
  
  last_erreur_rot = pEcart_rot;
  last_erreur_d   = pEcart_d;
  if (noCommandTimeOutFlag)
    {
      oApplyPWM( 0, DROITE);
      oApplyPWM( 0, GAUCHE);
    } 	
  else
    {
      oApplyPWM( commande_droite, DROITE);
      oApplyPWM( -commande_gauche, GAUCHE);
    }
}
