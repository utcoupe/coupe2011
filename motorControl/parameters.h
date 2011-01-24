/*
 * parameters.h
 *
 *  Created on: 16 janv. 2011
 *      Author: HoHen
 */

#ifndef PARAMETERS_H_
#define PARAMETERS_H_

/* Parametres mecanique du robot */
#define ENC_CENTER_DIST (2*160) // Distance entre chaque roue codeuse et le centre du robot en mm
#define ENC_RESOLUTION 500 // Nombre de points par tour d'encodeur
#define ENC_RADIUS 18 // Rayon de la roue codeuse
#define ENC_TICKS_TO_MM (2*M_PI*ENC_RADIUS)/(ENC_RESOLUTION*4) // en mm.tick^-1 (opti: a calculer apres) environ : 0.02826

// Durée d'un cycle (en millisecondes)
#define DUREE_CYCLE 2

/* Paramètres de la carte asservissement
 * pin 0 -> USB 2 TTL
 * pin 1 -> USB 2 TTL
 * pin 3 -> enable moteur 1 (pwm)
 * pin 12 -> direction moteur 1
 * pin 11 -> enable moteur 2 (pwm)
 * pin 13 -> direction moteur 2
 * pin 18 -> A encodeur 1
 * pin 19 -> B encodeur 1
 * pin 21 -> A encodeur 2
 * pin 20 -> B encodeur 2
 * */
#define PIN_EN_LEFT 3
#define PIN_DIR_LEFT 12
#define PIN_EN_RIGHT 11
#define PIN_DIR_RIGHT 13
#define PIN_LEFT_A 18
#define PIN_LEFT_B 19
#define PIN_RIGHT_A 21
#define PIN_RIGHT_B 20
#define INTERRUPT_LEFT_A 5
#define INTERRUPT_LEFT_B 4
#define INTERRUPT_RIGHT_A 2
#define INTERRUPT_RIGHT_B 3

/* Paramètre de la table en mm*/
#define TABLE_WIDTH_MM 2100
#define TABLE_HEIGHT_MM 3000
#define TABLE_DISTANCE_MAX_MM 3662

/*Paramètre de l'asservissement*/
#define K_DIST 0.5
#define K_ANGLE 700

#define KP_SPEED 2.0
#define KI_SPEED 5.0
#define KD_SPEED 1.0
#define KP_ANGLE 2.0
#define KI_ANGLE 5.0
#define KD_ANGLE 1.0
#define KP_DELTA .5
#define KI_DELTA 0.0
#define KD_DELTA 5.0
#define KP_ALPHA .5
#define KI_ALPHA 0.0
#define KD_ALPHA 5.0

/*Paramètre de liaison série*/
#define SOF '<'
#define EOF '>'
#define T_QUERY 0
#define T_POSITION 1
#define T_SPEED 2
#define T_ANGLE 3
#define T_PAUSE 4
#define T_RESET 6
#define T_ALIGN 7
#define T_SETX 9
#define T_SETY 10



/*Paramètre de la file des buts a atteindre*/
#define SIZE 10

/*Constantes*/
#define TYPE_SPEED 0
#define TYPE_ANGLE 1
#define TYPE_POSITION 2





#endif /* PARAMETERS_H_ */
