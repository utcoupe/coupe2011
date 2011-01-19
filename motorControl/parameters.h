/*
 * parameters.h
 *
 *  Created on: 16 janv. 2011
 *      Author: HoHen
 */

#ifndef PARAMETERS_H_
#define PARAMETERS_H_

/* Param�tres m�canique du robot en mm,rad*/
#define ENC_CENTER_DIST 200
#define ENC_RESOLUTION 1000
#define ENC_RADIUS 40
#define ENC_DELTA (2*M_PI*ENC_RADIUS)/ENC_RESOLUTION

/* Param�tres de la carte asservissement
 * pin 0 -> USB 2 TTL
 * pin 1 -> USB 2 TTL
 * pin 2 -> interrupt 0
 * pin 3 -> interrupt 1
 * pin 21 -> interrupt 2
 * pin 20 -> interrupt 3
 * pin 4 -> pwm
 * pin 5 -> pwm
 * pin 6 -> pwm
 * pin 7 -> pwm
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

/* Param�tre de la table en mm*/
#define TABLE_WIDTH_MM 2100
#define TABLE_HEIGHT_MM 3000
#define TABLE_DISTANCE_MAX_MM 3662

/*Param�tre de l'asservissement*/
#define K_DIST 0.5
#define K_ANGLE 700

#define KP_SPEED 100
#define KI_SPEED 5
#define KD_SPEED 20
#define KP_ANGLE 100
#define KI_ANGLE 5
#define KD_ANGLE 20
#define KP_POSITION 100
#define KI_POSITION 5
#define KD_POSITION 20

/*Param�tre de liaison s�rie*/
#define SOF '<'
#define EOF '>'

/*Param�tre de la file des buts a atteindre*/
#define SIZE 10

/*Constantes*/
#define TYPE_SPEED 0
#define TYPE_ANGLE 1
#define TYPE_POSITION 2





#endif /* PARAMETERS_H_ */
