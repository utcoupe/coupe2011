/*
 * control.cpp
 *
 *  Created on: 13 janv. 2011
 *      Author: HoHen
 */

#include "control.h"
#include "encoder.h"
#include "robotstate.h"

bool reinitPID;
CurrentGoal current_goal;


void initController(){
	current_goal.isReached = true;
	reinitPID = true;
}

/* Calcule les pwm à appliquer pour un asservissement en vitesse
 * <> speed : la consigne de vitesse en mm/s
 * <> value_pwm_left : la pwm a appliquer sur la roue gauche [-255,255]
 * <> value_pwm_right : la pwm a appliquer sur la roue droite [-255,255]
 * <> nb_period : le nombre d'iteration avant la phase de decceleration
 * */
void speedControl(int speed,int* value_pwm_left, int* value_pwm_right, int nb_period){
	/* si le robot est en train de tourner, et qu'on lui donne une consigne de vitesse, il ne va pas partir droit
	 * solution = décomposer l'asservissement en vitesse en 2 :
	 * -> stopper le robot (les 2 vitesses = 0)
	 * -> lancer l'asservissement en vitesse
	 */

	static int period_left = 0;

	if(period_left == 0) period_left = nb_period;
	else period_left--;

	if(period_left > 0){
		//phase acceleration + regime permanent
		(*value_pwm_left) = computePID(robot_state.speed_left-speed,KP_SPEED,KI_SPEED,KD_SPEED);
		(*value_pwm_right) = computePID(robot_state.speed_right-speed,KP_SPEED,KI_SPEED,KD_SPEED);
	}
	else{
		//phase de decceleration
		(*value_pwm_left) = computePID(robot_state.speed_left,KP_SPEED,KI_SPEED,KD_SPEED);
		(*value_pwm_right) = computePID(robot_state.speed_right,KP_SPEED,KI_SPEED,KD_SPEED);
		period_left = 0;
	}
}


/* Calcule les pwm à appliquer pour un asservissement en angle
 * <> angle : la consigne d'orientation en rad
 * <> value_pwm_left : la pwm a appliquer sur la roue gauche [-255,255]
 * <> value_pwm_right : la pwm a appliquer sur la roue droite [-255,255]
 * */
void angleControl(int angle,int* value_pwm_left, int* value_pwm_right){

	int pwm = computePID(robot_state.angle-angle,KP_ANGLE,KI_ANGLE,KD_ANGLE);

	if(angle>PI){
		(*value_pwm_left) = pwm;
		(*value_pwm_right) = -pwm;
	}else{
		(*value_pwm_left) = -pwm;
		(*value_pwm_right) = pwm;
	}
}

/* Calcule les pwm à appliquer pour un asservissement en position
 * <> x : la consigne de position en x
 * <> y : la consigne de position en y
 * <> value_pwm_left : la pwm a appliquer sur la roue gauche [-255,255]
 * <> value_pwm_right : la pwm a appliquer sur la roue droite [-255,255]
 * */
void positionControl(int x,int y,int* value_pwm_left, int* value_pwm_right){

	/*calcul de l'angle à combler avant d'être aligné avec le point cible
	 * borne = [0,2*PI[ */
	double gamma = 0; /*coeff angulaire de la droite passant par le robot et le point cible*/
	if(robot_state.x-x == 0)
		gamma = M_PI/2;
	else if(robot_state.y-y == 0)
		gamma = 0;
	else
		gamma = atan2(robot_state.y-y,robot_state.x-x); /*arctan(y/x) -> [-PI,PI]*/
	double delta_angle = M_PI - robot_state.angle - gamma;

	/* calcul de la distance a parcourir jusqu'au point
	 * borne [0,TABLE_DIST_MAX_MM] soit [0,3662] */
	double delta_move = sqrt(pow(robot_state.x-x,2)+pow(robot_state.y-y,2)); /*norme simple*/

	/* L'idée est d'affecter un poids au 2 delta pour eviter que l'angle soit sous représenté
	 * par rapport a la distance à parcourir (il suffit de regarder les bornes pour s'en rendre compte)
	 * -> les 2 poids sont K_DIST et K_ANGLE
	 */
	if(delta_angle>PI){
		(*value_pwm_left) = computePID(K_DIST*delta_move+K_ANGLE*delta_angle,KP_SPEED,KI_SPEED,KD_SPEED);
		(*value_pwm_right) = computePID(K_DIST*delta_move-K_ANGLE*delta_angle,KP_SPEED,KI_SPEED,KD_SPEED);
	}
	else{
		(*value_pwm_left) = computePID(K_DIST*delta_move-K_ANGLE*delta_angle,KP_SPEED,KI_SPEED,KD_SPEED);
		(*value_pwm_right) = computePID(K_DIST*delta_move+K_ANGLE*delta_angle,KP_SPEED,KI_SPEED,KD_SPEED);
	}

}

/* Implémentation du modèle d'évolution du robot à partir de l'odometrie
 * A appeler à intervalle régulier (à voir pour la mettre sur une interruption timer)
 * */
void computeRobotState(){

	static unsigned long prev_value_left_enc = 0;
	static unsigned long prev_value_right_enc = 0;
	static unsigned long prev_time = 0;

	/*calcul du deplacement depuis la dernière fois en mm*/
	double dl = (value_left_enc - prev_value_left_enc)*ENC_DELTA;
	double dr = (value_right_enc - prev_value_right_enc)*ENC_DELTA;

	/*ce déplacement a été réalisé en un temps donne -> calcul de la vitesse en mm/s*/
	double speed_left = dl/(millis()-prev_time);
	double speed_right = dr/(millis()-prev_time);
	double speed = (speed_left+speed_right)/2; /*estimation : simple moyenne*/

	/*mise a jour de l'orientation en rad*/
	double delta_angle = (dr-dl)/ENC_CENTER_DIST;
	double angle = fmod(robot_state.angle + delta_angle,PI); /*Attention au modulo PI*/

	/*mise a jour de la position en mm
	 * on utilise des cos et des sin et c'est pas très opti.
	 * A voir si l'approximation par un dev limité d'ordre 2 est plus efficace
	 * */
	double delta_move = (dr+dl)/ENC_CENTER_DIST;
	double x = robot_state.x + delta_move*cos(robot_state.angle + delta_angle/2);
	double y = robot_state.y + delta_move*sin(robot_state.angle + delta_angle/2);

	/*mise a jour de l'état du robot  */
	robot_state.speed = speed;
	robot_state.speed_left = speed_left;
	robot_state.speed_right = speed_right;
	robot_state.angle = angle;
	robot_state.x = x;
	robot_state.y = y;

	/*préparation de la prochaine itération*/
	prev_value_left_enc = value_left_enc;
	prev_value_right_enc = value_right_enc;
	prev_time = millis();
}

/* Calcul la consigne à appliquer en fonction de l'erreur à combler
 * error : l'erreur à combler
 * kp : le terme proportionnel qui permet d'augmenter la vitesse de montée (atteint la consigne le plus rapidement possible).
 * ki : le terme intégral qui réduit l'erreur statique.
 * kd : le terme dérivé qui réduit le dépassement (l'overshoot).
 * __________________________________________________________________________________________________
 * Coefficient		|Temps de montée	|Temps de stabilisation	|Dépassement	|Erreur statique	|
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



/* Vérifie si le but a été atteint.
 * Inutile de continuer à calculer les pwm aux prochaines itérations si c'est le cas
 */
void checkCurrentGoal(){
	switch (current_goal.type) {
		case TYPE_SPEED:
			/* A plus ou moins 10 mm/s, on concidere que le but est atteint*/
			if(abs(current_goal.speed-robot_state.speed) < 10)
				current_goal.isReached = true;
			break;
		case TYPE_ANGLE:
			/* A plus ou moins 1 deg, on concidere que le but est atteint*/
			if(abs(current_goal.angle-robot_state.angle) < 0.0017)
				current_goal.isReached = true;
			break;
		case TYPE_POSITION:
			/* A plus ou moins 2 mm, on concidere que le but est atteint*/
			if(abs(current_goal.x-robot_state.x) < 2 && abs(current_goal.y-robot_state.y) < 2)
				current_goal.isReached = true;
			break;
		default:
			break;
	}

}

