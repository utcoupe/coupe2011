/*
 * control.cpp
 *
 *  Created on: 13 janv. 2011
 *      Author: HoHen
 */


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
PID pid4SpeedControl(&pwm,&currentSpeed,&consigne,KP_SPEED,KI_SPEED,KD_SPEED);

/* Calcule les pwm � appliquer pour un asservissement en vitesse en trapeze
 * <> value_pwm_left : la pwm a appliquer sur la roue gauche [-255,255]
 * <> value_pwm_right : la pwm a appliquer sur la roue droite [-255,255]
 * */
void speedControl(int* value_pwm_left, int* value_pwm_right){
	/* si le robot est en train de tourner, et qu'on lui donne une consigne de vitesse, il ne va pas partir droit
	 * solution = d�composer l'asservissement en vitesse en 2 :
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
		pid4SpeedControl.SetSampleTime(10); //10ms, tout ce qu'il faut c'est que l'observateur soit plus rapide que le PID
		initDone = true;
	}

	if(current_goal.phase == PHASE_1){ //phase d'acceleration

		consigne = current_goal.speed;
		currentSpeed = robot_state.speed;
		if(abs(consigne-currentSpeed) < 1){ /*si l'erreur est inf�rieur a 1, on concidere la consigne atteinte*/
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


/* Calcule les pwm � appliquer pour un asservissement en angle
 * <> value_pwm_left : la pwm a appliquer sur la roue gauche [-255,255]
 * <> value_pwm_right : la pwm a appliquer sur la roue droite [-255,255]
 * */
void angleControl(int* value_pwm_left, int* value_pwm_right){


	/* A FINIR  !!!!!*/
//	static double pwm;
//	static double consigne;
//	static double currentAngle;
//	static PID pid4AngleControl(&pwm,&currentAngle,&consigne,KP_ANGLE,KI_ANGLE,KD_ANGLE);
//
//	static bool initDone = false;
//
//	if(!initDone){
//		pwm = 0;
//		currentAngle = .0;
//		consigne = .0;
//		pid4AngleControl.Reset();
//		pid4AngleControl.SetSampleTime(10); //10ms, tout ce qu'il faut c'est que l'observateur soit plus rapide que le PID
//		initDone = true;
//	}
//
//	if(current_goal.phase == PHASE_1){
//		consigne = current_goal.angle;
//		currentAngle = robot_state.angle;
//		if(abs(consigne-currentAngle) < M_PI/180){ /*si l'erreur est inf�rieur a 1deg, on concidere la consigne atteinte*/
//			current_goal.phase = PHASE_2;
//		}
//	}
//
//
//	pid4AngleControl.Compute();
//
//
//	if(current_goal.phase == PHASE_2){
//		(*value_pwm_right) = 0;
//		(*value_pwm_left) = 0;
//		current_goal.isReached = true;
//		initDone = false;
//	}
//	else{
//		(*value_pwm_right) = pwm;
//		(*value_pwm_left) = -pwm;
//	}

}

/* Calcule les pwm � appliquer pour un asservissement en position
 * <> x : la consigne de position en x
 * <> y : la consigne de position en y
 * <> value_pwm_left : la pwm a appliquer sur la roue gauche [-255,255]
 * <> value_pwm_right : la pwm a appliquer sur la roue droite [-255,255]
 * */
void positionControl(int* value_pwm_left, int* value_pwm_right){

//	/* A FINIR  !!!!!
//
//	static int pwm4angle;
//	static int pwm4dist;
//	static double consigneAngle;
//	static double currentAngle;
//	static int consigneDist;
//	static int currentDist;
//	static PID pid4AngleControl(&pwm4angle,&currentAngle,&consigneAngle,KP_ANGLE,KI_ANGLE,KD_ANGLE);
//	static PID pid4DistanceControl(&pwm4dist,&currentDist,&consigneDist,KP_DIST,KI_DIST,KD_DIST);
//
//	static bool initDone = false;
//
//	if(!initDone){
//		start_time = 0;
//		pwm = 0;
//		currentAngle = .0;
//		consigne = .0;
//		pid4AngleControl.Reset();
//		pid4AngleControl.SetSampleTime(10); //10ms, tout ce qu'il faut c'est que l'observateur soit plus rapide que le PID
//		initDone = true;
//	}
//
//	if(current_goal.phase == PHASE_1){
//		consigne = current_goal.angle;
//		currentAngle = robot_state.angle;
//		if(abs(consigne-currentAngle) < M_PI/180){ /*si l'erreur est inf�rieur a 1deg, on concidere la consigne atteinte*/
//			current_goal.phase = PHASE_2;
//		}
//	}
//
//
//	pid4AngleControl.Compute();
//
//
//	if(current_goal.phase == PHASE_2){
//		(*value_pwm_right) = 0;
//		(*value_pwm_left) = 0;
//		current_goal.isReached = true;
//		initDone = false;
//	}
//	else{
//		(*value_pwm_right) = pwm;
//		(*value_pwm_left) = -pwm;
//	}
//
//
//
//	/*calcul de l'angle � combler avant d'�tre align� avec le point cible
//	 * borne = [0,2*PI[ */
//	double gamma = 0; /*coeff angulaire de la droite passant par le robot et le point cible*/
//	if(robot_state.x-x == 0)
//		gamma = M_PI/2;
//	else if(robot_state.y-y == 0)
//		gamma = 0;
//	else
//		gamma = atan2(robot_state.y-y,robot_state.x-x); /*arctan(y/x) -> [-PI,PI]*/
//	double delta_angle = M_PI - robot_state.angle - gamma;
//
//	/* calcul de la distance a parcourir jusqu'au point
//	 * borne [0,TABLE_DIST_MAX_MM] soit [0,3662] */
//	double delta_move = sqrt(pow(robot_state.x-x,2)+pow(robot_state.y-y,2)); /*norme simple*/
//
//	/* L'id�e est d'affecter un poids au 2 delta pour eviter que l'angle soit sous repr�sent�
//	 * par rapport a la distance � parcourir (il suffit de regarder les bornes pour s'en rendre compte)
//	 * -> les 2 poids sont K_DIST et K_ANGLE
//	 */
//	if(delta_angle>PI){
//		(*value_pwm_left) = computePID(K_DIST*delta_move+K_ANGLE*delta_angle,KP_POSITION,KI_POSITION,KD_POSITION);
//		(*value_pwm_right) = computePID(K_DIST*delta_move-K_ANGLE*delta_angle,KP_POSITION,KI_POSITION,KD_POSITION);
//	}
//	else{
//		(*value_pwm_left) = computePID(K_DIST*delta_move-K_ANGLE*delta_angle,KP_POSITION,KP_POSITION,KP_POSITION);
//		(*value_pwm_right) = computePID(K_DIST*delta_move+K_ANGLE*delta_angle,KP_POSITION,KP_POSITION,KP_POSITION);
//	}

}

/* Impl�mentation du mod�le d'�volution du robot � partir de l'odometrie
 * A appeler � intervalle r�gulier (� voir pour la mettre sur une interruption timer)
 * */
void computeRobotState(){

	static unsigned long prev_value_left_enc = 0;
	static unsigned long prev_value_right_enc = 0;
	static unsigned long prev_time = 0;

	/*calcul du deplacement depuis la derni�re fois en mm*/
	double dl = (value_left_enc - prev_value_left_enc)*ENC_DELTA;
	double dr = (value_right_enc - prev_value_right_enc)*ENC_DELTA;

	/*ce d�placement a �t� r�alis� en un temps donne -> calcul de la vitesse en mm/s*/
	double speed_left = dl/(millis()-prev_time);
	double speed_right = dr/(millis()-prev_time);
	prev_time = millis();
	double speed = (speed_left+speed_right)/2; /*estimation : simple moyenne*/

	/*mise a jour de l'orientation en rad*/
	double delta_angle = (dr-dl)/ENC_CENTER_DIST;

	double angle = fmod(robot_state.angle + delta_angle/2,2*M_PI); /*Attention au modulo PI*/
	if (angle < 0) angle += 2*M_PI; /* l'angle est maintenant entre 0 et 2*PI
	/*mise a jour de la position en mm
	 * on utilise des cos et des sin et c'est pas tr�s opti.
	 * A voir si l'approximation par un dev limit� d'ordre 2 est plus efficace
	 * */
	double delta_move = (dr+dl)/ENC_CENTER_DIST;
	double x = robot_state.x + speed*cos(robot_state.angle + delta_angle);
	double y = robot_state.y + speed*sin(robot_state.angle + delta_angle);

	/*mise a jour de l'�tat du robot  */
	robot_state.speed = speed;
	robot_state.speed_left = speed_left;
	robot_state.speed_right = speed_right;
	robot_state.angle = angle;
	robot_state.x = x;
	robot_state.y = y;

	/*pr�paration de la prochaine it�ration*/
	prev_value_left_enc = value_left_enc;
	prev_value_right_enc = value_right_enc;
}

/* Calcul la consigne � appliquer en fonction de l'erreur � combler
 * error : l'erreur � combler
 * kp : le terme proportionnel qui permet d'augmenter la vitesse de mont�e (atteint la consigne le plus rapidement possible).
 * ki : le terme int�gral qui r�duit l'erreur statique.
 * kd : le terme d�riv� qui r�duit le d�passement (l'overshoot).
 * __________________________________________________________________________________________________
 * Coefficient		|Temps de mont�e	|Temps de stabilisation	|D�passement	|Erreur statique	|
 *	kp				|Diminue			|Augmente				|Augmente		|Diminue			|
 *	ki				|Diminue			|Augmente				|Augmente		|Annule				|
 *	kd				|-					|Diminue				|Diminue		|-					|
 * _________________|___________________|_______________________|_______________|___________________|
 */
int computePID(double error,int kp,int ki,int kd){
    static double lastError=0;
    static double errSum=0;

    if(reinitPID){
    	lastError = errSum = 0;
    	reinitPID = false;
    }

    /*terme proportionnel*/
    int P = error*kp;

    /*terme integral*/
    errSum += error;
    int I = errSum*ki;

    /*terme derivee*/
    double errDif = error - lastError;
    lastError = error;
    int D = errDif*kd;

    /*somme et saturation*/
	return constrain((P+I+D)/256, -255, 255);
}



