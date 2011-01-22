

//Pre-Processor Directives:
//Provide pre-processor directives to include Device header files and
//any application specific header files. Path locations to the Device
//header files are set up within the MPLAB Project>>BuildOptions>>Project
//dialog box. The path should point to X:\\MPLAB C30\support\h\, where
//"X:" is the folder where the MPLAB C30 tools are installed.
#include <p30fxxxx.h>   //"p30fxxxx.h" is a generic header file for dsPIC30F
                        //devices. This file will in turn select the actual
                        //device header file, based on the device selected
                        //by the MPLAB workspace/project. Using the p30fxxxx.h
                        //file makes it easy to migrate a project from one
                        //dsPIC device to another.
#include "system.h"     //"system.h" is a header file defined for this demo
                        //application.
//Macros for Configuration Fuse Registers:
//Invoke macros to set up  device configuration fuse registers.
//The fuses will select the oscillator source, power-up timers, watch-dog
//timers, BOR characteristics etc. The macros are defined within the device
//header files. The configuration fuse registers reside in Flash memory.
_FOSC(CSW_FSCM_OFF & FRC_PLL16);  //Run this project using an external crystal
                                //routed via the PLL in 8x multiplier mode
                                //For the 7.3728 MHz crystal we will derive a
                                //throughput of 7.3728e+6*8/4 = 14.74 MIPS(Fcy)
                                //,~67nanoseconds instruction cycle time(Tcy).
_FWDT(WDT_OFF);                 //Turn off the Watch-Dog Timer.
_FBORPOR(MCLR_EN & PWRT_64 & PWMxL_ACT_LO & PWMxH_ACT_LO);   //Enable MCLR reset pin and turn off the
                                //power-up timers.
_FGS(CODE_PROT_OFF);            //Disable Code Protection

//Declaration to Link External Functions & Variables:
//Declare functions being used that are defined outside this file, or
//elsewhere in this MPLAB project.
#include "ALTypes.h"
#include "spi_for_lcd.h"
#include "uart.h"
#include "quadratureEncoder.h"
#include "display_routines.h"
#include "intx_io_pins.h"
#include "delay.h"
#include "timers.h"
#include "ConversionAN.h"
#include "PWM.h"
#include "config.h"
#include "position.h"
#include "asserv.h"
#include "asservDistance.h"
#include "asservOrientation.h"
#include "speedAsserv.h"
#include "i2c.h" 
//Functions and Variables with Global Scope:
//Declare functions in this file that have global scope.
extern uInt8 gOneMsTemp;
extern uInt16 TruePositionReceived;
extern float distanceTheorique;
uInt8 noCommandTimeOutFlag = 0;
uInt32 timeSpentWithoutCommand = 0;
uInt32 timeSpentSinceLastNewCommand = 0;
uInt32 wrongCommand = 0;

//Code execution automatically reaches the main() function after
//two events have occurred;
//1. A Reset event triggered by hardware or software
//2. The execution of the C Start up library functions, present
//   in the crt0.o file in the libpic30-coff.a library file
int main (void)
{
  float ecart_d,ecart_rot;
  static volatile uInt8 planificationAsked=0;
  robot * Robot;
  	while (OSCCONbits.LOCK != 1) {}; // Issue : block the debug ...
  ADPCFG = 0xFFFF;        //After reset all port pins multiplexed
                          //with the A/D converter are configred analog.
                          //We will reconfigure them to be digital
                          //by writing all 1's to the ADPCFG register.
                          //Note: All dsPIC registers are memory mapped.
                          //The address of ADPCFG and other registers
                          //are defined in the device linker script
                          //in your project.

  //Function UART_Init() available in file, UART.c
  //UART_Init();            //Initialize the UART module to communicate
                                //with the COM port on the Host PC via an
                                //RS232 cable and the DB9 connector.
  oInitConfig();
  initRobot();
  initPid();
  InitQEI();
  oInitPWM();
  oInitI2C();
//  Function INTx_IO_Init() available in file, INTx_IO_pins.c
  INTx_IO_Init();         //Initialize the External interrupt pins and
                      //some I/O pins to accept input from the
                      //switches, S5 and S6 and drive the LEDs, D3
                      //and D4.
  
  // Function Timer1_Init() & Timer2_Init() available in file, Timers.c
  Timer1_Init();          //Initialize Timer1 to provide "blinking" time
                      //for the LEDs.

  TRISEbits.TRISE5 = 0;  // use to measure time spent 
 
 // RECUPERATION DES PARAMETRE DU ROBOT
  Robot = getRobot();
  while (1)               //Main Loop of Code Executes forever
  {
    while (!gOneMsTemp)
    ;
    LATEbits.LATE5 = 1;
    gOneMsTemp = 0;

    
    // RECUPERATION DES ECARTS
  	if (TruePositionReceived)
  	{
      calcul_position();
    	ecart_d = calculConsigne_d(Robot->target);
    	ecart_rot = calculEcart_rot(Robot->target);
    	asservissementVitesse(&ecart_d, &ecart_rot);
      asservissementPosition(ecart_d, ecart_rot);
      detectionCalageBordure();
      
    }
  	switch (checkNewCommandI2C())
  	{
      case NEW_COMMAND_CONTINOUS :
      {
        timeSpentSinceLastNewCommand = 0;
        timeSpentWithoutCommand = 0;
        noCommandTimeOutFlag = 0;
        break;
      }
      case NEW_COMMAND_DIFFERENT :
      {
        timeSpentSinceLastNewCommand = 0;
        timeSpentWithoutCommand = 0;
        noCommandTimeOutFlag = 0;
        planificationAsked = 1;
		distanceTheorique = calculConsigne_d(Robot->target);
        break;
      }
    	case OLD_COMMAND :	
      {
      	timeSpentSinceLastNewCommand ++;
      	timeSpentWithoutCommand = 0;
        noCommandTimeOutFlag = 0;
      	break;
      }
      case NO_COMMAND :
      {
        timeSpentWithoutCommand ++;
        if (timeSpentWithoutCommand >= NO_COMMAND_TIMEOUT)
        {
          timeSpentWithoutCommand = NO_COMMAND_TIMEOUT;
          noCommandTimeOutFlag = 1;
        }
        else
        {
          noCommandTimeOutFlag = 0;
        }
        break;
      }
      case WRONG_COMMAND :
      {
        wrongCommand++;
        break;
      }  
      default : break;
    }  
  	if (TruePositionReceived)
  	{    
      nextTargetPointManagement(ecart_d,planificationAsked);
    }
    planificationAsked = 0;
    fillSendBuffer();
    LATEbits.LATE5 = 0;
  }
  return 0;               //Code never reaches here!
}

/*
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                    ;
;                       Software License Agreement                   ;
;                                                                    ;
;   The software supplied herewith by Microchip Technology           ;
;   Incorporated (the "Company") for its dsPIC controller            ;
;   is intended and supplied to you, the Company's customer,         ;
;   for use solely and exclusively on Microchip dsPIC                ;
;   products. The software is owned by the Company and/or its        ;
;   supplier, and is protected under applicable copyright laws. All  ;
;   rights are reserved. Any use in violation of the foregoing       ;
;   restrictions may subject the user to criminal sanctions under    ;
;   applicable laws, as well as to civil liability for the breach of ;
;   the terms and conditions of this license.                        ;
;                                                                    ;
;   THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION.  NO           ;
;   WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING,    ;
;   BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND    ;
;   FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE     ;
;   COMPANY SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL,  ;
;   INCIDENTAL OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.  ;
;                                                                    ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
*/


