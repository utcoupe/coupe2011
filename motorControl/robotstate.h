/*
 * robotstate.h
 *
 *  Created on: 13 janv. 2011
 *      Author: HoHen
 */

typedef struct RobotState {
  double x;
  double y;
  double speed_left;
  double speed_right;
  double speed;
  double angle;
} RobotState;

extern RobotState robot_state;

void initRobotState();
