/**
 * @author Joseph pinkasfeld
 *
 * Version : $Id$
 */

/** 
 * Ram configuration data.
 * Main struct with configuration used in the program.
 * This struct is directly readable/writable by the Mother Board
 */ 


#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "ALTypes.h"
#include "trajectory.h"

#define DROITE 0
#define GAUCHE 1

#define CONFIG_SENSOR_TYPE_NONE 0 // No sensors
#define CONFIG_SENSOR_TYPE_ONE_MRE 1 // One MRE on the joint only, 
#define CONFIG_SENSOR_TYPE_TWO_MRE 2 // Two MRE, one on the joint, one on the motor
#define CONFIG_SENSOR_TYPE_POTAR_1 3 // potentionmeter arm type 1 
#define CONFIG_SENSOR_TYPE_POTAR_2 4 // potentionmeter arm type 2 

#define COUNT_MILLIMETER 17.86 // 21.25


/**
 * Joint configuration 
 * Sensor anc actuator
 * Separate struct because there is 2 of them ...
 */
typedef struct JointConfig {
  

    struct {
      sInt8 position;
    } sensors;

    struct {
      uInt8 motorPosition; // 1 or 2
      uInt8 InvertControlDirection ;
      float reductionRateMotor;
    } motor;
    
    sInt32 position;
    sInt32 lastPosition;
    sInt32 speed;

} JointConfig;


void updateJoints(void);

typedef struct robot {
  point position;
  float tx;
  float ty;  
  float speed;
  float acceleration;
  float accelerationRot;
  float decceleration;
  float orientation;
  float orientation_initiale;
  sInt16 rotationMode;
  float speedConsigne;
  float speedConsigneRot;
  float speedRot;
  float speedRotMax;
  float speedMax;
  float entraxe;
  point target;
  point nextTarget;
} robot; 
 
typedef struct asserv{
      sInt32 Kp_d; // Proportional term to reduce the error in motor control
      sInt32 Ki_d; // Integral term to reduce the error in motor control
      sInt32 Kd_d; // Derivative term to reduce the error in motor control
      sInt32 iMax_d; // Maximum value of the integral
      sInt32 Kp_o; // Proportional term to reduce the error in motor control
      sInt32 Ki_o; // Integral term to reduce the error in motor control
      sInt32 Kd_o; // Derivative term to reduce the error in motor control
      sInt32 iMax_o; // Maximum value of the integral
} asserv;

void updateLastRobot(void);

void initPid(void);

asserv * getPID(void);

/** 
 * Get main struct with data.
 *
 * @return the struct configuration
 */ 
 JointConfig * oGetJointConfig(uInt16 pJoint);

// Remplace by gSendData.ramMainConfig
//mainConfig* oGetMainConfig(void);

/** 
 * Initialise the configuration data
 */ 
void oInitConfig(void);
void initRobot(void);
robot * getRobot(void);
robot * getRobotLast(void);
float toMillimeter(float pDistance);
float toCount(float pDistance);

#endif
