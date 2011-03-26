/*
 * pwm.h
 *
 *  Created on: 13 janv. 2011
 *      Author: HoHen
 */

#ifndef PWM_H_
#define PWM_H_

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

void initPWM();
void setAVPWM(int);
void setARPWM(int);

#endif /* PWM_H_ */
