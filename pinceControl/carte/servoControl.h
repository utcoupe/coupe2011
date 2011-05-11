#ifndef SERVO_CONTROL_H_
#define SERVO_CONTROL_H_

#define SERVO_FERME_A	50
#define SERVO_FERME_B	160
#define SERVO_SERRE_A	90
#define SERVO_SERRE_B	120
#define SERVO_OUVERT_A	130
#define SERVO_OUVERT_B	70


int setServoState(unsigned char index,unsigned char etat);
void initServo();

#endif

