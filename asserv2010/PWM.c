/**
 * @author 
 * (c) 2007 All Rights Reserved - This file is confidential.
 *
 * Version : $Id$
 */

/**
 * All ADC needs : Potentiometters, Current value, voltage check.
 */ 
//Pre-Processor Directives:
#include <p30fxxxx.h>
#include "ALTypes.h"
#include "System.h"
#include "PWM.h"
#include "config.h"

/**
 * PWM frequency, 60 kHz, so that no audible noise is present.
 */
#define FPWM 60000		

#define USE_A3995


//Break
#define BREAK_UP_A3995 0x0105 
// 01 : 1L = enable = en PWM
// OE : 2L = mode = 0 ; 1H = phase = sens (here 1) ; 1L = Enable = 0 (default if no PWM) ; 2H = VREF curent limit = 1 (if no PWM)   
#define BREAK_DOWN_A3995 0x0105
// 01 : 1L = enable = en PWM
// OC : 2L = mode = 0 ; 1H = phase = sens (here 0) ; 1L = Enable = 0 (default if no PWM) ; 2H = VREF curent limit = 1 (if no PWM)   


//Break
//#define BREAK_UP_A3995 0x010A
//#define BREAK_UP_A3995 0x0D0E
// 01 : 1L = enable = en PWM + 2H = VREF curent limit = PWM   
// OE : 2L = mode = 1 ; 1H = phase = sens (here 1) ; 1L = Enable = 0 (default if no PWM) ; 

//#define BREAK_DOWN_A3995_1 0x3F00
//#define BREAK_DOWN_A3995_2 0x0303
// 01 : 1L = enable = en PWM + ; 2H = VREF curent limit = PWM
// OC : 2L = mode = 1 ; 1H = phase = sens (here 0) ; 1L = Enable = 0 (default if no PWM) 


#define PWM_DEAD_ZONE 1000
/** 
 * Init all PWM, using external one-chip double H bridge
 *
 */ 
void oInitPWM(void)
{
	TRISE = 0x0100;	// PWM pins as outputs, and FLTA as input
  PTCON = 0x0002;		// start PWM as center aligned mode UP/DOWN mode continous
	DTCON1 = 0x0000;	// ~500 ns of dead time
	OVDCON = 0x0000;	// Disable all PWM outputs.
  PTPER = (FCY/FPWM - 1) >> 1;	// Compute Period based on CPU speed and required PWM frequency (see defines)
  PDC1 = PTPER;	// Initialize as 0 voltage
	PDC2 = PTPER;	// Initialize as 0 voltage
	PDC3 = PTPER;	// Initialize as 0 voltage

	PWMCON1 = 0xFF03;	//  Enable 1L 2L 3L PWM output pins and configure them as complementary mode		
	PWMCON2 = 0x0F00;	// 16 postscale values, for achieving 20 kHz 
	SEVTCMP = PTPER-2;	// Enable triggering for just at the end of the PWM
  SEVTCMPbits.SEVTDIR = 0; // when counting down
  PTCON = 0x8002;		// start PWM as center aligned mode
  OVDCON = 0x0500;
  oApplyPWM( 0, DROITE);
	oApplyPWM( 0, GAUCHE);
  return;	
}


/** 
 * Change the PWM command
 *
 * @param value from -32768 to + 32767 0 means no PWM (full break)
 * @param pJoint is the motor number, 0 or 1
 *
 */ 
#define PWM_PHASE_1 LATEbits.LATE1
#define PWM_PHASE_2 LATEbits.LATE3

void oApplyPWM(sInt16 value, 
               uInt8 pJoint)
{
  uInt16 PWMMoteur;
  JointConfig* jointInfo;

  // Get config 
  jointInfo = oGetJointConfig(pJoint);

	// Zone morte
	if (value >0)
	{
		if (value<PWM_DEAD_ZONE)
		{
			value = 0;
		}
	}
	if (value <0)
	{
		if ((-value)<PWM_DEAD_ZONE)
		{
			value = 0;
		}
	}

	PWMMoteur = (sInt32)value+(sInt32)32768;

	// PWM
	if (PWMMoteur >= 32768)
	{
		uInt16 val = PWMMoteur-32767;
		
    val = 32768-val;
		// Apply for all PWM
    if (jointInfo->motor.motorPosition == 1)
    { 
      PDC1 = ((sInt32)val*(sInt32)PTPER)/16384 +3;
      PWM_PHASE_1 = 1;    
    }
    if (jointInfo->motor.motorPosition == 2) 
    {
      PDC2 = ((sInt32)val*(sInt32)PTPER)/16384 +3;
      PWM_PHASE_2 = 1;    
    }	  
  }
	else
	{	
		uInt16 val = 32767-PWMMoteur;
    val = 32768-val;
    if (jointInfo->motor.motorPosition == 1)
    {
  		PDC1 = ((sInt32)val*(sInt32)PTPER)/16384 +3;
  		PWM_PHASE_1 = 0;
    }
    if (jointInfo->motor.motorPosition == 2)
    {
  		PDC2 = ((sInt32)val*(sInt32)PTPER)/16384 +3;
  		PWM_PHASE_2 = 0;
    }      
	}	
}
