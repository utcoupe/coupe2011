/*
 * control.cpp
 *
 *  Created on: 13 janv. 2011
 *      Author: HoHen
 */


/* Quelques trucs sur le PID
 *
 * kp : le terme proportionnel qui permet d'augmenter la vitesse de montee (atteint la consigne le plus rapidement possible).
 * ki : le terme integral qui reduit l'erreur statique.
 * kd : le terme derive qui reduit le depassement (l'overshoot).
 * __________________________________________________________________________________________________
 * Coefficient		|Temps de montee	|Temps de stabilisation	|Depassement	|Erreur statique	|
 *	kp				|Diminue			|Augmente				|Augmente		|Diminue			|
 *	ki				|Diminue			|Augmente				|Augmente		|Annule				|
 *	kd				|-					|Diminue				|Diminue		|-					|
 * _________________|___________________|_______________________|_______________|___________________|
 */

#include "WProgram.h"
#include "control.h"
#include "encoder.h"
#include "robotstate.h"
#include "PID_Beta6.h"

bool reinitPID;
CurrentGoal current_goal;

void initController(){
	current_goal.isReached = true;
	reinitPID = true;
}


double pwm,consigne,currentSpeed;
PID pid4SpeedControl(&currentSpeed,&pwm,&consigne,KP_SPEED,KI_SPEED,KD_SPEED);

/* Calcule les pwm a appliquer pour un asservissement en vitesse en trapeze
 * <> value_pwm_left : la pwm a appliquer sur la roue gauche [-255,255]
 * <> value_pwm_right : la pwm a appliquer sur la roue droite [-255,255]
 * */
void speedControl(int* value_pwm_left, int* value_pwm_right){
	/* si le robot est en train de tourner, et qu'on lui donne une consigne de vitesse, il ne va pas partir droit
	 * solution = decomposer l'asservissement en vitesse en 2 :
	 * -> stopper le robot (les 2 vitesses = 0)
	 * -> lancer l'asservissement en vitesse
	 */

	static int start_time;

	static bool initDone = false;

	if(!initDone){
		start_time = 0;
		pwm = 0;
		currentSpeed = 0;
		consigne = 0;
		pid4SpeedControl.Reset();
		pid4SpeedControl.SetInputLimits(-10,10);
		pid4SpeedControl.SetOutputLimits(-255,255);
		pid4SpeedControl.SetSampleTime(2); //10ms, tout ce qu'il faut c'est que l'observateur soit plus rapide que le PID
		pid4SpeedControl.SetMode(AUTO);
		initDone = true;
	}

	if(current_goal.phase == PHASE_1){ //phase d'acceleration
		consigne = current_goal.speed;
		currentSpeed = robot_state.speed;
		if(abs(consigne-currentSpeed) < 1){ /*si l'erreur est infï¿½rieur a 1, on concidere la consigne atteinte*/
			current_goal.phase = PHASE_2;
			start_time = millis();
		}
	}
	else if(current_goal.phase == PHASE_2){ //phase de regime permanent
		consigne = current_goal.speed;
		currentSpeed = robot_state.speed;
		if(millis()-start_time > current_goal.periode){ /* fin de regime permanent */
			current_goal.phase = PHASE_3;
		}
	}
	else if(current_goal.phase == PHASE_3){ //phase de decceleration
		consigne = 0;
		currentSpeed = robot_state.speed;
		if(abs(robot_state.speed)<1){
			current_goal.phase = PHASE_4;
		}
	}

	pid4SpeedControl.Compute();

	if(current_goal.phase == PHASE_4){
		(*value_pwm_right) = 0;
		(*value_pwm_left) = 0;
		current_goal.isReached = true;
		initDone = false;
	}else{
		(*value_pwm_right) = pwm;
		(*value_pwm_left) = pwm;
	}

}

double currentAngle;
PID pid4AngleControl(&currentAngle,&pwm,&consigne,KP_ANGLE,KI_ANGLE,KD_ANGLE);
/* Calcule les pwm a appliquer pour un asservissement en angle
 * <> value_pwm_left : la pwm a appliquer sur la roue gauche [-255,255]
 * <> value_pwm_right : la pwm a appliquer sur la roue droite [-255,255]
 * */
void angleControl(int* value_pwm_left, int* value_pwm_right){


	static bool initDone = false;

	if(!initDone){
		pwm = 0;
		currentAngle = .0;
		consigne = .0;
		pid4AngleControl.Reset();
		pid4AngleControl.SetInputLimits(-M_PI,M_PI);
		pid4AngleControl.SetOutputLimits(-255,255);
		pid4AngleControl.SetSampleTime(2); //2ms, tout ce qu'il faut c'est que l'observateur soit plus rapide que le PID
		pid4AngleControl.SetMode(AUTO);
		initDone = true;
	}

	if(current_goal.phase == PHASE_1){
		//la consigne doit etre fixe, comprise entre [O, 2PI[
		//donc on ne doit modifier que l'expression de l'angle courant pour jouer sur l'Žcart
		//cas 1 : consigne = 90deg	, angle reel = 1deg => 89deg dans le sens direct
		//cas 2 : consigne = 1deg	, angle reel = 359deg => 2deg dans le sens direct
		//cas 3 : consigne = 359deg	, angle reel = 1deg => 2deg dans le sens indirect
		//cas 4 : consigne = 1deg	, angle reel = 90deg => 89deg dans le sens indirect
		// -> trouver la valeur de l'Žcart [-PI PI], le signe donnera le sens

		consigne = current_goal.angle;

		if(abs(current_goal.angle - robot_state.angle) >M_PI)
			//si la difference d'angle entre la consigne est superieur a PI
			if(current_goal.angle > robot_state.angle)
				currentAngle = robot_state.angle + 2*M_PI; //dans le cas ou l'angle consigne est de 359deg et l'angle courant 1deg, on met l'angle courant a 361

			else
				currentAngle = robot_state.angle - 2*M_PI; //dans le cas ou l'angle consigne est de 1deg et l'angle courant 359deg, on met l'angle courant a -1

		else
			//si la difference d'angle entre la consigne est inferieur a PI, la valeur de l'ecart est cette difference => l'angle courant ne change pas
			currentAngle = robot_state.angle ;



		if(abs(consigne-currentAngle) < M_PI/180){ /*si l'erreur est inferieur a 1deg, on concidere la consigne atteinte*/
			current_goal.phase = PHASE_2;
		}
	}


	pid4AngleControl.Compute();


	if(current_goal.phase == PHASE_2){
		(*value_pwm_right) = 0;
		(*value_pwm_left) = 0;
		current_goal.isReached = true;
		initDone = false;
	}
	else{
		(*value_pwm_right) = pwm;
		(*value_pwm_left) = -pwm;
	}


}

/* Asservissement en position
 * Se base sur un representation alpha-delta (polaire en quelquesorte) de l'ecart avec la position desiree
 * Alpha represente l'ecart de l'angle (rho en polaire)
 * Delta represente l'ecart de distance (r en polaire)
 * L'idee est donc de separer la pwm resultante en 2 composantes :
 * 	-la vitesse de rotation pour reduire l'ecart alpha
 * 	-la vitesse lineaire pour reduire l'ecart delta
 */
double output4Delta, output4Alpha;
double currentDelta, currentAlpha;
double consigneDelta, consigneAlpha;
PID pid4DeltaControl(&currentDelta,&output4Delta,&consigneDelta,KP_DELTA,KI_DELTA,KD_DELTA);
PID pid4AlphaControl(&currentAlpha,&output4Alpha,&consigneAlpha,KP_ALPHA,KI_ALPHA,KD_ALPHA);

/* Calcule les pwm a appliquer pour un asservissement en position
 * <> value_pwm_left : la pwm a appliquer sur la roue gauche [-255,255]
 * <> value_pwm_right : la pwm a appliquer sur la roue droite [-255,255]
 * */
void positionControl(int* value_pwm_left, int* value_pwm_right){

	static bool initDone = false;

	if(!initDone){
		output4Delta = 0;
		output4Alpha = 0;
		currentDelta = .0;
		currentAlpha = .0;
		consigneDelta = .0;
		consigneAlpha = .0;
		pid4DeltaControl.Reset();
		pid4DeltaControl.SetInputLimits(-TABLE_DISTANCE_MAX_MM/ENC_TICKS_TO_MM,TABLE_DISTANCE_MAX_MM/ENC_TICKS_TO_MM);
		pid4DeltaControl.SetSampleTime(2);
		pid4DeltaControl.SetOutputLimits(-current_goal.speed,current_goal.speed);
		pid4DeltaControl.SetMode(AUTO);
		pid4AlphaControl.Reset();
		pid4AlphaControl.SetSampleTime(2);
		pid4AlphaControl.SetInputLimits(-M_PI,M_PI);
		pid4AlphaControl.SetOutputLimits(-55,55);
		pid4AlphaControl.SetMode(AUTO);
		initDone = true;
	}


	if(current_goal.phase == PHASE_1){

		/*calcul de l'angle alpha combler avant d'etre aligne avec le point cible
		 * borne = [-PI/2 PI/2] */
		double alpha = atan2(abs(robot_state.y-current_goal.y),abs(robot_state.x-current_goal.x)); /*arctan(y/x) -> [-PI/2,PI/2]*/
		currentAlpha = M_PI - robot_state.angle - alpha;

	 	double dx = robot_state.x-current_goal.x;
		double dy = robot_state.y-current_goal.y;
		int sens = dx/abs(dx);
		currentDelta = sens*sqrt(dx*dx+dy*dy);

		/*Serial.print("a:");
		Serial.println(currentAlpha);

		Serial.print("d:");
		Serial.println(currentDelta);*/
		
		/* condition d'arret */
		if(abs(currentDelta) < 42){ /*si l'ecart n'est plus que de 42 ticks, on considere la consigne comme atteinte*/
			current_goal.phase = PHASE_2;
		}
	}


	pid4AlphaControl.Compute();
	pid4DeltaControl.Compute();

	if(current_goal.phase == PHASE_2){
		(*value_pwm_right) = 0;
		(*value_pwm_left) = 0;
		current_goal.isReached = true;
		initDone = false;
	}
	else{
		(*value_pwm_right) = output4Delta+output4Alpha;
		(*value_pwm_left) = output4Delta-output4Alpha;
	}




}

/* Implementation du modele d'evolution du robot a partir de l'odometrie
 * A appeler a intervalle regulier (a voir pour la mettre sur une interruption timer)
 * */
void computeRobotState(){
	static long prev_value_left_enc = 0;
	static long prev_value_right_enc = 0;

	/*calcul du deplacement depuis la dernire fois en ticks */
	long dl = value_left_enc - prev_value_left_enc;
	long dr = value_right_enc - prev_value_right_enc;

	/*prŽparation de la prochaine itŽration*/
	prev_value_left_enc = value_left_enc;
	prev_value_right_enc = value_right_enc;

	/*ce dŽplacement a ŽtŽ rŽalisŽ en un temps DUREE_CYCLE -> calcul de la vitesse en ticks/ms */
	double speed_left = (double)dl/(double)DUREE_CYCLE;
	double speed_right = (double)dr/(double)DUREE_CYCLE;
	double speed = (speed_left+speed_right)/2.0; /*estimation : simple moyenne*/

	/* mise a jour de l'orientation en rad */
	double delta_angle = (double)(dr-dl)*(double)ENC_TICKS_TO_MM/(double)ENC_CENTER_DIST;
	
	// Angle du robot par rapport ˆ l'axe X
	double angle = fmod(robot_state.angle + delta_angle,2*M_PI); /*Attention au modulo PI*/
	if (angle < 0) angle += 2*M_PI;

	/* mise a jour de la position en ticks
	 * on utilise des cos et des sin et c'est pas trs opti.
	 * A voir si l'approximation par un dev limitŽ d'ordre 2 est plus efficace
	 */
	double dx = speed*DUREE_CYCLE*cos(angle);
	double dy = speed*DUREE_CYCLE*sin(angle);

	/*mise a jour de l'Žtat du robot  */
	robot_state.speed = speed;
	robot_state.speed_left = speed_left;
	robot_state.speed_right = speed_right;
	robot_state.angle = angle;
	robot_state.x += dx;
	robot_state.y += dy;
}




