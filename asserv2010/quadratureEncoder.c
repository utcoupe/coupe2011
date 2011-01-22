#include <p30fxxxx.h>
#include "ALTypes.h"
#include "quadratureEncoder.h"
#include "qei.h"



//#define WITH_INDEX 
#define WITHOUT_INDEX 


// Position
volatile sInt32 enQNbTurn;

// Software encodeur
sInt32 soft_POSCNT;
sInt32 soft_POSCNT_78;
sInt32 hard_POSCNT;

unsigned char soft_UPDN = 0;
unsigned char soft_QE_lastTransition;
unsigned char soft_UPDN_78 = 0;
unsigned char soft_QE_lastTransition_78;

#define QEA_UP 'A'
#define QEA_DOWN 'a'
#define QEB_UP 'B'
#define QEB_DOWN 'b'


// Interrupt on logicial EA

void __attribute__((__interrupt__, __no_auto_psv__)) _IC7Interrupt (void)
{
	//OVDCON = 0x0004;  /* PWM1L -> 1, PWM2H -> PWM */

//	LATEbits.LATE0 = 1;

	IFS1bits.IC7IF = 0;	// Cleat interrupt flag

	if (PORTBbits.RB4==1)
	{
		if (soft_QE_lastTransition_78 == QEB_DOWN)
		{
			soft_UPDN_78 = 1;
			soft_POSCNT_78++;
		} 
		else
		{
			if (soft_QE_lastTransition_78== QEB_UP)
			{
				soft_UPDN_78 = 0;
				soft_POSCNT_78--;
			}
			else
			{
				if (soft_QE_lastTransition_78== QEA_DOWN)
				{
					soft_UPDN_78 = !soft_UPDN_78;
					if (soft_UPDN_78)
					{
						soft_POSCNT_78++;
					}
					else
					{
						soft_UPDN_78--;
					}
				}
				else
				{
					// Impossible
				}
			}
		}
/*
		tmp_debug[tmp_debug_index++] = QEA_UP;
		tmp_debug[tmp_debug_index++] = '(';
		tmp_debug[tmp_debug_index++] = soft_QE_lastTransition_78;
		if (soft_UPDN)
		{	
			tmp_debug[tmp_debug_index++] = '1';
		}
		else
		{	
			tmp_debug[tmp_debug_index++] = '0';
		}
*/
		soft_QE_lastTransition_78 = QEA_UP;

	}
	else
	{
		if (soft_QE_lastTransition_78== QEB_UP)
		{
			soft_UPDN_78 = 1;
			soft_POSCNT_78++;
		} 
		else
		{ 
			if (soft_QE_lastTransition_78== QEB_DOWN)
			{
				soft_UPDN_78 = 0;
				soft_POSCNT_78--;
			} 
			else
			{
				if (soft_QE_lastTransition_78== QEA_UP)
				{
					soft_UPDN_78 = !soft_UPDN_78;
					if (soft_UPDN_78)
					{
						soft_POSCNT_78++;
					}
					else
					{
						soft_POSCNT_78--;
					}
				} 
				else
				{
					// Impossible ...
				}
			}
		}
/*
		tmp_debug[tmp_debug_index++] = QEA_DOWN;
		tmp_debug[tmp_debug_index++] = '(';
		tmp_debug[tmp_debug_index++] = soft_QE_lastTransition_78;
		if (soft_UPDN_78)
		{	
			tmp_debug[tmp_debug_index++] = '1';
		}
		else
		{	
			tmp_debug[tmp_debug_index++] = '0';
		}
*/
		soft_QE_lastTransition_78 = QEA_DOWN;
	}	


	// Software encodeur
/*long soft_POSCNT_78;
unsigned char soft_UPDN;
unsigned char soft_QE_lastTransition_78;
*/
	

/*
	HallValue = (unsigned int)((PORTB >> 3) & 0x0007);	// Read halls
	Sector = SectorTable[HallValue];	// Get Sector from table
	if (Sector != LastSector)	// This is a MUST for getting around the HW slow rate
	{
		// Calculate Hall period corresponding to half an electrical cycle
		PastCapture = ActualCapture;
		ActualCapture = IC7BUF;
		IC7BUF;
		IC7BUF;
		IC7BUF;
		// Since a new sector is detected, clear variable that would stop the motor if stalled.
		MotorStalledCounter = 0;
		// Motor current direction is computed based on Sector
		if ((Sector == 3) || (Sector == 0))	// If going forward for falling and rising then CW
			Current_Direction = CCW;
		else
			Current_Direction = CW;
	
		if (Required_Direction == CW)	// Motor commutation is actually based on the required direction, not the current dir
		{
			Phase = PhaseValues[Sector];
		}
		else
		{
			// For CCW an offset must be added to compensate difference in symmetry of the sine table used for CW and CCW
			Phase = PhaseValues[(Sector + 3) % 6] + PhaseOffset;
		}
		LastSector = Sector;	// If a change in sector, update last sector
	}
	else
	{
	}
*/
//	LATEbits.LATE0 = 0;

	return;
}

/**********************************************************************
	Hall C interrupt, this is done with the input change notification IC8.
	The purpose of this ISR is to Calculate the actual mechanical
	direction of rotation of the motor, and to adjust the Phase variable
	depending on the sector the rotor is in.
	Note 1: The sector must be validated in order to avoid any spurious
	interrupt due to a slow slew rate on the halls inputs due to hardware
	filtering.
	Note 2: For Phase adjustment in CCW, an offset must be added to
	compensate non-symetries in the sine table used.
**********************************************************************/

// Interrupt on logicial EB


void __attribute__((__interrupt__, __no_auto_psv__)) _IC8Interrupt (void)
{	
//	LATEbits.LATE5 = 1;

	IFS1bits.IC8IF = 0;	// Cleat interrupt flag

	if (PORTBbits.RB5)
	{
		if (soft_QE_lastTransition_78== QEA_UP)
		{
			soft_UPDN_78 = 1;
			soft_POSCNT_78++;
		} 
		else
		{
			if (soft_QE_lastTransition_78== QEA_DOWN)
			{
				soft_UPDN_78 = 0;
				soft_POSCNT_78--;
			} 
			else
  			{
				if (soft_QE_lastTransition_78== QEB_DOWN)
				{
					soft_UPDN_78 = !soft_UPDN_78;
					if (soft_UPDN_78)
					{
						soft_POSCNT_78++;
					}
					else
					{
						soft_POSCNT_78--;
					}
				} 
				else
				{
					// Impossible ...
				}
			}
		}
/*
		tmp_debug[tmp_debug_index++] = QEB_UP;
		tmp_debug[tmp_debug_index++] = '(';
		tmp_debug[tmp_debug_index++] = soft_QE_lastTransition_78;
		if (soft_UPDN_78)
		{	
			tmp_debug[tmp_debug_index++] = '1';
		}
		else
		{	
			tmp_debug[tmp_debug_index++] = '0';
		}
*/
		soft_QE_lastTransition_78 = QEB_UP;

	}
	else
	{
		if (soft_QE_lastTransition_78== QEA_DOWN)
		{
			soft_UPDN_78 = 1;
			soft_POSCNT_78++;
		} 
		else
		{ 
			if (soft_QE_lastTransition_78== QEA_UP)
			{
				soft_UPDN_78 = 0;
				soft_POSCNT_78--;
			} 
			else
			{
				if (soft_QE_lastTransition_78== QEB_UP)
				{
					soft_UPDN_78 = !soft_UPDN_78;
					if (soft_UPDN_78)
					{
						soft_POSCNT_78++;
					}
					else
					{
						soft_POSCNT_78--;
					}
				} 
				else
				{
					// Impossible ...
				}
			}
		}

/*
		tmp_debug[tmp_debug_index++] = QEB_DOWN;
		tmp_debug[tmp_debug_index++] = '(';
		tmp_debug[tmp_debug_index++] = soft_QE_lastTransition_78;
		if (soft_UPDN_78)
		{	
			tmp_debug[tmp_debug_index++] = '1';
		}
		else
		{	
			tmp_debug[tmp_debug_index++] = '0';
		}
*/		
		soft_QE_lastTransition_78 = QEB_DOWN;
	}	

//	LATEbits.LATE5 = 0;



/*	HallValue = (unsigned int)((PORTB >> 3) & 0x0007);	// Read halls
	Sector = SectorTable[HallValue];	// Get Sector from table
	if (Sector != LastSector)	// This is a MUST for getting around the HW slow rate
	{
		// Since a new sector is detected, clear variable that would stop the motor if stalled.
		MotorStalledCounter = 0;
		// Motor current direction is computed based on Sector
		if ((Sector == 1) || (Sector == 4))	// If going forward for falling and rising then CW
			Current_Direction = CCW;
		else
			Current_Direction = CW;
		
		if (Required_Direction == CW)	// Motor commutation is actually based on the required direction, not the current dir
		{
			Phase = PhaseValues[Sector];
		}
		else
		{
			// For CCW an offset must be added to compensate difference in symmetry of the sine table used for CW and CCW
			Phase = PhaseValues[(Sector + 3) % 6] + PhaseOffset;
		}
		LastSector = Sector;	// If a change in sector, update last sector
	}
	else
	{
	}
*/


	return;
}



/**********************************************************************
	Hall B interrupt, this is done with the input change notification IC7.
	The purpose of this ISR is to Calculate the actual mechanical
	direction of rotation of the motor, and to adjust the Phase variable
	depending on the sector the rotor is in AND ALSO to calculate the
	mechanical Period between hall transition on the same input using
	input capture read.
	Note 1: The sector must be validated in order to avoid any spurious
	interrupt due to a slow slew rate on the halls inputs due to hardware
	filtering.
	Note 2: For Phase adjustment in CCW, an offset must be added to
	compensate non-symetries in the sine table used.
**********************************************************************/

// Interrupt on logicial EA

void __attribute__((__interrupt__, __no_auto_psv__)) _IC2Interrupt (void)
{
	//OVDCON = 0x0004;  /* PWM1L -> 1, PWM2H -> PWM */

//	LATDbits.LATD1 = 1;

	IFS0bits.IC2IF = 0;	// Clear interrupt flag

	if (PORTDbits.RD1==1)
	{
		if (soft_QE_lastTransition== QEB_DOWN)
		{
			soft_UPDN = 1;
			soft_POSCNT++;
		} 
		else
		{
			if (soft_QE_lastTransition== QEB_UP)
			{
				soft_UPDN = 0;
				soft_POSCNT--;
			}
			else
			{
				if (soft_QE_lastTransition== QEA_DOWN)
				{
					soft_UPDN = !soft_UPDN;
					if (soft_UPDN)
					{
						soft_POSCNT++;
					}
					else
					{
						soft_POSCNT--;
					}
				}
				else
				{
					// Impossible
				}
			}
		}
/*
		tmp_debug[tmp_debug_index++] = QEA_UP;
		tmp_debug[tmp_debug_index++] = '(';
		tmp_debug[tmp_debug_index++] = soft_QE_lastTransition;
		if (soft_UPDN)
		{	
			tmp_debug[tmp_debug_index++] = '1';
		}
		else
		{	
			tmp_debug[tmp_debug_index++] = '0';
		}
*/
		soft_QE_lastTransition = QEA_UP;

	}
	else
	{
		if (soft_QE_lastTransition== QEB_UP)
		{
			soft_UPDN = 1;
			soft_POSCNT++;
		} 
		else
		{ 
			if (soft_QE_lastTransition== QEB_DOWN)
			{
				soft_UPDN = 0;
				soft_POSCNT--;
			} 
			else
			{
				if (soft_QE_lastTransition== QEA_UP)
				{
					soft_UPDN = !soft_UPDN;
					if (soft_UPDN)
					{
						soft_POSCNT++;
					}
					else
					{
						soft_POSCNT--;
					}
				} 
				else
				{
					// Impossible ...
				}
			}
		}
/*
		tmp_debug[tmp_debug_index++] = QEA_DOWN;
		tmp_debug[tmp_debug_index++] = '(';
		tmp_debug[tmp_debug_index++] = soft_QE_lastTransition;
		if (soft_UPDN)
		{	
			tmp_debug[tmp_debug_index++] = '1';
		}
		else
		{	
			tmp_debug[tmp_debug_index++] = '0';
		}
*/
		soft_QE_lastTransition = QEA_DOWN;
	}	


	// Software encodeur
/*long soft_POSCNT;
unsigned char soft_UPDN;
unsigned char soft_QE_lastTransition;
*/
	

/*
	HallValue = (unsigned int)((PORTB >> 3) & 0x0007);	// Read halls
	Sector = SectorTable[HallValue];	// Get Sector from table
	if (Sector != LastSector)	// This is a MUST for getting around the HW slow rate
	{
		// Calculate Hall period corresponding to half an electrical cycle
		PastCapture = ActualCapture;
		ActualCapture = IC7BUF;
		IC7BUF;
		IC7BUF;
		IC7BUF;
		// Since a new sector is detected, clear variable that would stop the motor if stalled.
		MotorStalledCounter = 0;
		// Motor current direction is computed based on Sector
		if ((Sector == 3) || (Sector == 0))	// If going forward for falling and rising then CW
			Current_Direction = CCW;
		else
			Current_Direction = CW;
	
		if (Required_Direction == CW)	// Motor commutation is actually based on the required direction, not the current dir
		{
			Phase = PhaseValues[Sector];
		}
		else
		{
			// For CCW an offset must be added to compensate difference in symmetry of the sine table used for CW and CCW
			Phase = PhaseValues[(Sector + 3) % 6] + PhaseOffset;
		}
		LastSector = Sector;	// If a change in sector, update last sector
	}
	else
	{
	}
*/
//	LATEbits.LATE0 = 0;

	return;
}

/**********************************************************************
	Hall C interrupt, this is done with the input change notification IC8.
	The purpose of this ISR is to Calculate the actual mechanical
	direction of rotation of the motor, and to adjust the Phase variable
	depending on the sector the rotor is in.
	Note 1: The sector must be validated in order to avoid any spurious
	interrupt due to a slow slew rate on the halls inputs due to hardware
	filtering.
	Note 2: For Phase adjustment in CCW, an offset must be added to
	compensate non-symetries in the sine table used.
**********************************************************************/

// Interrupt on logicial EB


void __attribute__((__interrupt__, __no_auto_psv__)) _IC1Interrupt (void)
{	
//	LATEbits.LATE5 = 1;

	IFS0bits.IC1IF = 0;	// Cleat interrupt flag

	if (PORTDbits.RD0)
	{
		if (soft_QE_lastTransition== QEA_UP)
		{
			soft_UPDN = 1;
			soft_POSCNT++;
		} 
		else
		{
			if (soft_QE_lastTransition== QEA_DOWN)
			{
				soft_UPDN = 0;
				soft_POSCNT--;
			} 
			else
  			{
				if (soft_QE_lastTransition== QEB_DOWN)
				{
					soft_UPDN = !soft_UPDN;
					if (soft_UPDN)
					{
						soft_POSCNT++;
					}
					else
					{
						soft_POSCNT--;
					}
				} 
				else
				{
					// Impossible ...
				}
			}
		}
/*
		tmp_debug[tmp_debug_index++] = QEB_UP;
		tmp_debug[tmp_debug_index++] = '(';
		tmp_debug[tmp_debug_index++] = soft_QE_lastTransition;
		if (soft_UPDN)
		{	
			tmp_debug[tmp_debug_index++] = '1';
		}
		else
		{	
			tmp_debug[tmp_debug_index++] = '0';
		}
*/
		soft_QE_lastTransition = QEB_UP;

	}
	else
	{
		if (soft_QE_lastTransition== QEA_DOWN)
		{
			soft_UPDN = 1;
			soft_POSCNT++;
		} 
		else
		{ 
			if (soft_QE_lastTransition== QEA_UP)
			{
				soft_UPDN = 0;
				soft_POSCNT--;
			} 
			else
			{
				if (soft_QE_lastTransition== QEB_UP)
				{
					soft_UPDN = !soft_UPDN;
					if (soft_UPDN)
					{
						soft_POSCNT++;
					}
					else
					{
						soft_POSCNT--;
					}
				} 
				else
				{
					// Impossible ...
				}
			}
		}

/*
		tmp_debug[tmp_debug_index++] = QEB_DOWN;
		tmp_debug[tmp_debug_index++] = '(';
		tmp_debug[tmp_debug_index++] = soft_QE_lastTransition;
		if (soft_UPDN)
		{	
			tmp_debug[tmp_debug_index++] = '1';
		}
		else
		{	
			tmp_debug[tmp_debug_index++] = '0';
		}
*/		
		soft_QE_lastTransition = QEB_DOWN;
	}	

//	LATEbits.LATE5 = 0;



/*	HallValue = (unsigned int)((PORTB >> 3) & 0x0007);	// Read halls
	Sector = SectorTable[HallValue];	// Get Sector from table
	if (Sector != LastSector)	// This is a MUST for getting around the HW slow rate
	{
		// Since a new sector is detected, clear variable that would stop the motor if stalled.
		MotorStalledCounter = 0;
		// Motor current direction is computed based on Sector
		if ((Sector == 1) || (Sector == 4))	// If going forward for falling and rising then CW
			Current_Direction = CCW;
		else
			Current_Direction = CW;
		
		if (Required_Direction == CW)	// Motor commutation is actually based on the required direction, not the current dir
		{
			Phase = PhaseValues[Sector];
		}
		else
		{
			// For CCW an offset must be added to compensate difference in symmetry of the sine table used for CW and CCW
			Phase = PhaseValues[(Sector + 3) % 6] + PhaseOffset;
		}
		LastSector = Sector;	// If a change in sector, update last sector
	}
	else
	{
	}
*/


	return;
}
/*

void __attribute__((__interrupt__, __no_auto_psv__)) _QEIInterrupt(void)
{
	IFS2bits.QEIIF = 0;

	//pos = enQNbTurn*2048 + POSCNT;
	// Positive mvt
	if (QEICONbits.UPDN)
	{
		// Index interrupt : new turn up
		enQNbTurn++;
	}
	else
	{
		// Index interrupt : new turn down
		enQNbTurn--;
	}

}
*/
void InitQEI(void)
{
// Hardware QEI
/*
#ifdef WITH_INDEX 

	MAXCNT = 512*4; // MAXCNT register.

	// Quadrature encoder interface enabled (*4 mode) with index pulse reset of position counter
	QEICONbits.QEIM = 6; // 110
	// Index pulse resets counter
	QEICONbits.POSRES = 1; 


	// Index match value
	DFLTCONbits.IMV = 3;

	// Interrupt due to position count errors disabled
	DFLTCONbits.CEID = 1; 

	// QEA/QEB Digital filter Output enable bit.
	DFLTCONbits.QEOUT = 1;
	// Digital filter clock divide select bits
	DFLTCONbits.QECK = 3; // 1:16 clock divide

	// Index channel Digital filter Output enable bit.
	//DFLTCONbits.INDOUT = 1;
	// Inex channel Digital filter clock divide select bits
	//DFLTCONbits.INDCK = 3; // 1:16 clock divide

	// Enable QEI interrupt
	IEC2bits.QEIIE = 1;	

	//pos = enQNbTurn*2048 + POSCNT;
#endif

#ifdef WITHOUT_INDEX 
	
	MAXCNT = 32767; // MAXCNT register.

	// Quadrature encoder interface enabled (*4 mode) with position counter reset by match
	QEICONbits.QEIM = 7; // 111
	// Index pulse does not resets counter
	QEICONbits.POSRES = 0; 

	// Interrupt due to position count errors disabled
	DFLTCONbits.CEID = 1; 

	// QEA/QEB Digital filter Output enable bit.
	DFLTCONbits.QEOUT = 1;
	// Digital filter clock divide select bits
	DFLTCONbits.QECK = 3; // 1:16 clock divide

	// Index channel Digital filter Output enable bit.
	//DFLTCONbits.INDOUT = 1;
	// Inex channel Digital filter clock divide select bits
	//DFLTCONbits.INDCK = 3; // 1:16 clock divide

	// Enable QEI interrupt
	IEC2bits.QEIIE = 1;	

	// Set to 0
	POSCNT = 0;

	enQNbTurn = 0; // No turn

#endif
*/

	//unsigned int ReadQEI(void);

// software QEI
  IC1CONbits.ICM = 1; // enable input capture 2
	IC2CONbits.ICM = 1; // enable input capture 2
  IC7CONbits.ICM = 1; // enable input capture 2
	IC8CONbits.ICM = 1; // enable input capture 2
	
	IFS0bits.IC1IF = 0;	// Clear interrupt flag
	IFS0bits.IC2IF = 0;	// Clear interrupt flag
	IFS1bits.IC7IF = 0;	// Clear interrupt flag
	IFS1bits.IC8IF = 0;	// Clear interrupt flag

  CNPU1bits.CN7PUE = 1; // enable pull up on QEI B
  CNPU1bits.CN6PUE = 1; // enable pull up on QEI A
  
	IEC0bits.IC1IE = 0;	// Enable interrupts on IC1
	IEC0bits.IC2IE = 0;	// Enable interrupts on IC2
	IEC1bits.IC7IE = 0;	// Enable interrupts on IC7
	IEC1bits.IC8IE = 0;	// Enable interrupts on IC8
	
	// Set priority max for IC1 and IC2 (software QE)
	IPC0bits.IC1IP = 7;
	IPC1bits.IC2IP = 7;
	// Set priority max for IC7 and IC8 (software QE)
	IPC4bits.IC8IP = 7;
	IPC4bits.IC7IP = 7;
		
	soft_POSCNT = 0;
	soft_POSCNT_78 = 0;
//	POSCNT = 0;
//	enQNbTurn = 0; // No turn
}
/*
sInt32 getHardwarePosQEI()
{
  hard_POSCNT = enQNbTurn*32768 + (sInt32)POSCNT;
	return -hard_POSCNT;
}
*/
sInt32 getSoftwarePosQEI()
{
	return -soft_POSCNT;
}

sInt32 getSoftwarePosQEI2()
{
	return -soft_POSCNT_78;
}
