#ifndef PWM_H
#define PWM_H
/**
 * @author
 * (c) 2007 All Rights Reserved - This file is confidential.
 *
 * Version : $Id$
 */

/** 
 * All ADC needs : Potentiometters, Current value, voltage check.
 */ 
/** 
 * Init all PWM, using external one-chip double H bridge
 *
 */ 
 
#define PWM_MAX 32767 
 
void oInitPWM(void);	

/** 
 * Change the PWM command
 *
 * @param value from -32768 to + 32768. 0 means no PWM (full break)
 * @param pJoint is the motor number, 0 or 1
 *
 */ 
void oApplyPWM(sInt16 value, 
               uInt8 pJoint);

/** 
 * Set the hardware current limit
 *
 * @param pValue Val from 0 (I limit to 0 A) to 65535 ((I limit to max, ie 1,8 A (check !))
 * @param pJoint is the motor number, 0 or 1
 *
 */ 
void oSetIlimit(uInt16 pValue,uInt8 pJoint);

#endif
