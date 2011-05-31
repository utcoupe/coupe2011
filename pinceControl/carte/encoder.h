#ifndef ENCODER_H_
#define ENCODER_H_

#define PIN_AR_A 18 //bleu //arriere
#define PIN_AR_B 19 //blanc //arriere
#define PIN_AV_A 20 //bleu //avant
#define PIN_AV_B 21 //blanc //avant
#define INTERRUPT_AR_A 5
#define INTERRUPT_AR_B 4
#define INTERRUPT_AV_A 2
#define INTERRUPT_AV_B 3
#define PIN_MS_RECAL_AV_HAUT	41 //21
#define PIN_MS_RECAL_AV_BAS		43 //20
#define PIN_MS_RECAL_AR_HAUT	47 //18
#define PIN_MS_RECAL_AR_BAS		45 //19

#include "pinceControl.h"
#include "wiring.h"


void initEncoders();
void testAR();
void testAV();
void encoderSafe();

/*The most common type of incremental encoder uses two output channels (A and B) to sense position.
 *  the two output channels of the quadrature encoder indicate both position and direction of rotation.
 *  If A leads B, for example, the disk is rotating in a clockwise direction.
 *  If B leads A, then the disk is rotating in a counter-clockwise direction.
 *  */

void valueChangeOnEncoderARPinA();
void valueChangeOnEncoderARPinB();
void valueChangeOnEncoderAVPinA();
void valueChangeOnEncoderAVPinB();
/*void valueChangeOnMSRecalAVBas();
void valueChangeOnMSRecalAVHaut();
void valueChangeOnMSRecalARBas();
void valueChangeOnMSRecalARHaut();*/

extern long value_AR_enc;//AR
extern long value_AV_enc;//AV
extern long goal_position_AV;
extern long goal_position_AR;


#endif /* ENCODER_H_ */
