/**
 * @author joseph pinkasfeld
 *  (c) 2007 All Rights Reserved - This file is confidential.
 *
 * Version : $Id$
 */

/** 
 * Ram configuration data.
 * Main struct with configuration used in the program.
 * This struct is directly readable/writable by the Mother Board
 */ 
#include "ALTypes.h"
#include "config.h"
#include "math.h"
#include "asservOrientation.h"
/** 
 * Main struct with data
 */ 
#define NB_JOINTS 2
JointConfig joints[NB_JOINTS]; // 2 joints

JointConfig * oGetJointConfig(uInt16 pJoint)
{
    return &(joints[pJoint]);
}  

/** 
 * Initialise the configuration data
 */ 
void oInitConfig(void)
{
    //joints[0].motor.reductionRateMotor = 201.3;
    joints[0].motor.InvertControlDirection = 0;
    joints[0].motor.motorPosition = 1;
    joints[0].sensors.position = 2;
    joints[0].position = 0;
    joints[0].lastPosition = 0;
    joints[0].speed = 0;
        
    //joints[1].motor.reductionRateMotor = 201.3;
    joints[1].motor.InvertControlDirection = 0;
    joints[1].motor.motorPosition = 2;
    joints[1].sensors.position = 2;
    joints[1].position = 0;
    joints[1].lastPosition = 0;
    joints[1].speed = 0;
}

robot ROBOT;
robot RobotLast;

void updateLastRobot(void)
{
  RobotLast = ROBOT;
}

void updateJoints(void)
{
  joints[0].lastPosition = joints[0].position;
  joints[1].lastPosition = joints[1].position;  
}
robot * getRobot(void)
{
  return &ROBOT;
} 

robot * getRobotLast(void)
{
  return &ROBOT;
} 

void initRobot(void)
{
  ROBOT.position.x=0;
  ROBOT.position.y=0;
  ROBOT.tx=0;
  ROBOT.ty=0;
  ROBOT.speed=0;
//  ROBOT.acceleration=0.032;
  ROBOT.acceleration=0.082;
  ROBOT.decceleration=-0.084;
  ROBOT.orientation=0.0;
  ROBOT.orientation_initiale= 0.0;
  ROBOT.entraxe = 5510; // 5680 // was in 2008 : 7049.3
  ROBOT.target = ROBOT.position;
  ROBOT.nextTarget = ROBOT.target;
  ROBOT.speedConsigne = 0;
  ROBOT.speedMax = 20;
  ROBOT.speedConsigneRot = 0;
  ROBOT.speedRotMax = (ROBOT.speedMax * PI)/(ROBOT.entraxe);
  ROBOT.accelerationRot = (ROBOT.acceleration * PI )/( ROBOT.entraxe);
  //ROBOT.speedConsigneRot = 0.009; 
  ROBOT.rotationMode = ROTATION_IN_PLACE;
  updateLastRobot();
}

asserv PID;

void initPid(void)
{
  PID.Kp_d = 64;
  PID.Ki_d = 0;
  PID.Kd_d = 30;  
  PID.Kp_o = 131000;
  PID.Ki_o = 0; 
  PID.Kd_o = 32;
}  

asserv * getPID(void)
{
  return &PID;
}


float toMillimeter(float pDistance)
{
  return pDistance/COUNT_MILLIMETER;
}

float toCount(float pDistance)
{
  return pDistance*COUNT_MILLIMETER;
}
