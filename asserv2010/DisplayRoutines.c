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
; This file contains a routine to convert hexadecimal A/D conversion
; results into decimal. This file also contains a routine to update
; the image of the LCD display buffer stored in the dsPIC30F device
; RAM.
;
;REVISION HISTORY:
;  $Log: DisplayRoutines.c,v $
;  Revision 1.1.1.1  2005/06/06 09:16:45  VasukiH
;  First release of software
;
;
*/

//Pre-Processor Directives:
#include <p30fxxxx.h>
#include "system.h"
#include "display_routines.h"
//Declaration to Link External Functions & Variables:
extern  int     TempSensor;
extern  int     Potentiometer;

//Functions and Variables with Global Scope:

unsigned char DisplayData[] = "dsPICDEM2  Board\r\nT=+00C Pot=0.00v\r\n\r\n\0";
unsigned char adones=0;
unsigned char adtenths=0;
unsigned char adhundredths=0;

//UpdateDisplayBuffer() Function:
//This function writes new A/D readings to the Display Buffer.
//The display buffer stores a string to be displayed on both the RS232 and
//the LCD.
void UpdateDisplayBuffer(void)
{
        int temp = 0;                   //Convert TC1047A reading to Deg
                                        //Celsius
        temp = TempSensor - 0x66;      //Determine if the temperature is
        if (temp > 0)                   //negative, in order to write
        {                               //either a + or - sign to display areas
                DisplayData[20] = '+';
        }
        else
        {
                temp = 0x66 - TempSensor;
                DisplayData[20] = '-';
        }

        ADCResult2Decimal(temp);        //Convert the hex value to decimal
        DisplayData[21] = adtenths;     //Update the display buffer characters
        DisplayData[22] = adhundredths;

        ADCResult2Decimal(Potentiometer); //Convert the A/D hex value to
        DisplayData[29] = adones;       //decimal and update the display buffer
        DisplayData[31] = adtenths;
        DisplayData[32] = adhundredths;

}

//ADCResult2Decimal() Function:
//This function converts a 10-bit A/D result in hexadecimal to decimal
//assuming a 5 volt reference voltage.
void ADCResult2Decimal(unsigned int ADRES)
{
         adones = 0;            //reset values
         adtenths = 0;
         adhundredths = 0;
         while (ADRES > 0x8)
         {
                if(ADRES > 0xCC)                //test for 1 volt or greater
                {
                        adones++;                       //increment 1 volt counter
                        ADRES -= 0xCD;                  //subtract 1 volt
                }
                else if(ADRES > 0x14 && ADRES <= 0xCC)   //test for 0.1 volt or greater
                {
                        if (adtenths < 9)
                        {
                          adtenths++;                   //increment tenths
                        }
                        else
                        {
                          adones++;                     //tenths has rolled over
                          adtenths = 0;                 //so increment ones and reset tenths
                        }
                        ADRES -=0x15;
                }
                else if(ADRES > 0x2 && ADRES <= 0x14)   //test for 0.01 volt or greater
                {
                        if (adhundredths < 9)
                        {
                                adhundredths++;         //increment hundreths
                        }
                        else
                        {

                                adhundredths = 0;       //reset hundredths
                                if (adtenths < 9)
                                {
                                        adtenths++;                     //and increment tenths
                                }
                                else
                                {
                                        adones++;                       //unless tenths has rolled over
                                        adtenths = 0;                   //so increment ones and reset tenths
                                }
                        }
                        ADRES -= 0x3;
                }



         }
        adones += 0x30;
        adtenths += 0x30;
        adhundredths += 0x30;

}




