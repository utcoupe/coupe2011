/*
; © 2005  Microchip Technology Inc.
;
; Microchip Technology Inc. (“Microchip”) licenses this software to you
; solely for use with Microchip dsPIC® digital signal controller
; products.  The software is owned by Microchip and is protected under
; applicable copyright laws.  All rights reserved.
;
; SOFTWARE IS PROVIDED “AS IS.”  MICROCHIP EXPRESSLY DISCLAIMS ANY
; WARRANTY OF ANY KIND, WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT
; LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
; PARTICULAR PURPOSE, OR NON-INFRINGEMENT. IN NO EVENT SHALL MICROCHIP
; BE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR CONSEQUENTIAL
; DAMAGES, LOST PROFITS OR LOST DATA, HARM TO YOUR EQUIPMENT, COST OF
; PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR SERVICES, ANY CLAIMS
; BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF),
; ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION, OR OTHER SIMILAR COSTS.
;
;
;FILE DESCRIPTION:
; This file contains set up and interrupt service routines for
; I/O ports and External Interrupt pins connected to the switches and
; LEDs provided on the dsPICDEM2 development board.
;
;REVISION HISTORY:
;  $Log: INTx_IO_pins.c,v $
;  Revision 1.1.1.1  2005/06/06 09:16:45  VasukiH
;  First release of software
;
;
*/

//Pre-Processor Directives:
#include <p30fxxxx.h>
#include "system.h"
#include "intx_io_pins.h"

//Functions and Variables with Global Scope:


//Functions:
//INTx_IO_Init() sets up the INT0 and INT1 pins connected to the
//switches S5 and S6, on the dsPICDEM2 board. It also sets port
//pins, RB0 and RB1, as output pins to drive LEDs, D3 and D4.
void INTx_IO_Init(void)
{
/*      //Set up Switches S5 and S6 as external interrupt input pins
        //S5 connected to INT0
        //S6 connected to INT1
        INTCON2 = 0x0003;       //Setup INT0-1 pins to interupt on falling edge
        IFS0bits.INT0IF = 0;    //Reset INT0 interrupt flag
        IEC0bits.INT0IE = 1;    //Enable INT0 Interrupt Service Routine
        IFS1bits.INT1IF = 0;    //Reset INT1 interrupt flag
        IEC1bits.INT1IE = 1;    //Enable INT1 Interrupt Service Routine
*/
        //Set up port pins RB0 and RB1 to drive the LEDs, D3 and D4
        //RB0 connected to D3
        //RB1 connected to D4
       
        LATBbits.LATB0 = 0;     //Clear Latch bit for RB0 port pin
        TRISBbits.TRISB0 = 0;   //Set the RB0 pin direction to be an output
        LATBbits.LATB1 = 0;     //Clear Latch bit for RB1 port pin
        TRISBbits.TRISB1 = 0;   //Set the RB1 pin direction to be an output

}

//_INT0Interrupt() is the INT0 interrupt service routine (ISR).
//The routine must have global scope in order to be an ISR.
//The ISR name is chosen from the device linker script.
//On every INT0 interrupt event, the rate at which the LEDs blink is changed
//by varying the prescaler on Timer1 between the states 1:1, 1:8, 1:64 & 1:256
/*
void __attribute__((__interrupt__, __no_auto_psv__)) _INT0Interrupt(void)
{
        while (Switch_S5 == 1); //Wait in ISR until Switch S5 is released

                                //Change Timer1 Prescaler to next value
                                //in the sequence 1:1, 1:8, 1:64, 1:256
        if (T1CONbits.TCKPS == 3) T1CONbits.TCKPS = 0;
        else T1CONbits.TCKPS +=1;

        IFS0bits.INT0IF = 0;    //Clear the INT0 interrupt flag or else
                                //the CPU will keep vectoring back to the ISR
}
*/
/*
//_INT1Interrupt() is the INT1 interrupt service routine (ISR).
//The routine must have global scope in order to be an ISR.
//The ISR name is chosen from the device linker script.
//On every other INT1 interrupt event, the state of LED D3 is flipped so that
//the two LEDs are NOT ON simultaneously. On every other INT1 interrupt event,
//the state of LED D3 is flipped back so that the two LEDs are ON simultaneously.
void __attribute__((__interrupt__, __no_auto_psv__)) _INT1Interrupt(void)
{
        while (Switch_S6 == 1); //Wait in ISR until Switch S5 is released
        LATBbits.LATB0 =~ LATBbits.LATB0; //Toggle LED D3

        IFS1bits.INT1IF = 0;    //Clear the INT0 interrupt flag or else
                                //the CPU will keep vectoring back to the ISR
}
*/
