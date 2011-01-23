/*
 * robotstate.h
 *
 *  Created on: 13 janv. 2011
 *      Author: HoHen
 */

typedef struct RobotState {
  long x;
  long y;
  long speed_left;
  long speed_right;
  long speed;
  double angle;
} RobotState;

extern RobotState robot_state;

void initRobotState();
