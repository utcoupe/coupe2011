/*
 * robotstate.cpp
 *
 *  Created on: 13 janv. 2011
 *      Author: HoHen
 */

#include "robotstate.h"

RobotState robot_state;

void initRobotState(){
	robot_state.x = 0;
	robot_state.y = 0;
	robot_state.speed = 0;
	robot_state.angle = 0;
}

