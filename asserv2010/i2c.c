/**
 * @author 
 * code written by Sammy ZIDANE
 * (c) 2007 All Rights Reserved - This file is confidential.
 *
 * Version : $Id$
 */
 
//Pre-Processor Directives:
#include <p30fxxxx.h>

#include "ALTypes.h"
#include "i2c.h"
#include "math.h"
#include "config.h"
#include "config.h"
#include "asservOrientation.h"

#define                   ADDRESS               37
#define                   INPUT_BUFFER_MAX      128
#define                   LOCK_SEND_BUFFER      99
#define                   SEND_MESSAGE_LENGTH 8
//Functions and Variables with Global Scope:
volatile static uInt8     gInputBufferIndex = 0;
uInt8     gIndexByteRead=0;
uInt8     gCommand=0;
volatile static uInt8     gLastAddress = 0;
volatile uInt8     g_byteReceived[INPUT_BUFFER_MAX];
volatile uInt8     g_bufferSend[INPUT_BUFFER_MAX];
volatile uInt8     gDataAvailable;
volatile uInt8    sendBufferLock = 1;

uInt16 TruePositionReceived = 0;
extern float distanceTheorique;


void							  oInitI2C(void)
{
//	LATEbits.LATE2 = 1;

	I2CCONbits.A10M = 0;	 	  // 7 bit address
	I2CCONbits.IPMIEN = 0; 	// IPMI mode not enabled 
	I2CCONbits.GCEN = 1; 		// Broadast
	I2CCONbits.DISSLW = 1;  	// Slew rate control disabled
	I2CCONbits.STREN = 0; 		// Enable software or receive clock stretching
	I2CADD = ADDRESS;        // Address
	I2CCONbits.SCLREL = 1; 	// Release SCL
	I2CCONbits.I2CEN = 1; 		// Enable I2C module
  IPC3bits.SI2CIP = 6;
  gIndexByteRead = 0;
  gDataAvailable = 0;
	IFS0bits.SI2CIF = 0;
	IEC0bits.SI2CIE = 1;
}

void __attribute__((__interrupt__, __no_auto_psv__))_SI2CInterrupt(void)
{
  static uInt8     gNbByteToReceiv=0;
  static uInt8     crc=0;
  // Master read, last byte was an adress
  if ((I2CSTATbits.S) && (I2CSTATbits.R_W) && (!I2CSTATbits.D_A)
      && (I2CSTATbits.RBF))
  {
    // Ask for a reply !
    gIndexByteRead = 0;
    // Read the addess
    gLastAddress = I2CRCV;
    
    // Send the 1st byte
    I2CTRN = g_bufferSend[gIndexByteRead++];
    gIndexByteRead %= INPUT_BUFFER_MAX;
  }
  else
  {
    // Master read, last byte was data
    if ((I2CSTATbits.S) && (I2CSTATbits.R_W) && (!I2CSTATbits.D_A)
        && (!I2CSTATbits.RBF))
    {
      I2CTRN = g_bufferSend[gIndexByteRead++];
      gIndexByteRead %= INPUT_BUFFER_MAX;
      if (gIndexByteRead >= SEND_MESSAGE_LENGTH)
      {
         // if all data have been send release the send buffer lock
         sendBufferLock = 1;
      }  
    }
    else
    {
      // Master write, last byte was an address
      if ((I2CSTATbits.S) && (!I2CSTATbits.R_W) && (!I2CSTATbits.D_A) &&
          (I2CSTATbits.RBF))
      {
        // Read the addess, but do nothing
        gLastAddress = I2CRCV;
        gInputBufferIndex = 128;
        crc=0;
      }
      else
      {
        // Master write, last byte was data
        if ((I2CSTATbits.S) && (!I2CSTATbits.R_W) && (I2CSTATbits.D_A)
            && (I2CSTATbits.RBF))
        {
          gDataAvailable = 0;
          // check if the byte received is the first = message length
          if (gInputBufferIndex == 128)
          {
            gNbByteToReceiv = I2CRCV;
            if (gNbByteToReceiv == LOCK_SEND_BUFFER)
            {
              sendBufferLock = 0;
            }
            else
            {
              sendBufferLock = 1;
            }  
            crc ^= gNbByteToReceiv;
            gInputBufferIndex = 0;

          }
          else
          {
            uInt8 receivedData;
            // receiv buffer must be flush even if there is an error
            receivedData = I2CRCV;
            if (gInputBufferIndex < INPUT_BUFFER_MAX)
            {
              g_byteReceived[gInputBufferIndex] = receivedData ;
              crc ^= receivedData;
              gInputBufferIndex++;
              gNbByteToReceiv--;
              // if whole message received, check if it is correct and signal it
              if (( gNbByteToReceiv == 0) && (  crc == 0))
              {
                gDataAvailable=1;
                gCommand = gInputBufferIndex;
              }  
            }  
          }    
        }
        else
        {
          // Master NACK
          if ((I2CSTATbits.S) && (!I2CSTATbits.R_W) && (I2CSTATbits.D_A)
              && (!I2CSTATbits.RBF))
          {
            // Do nothing ...
          }
        }
      }
    }
  }

  I2CCONbits.SCLREL = 1; // Release SCL in any cases
  
  // Check for overflow
  if( I2CSTATbits.IWCOL)
    I2CSTATbits.IWCOL=0;

  // Check for write collision
  if( I2CSTATbits.I2COV)
    I2CSTATbits.I2COV = 0;

  // Clear the interrupt
  IFS0bits.SI2CIF = 0;
}

void oWaitI2Cdata(void)
{
  while(!gDataAvailable){
  }  
  gDataAvailable = 0;
}  

uInt8 oGetCommand(void)
{
  return g_byteReceived[0];
}  

uInt8 * oGetReceptionBuffer(void)
{
  return g_byteReceived;
}
uInt16 getBufferSize(void)
{
  return gInputBufferIndex;
}

void fillSendBuffer(void)
{
  robot * Robot;
  sInt16 orientation_sInt16;
  sInt16 temp;
  uInt8 status = 0;
  uInt8 crc = 10;
  uInt8 index = 0;
  Robot = getRobot();
  // check if no new point are available
  if ( (Robot->target.x == Robot->nextTarget.x) && (Robot->target.y == Robot->nextTarget.y) )
  {
    status |= 0x80;
  }
  if ( Robot->speedConsigne == 0) 
  {
    status |= 0x40;    
  } 

  if (Robot->rotationMode == NO_PWM )
  {
    status |= 0x20;
  }  
  
  if (sendBufferLock )
  {
    // this operation is locked after a write command 99
    // and release with the begining of a write command or after 8 bytes transmission
    // this whole operation takes 14 us
    // a byte (the address with read flag )is transfered in 25 us on i2c just after the 99 write command
    // in the worst case 11 us is free if no other interruption like input capture or timer interruption happen
    // this should works :p
    temp = (sInt16)toMillimeter(Robot->position.x);
    g_bufferSend[index]=temp >> 8 ;
    crc ^= g_bufferSend[index++];
    g_bufferSend[index]=temp;
    crc ^= g_bufferSend[index++];
    temp = (sInt16)toMillimeter(Robot->position.y);
    g_bufferSend[index]=temp >> 8;
    crc ^= g_bufferSend[index++];
    g_bufferSend[index]= temp;
    crc ^= g_bufferSend[index++];
    orientation_sInt16 = (sInt16)(Robot->orientation * 180.0 /PI);
    g_bufferSend[index]=(orientation_sInt16 >> 8) ;
    crc ^= g_bufferSend[index++];
    g_bufferSend[index]=orientation_sInt16 ;
    crc ^= g_bufferSend[index++];
    g_bufferSend[index]=status;
    crc ^= g_bufferSend[index++];
    g_bufferSend[index]=crc;
  }
}  


#define TRAME_STANDARD 10
#define TRAME_CONFIGURATION 7
uInt8 checkNewCommandI2C(void)
{
  robot * Robot;
  static point target;
  static point nextTarget;
  static point targetLast;
  static point nextTargetLast;
  static float speedMaxLast=0;
  static float speedMax;
  static uInt8 initLast=0;
  
  if (!initLast)
  {
    targetLast.x=0;
    targetLast.y=0;
    nextTargetLast.x=0;
    nextTargetLast.y=0;
    initLast = 1;
  }  
  
  Robot = getRobot();

  if (gDataAvailable)
  {
    LATBbits.LATB1 = !LATBbits.LATB1;
    gDataAvailable = 0;
    switch (gCommand) 
    {
      case TRAME_STANDARD :
      {
        uInt8 newTrame=0;
        //READ COMMAND
        target.x = toCount((float)(((sInt16)g_byteReceived[0]<<(sInt16)8) + g_byteReceived[1]));
        if (target.x != targetLast.x) newTrame = 1;
        target.y = toCount((float)(((sInt16)g_byteReceived[2]<<(sInt16)8) + g_byteReceived[3]));
        if (target.y != targetLast.y) newTrame = 1;
        nextTarget.x = toCount((float)(((sInt16)g_byteReceived[4]<<(sInt16)8) + g_byteReceived[5]));
        if (nextTarget.x != nextTargetLast.x) newTrame = 1;
        nextTarget.y = toCount((float)(((sInt16)g_byteReceived[6]<<(sInt16)8) + g_byteReceived[7]));
        if (nextTarget.y != nextTargetLast.y) newTrame = 1;
        speedMax = (float)((sInt8)g_byteReceived[8]);

        //if (speedMax != speedMaxLast) newTrame = 1;

        // UPDATE LAST COMMAND
        targetLast.x = target.x;
        targetLast.y = target.y;
        nextTargetLast.x = nextTarget.x;
        nextTargetLast.y = nextTarget.y;
        speedMaxLast = speedMax;

        // UPDATE ROBOT PARAMETER IN CASE OF NEWCOMMAND
        if (newTrame)
        {   
          if ( (Robot->target.x != target.x) || (Robot->target.y != target.y) )
          {
            Robot->target.x = target.x;
            Robot->target.y = target.y;
            Robot->nextTarget.x = nextTarget.x;
            Robot->nextTarget.y = nextTarget.y;
            Robot->speedMax = speedMax;
            Robot->speedRotMax = (fabs(Robot->speedMax) * PI)/(Robot->entraxe);
            return NEW_COMMAND_DIFFERENT;
          }
          else
          {
            Robot->target.x = target.x;
            Robot->target.y = target.y;
            Robot->nextTarget.x = nextTarget.x;
            Robot->nextTarget.y = nextTarget.y;
            Robot->speedMax = speedMax;
            Robot->speedRotMax = (fabs(Robot->speedMax) * PI)/(Robot->entraxe);
            return NEW_COMMAND_CONTINOUS;
          }  
        }
        else
        {
          return OLD_COMMAND;
        }  
        break;
      }
      case TRAME_CONFIGURATION :
      {	
        float newOrientation;
        Robot->position.x = toCount((float)(((sInt16)g_byteReceived[0]<<(sInt16)8) + (sInt16)g_byteReceived[1]));
        Robot->position.y = toCount((float)(((sInt16)g_byteReceived[2]<<(sInt16)8) + (sInt16)g_byteReceived[3]));
        Robot->target = Robot->position;
        Robot->nextTarget = Robot->position;
        targetLast = Robot->position;
        nextTargetLast = Robot->position;
        speedMaxLast = Robot->speedMax;
        newOrientation = ((sInt16)g_byteReceived[4]<<((sInt16)8)) + (sInt16)g_byteReceived[5];
        newOrientation *= -PI/180;
        while (newOrientation >  PI) newOrientation -=PIx2;
	      while (newOrientation < -PI) newOrientation +=PIx2;
        Robot->orientation_initiale = newOrientation - ( Robot->orientation - Robot->orientation_initiale ) ;
        IEC0bits.IC1IE = 1;	// Enable interrupts on IC1
	      IEC0bits.IC2IE = 1;	// Enable interrupts on IC2
	      IEC1bits.IC7IE = 1;	// Enable interrupts on IC7
	      IEC1bits.IC8IE = 1;	// Enable interrupts on IC8
	
        TruePositionReceived = 1;
        return NO_COMMAND;
        break;
      }
      default :
      {
        return WRONG_COMMAND;
        break;
      }  
    }  
  }
  else
  {
    return NO_COMMAND;
  }      
}  

 