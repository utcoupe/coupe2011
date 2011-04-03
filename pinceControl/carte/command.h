#ifndef COMMAND_H_
#define COMMAND_H_

#include "WProgram.h"
#include "message.h"
#include "pinceControl.h"
#include "sensor.h"
#include "tourelle.h"

#define Q_PING 				1
#define Q_IDENT 			0
#define Q_SHARP 			12
#define Q_TRIGGERSHARP 		13
#define Q_MS	 			14
#define Q_TRIGGERMS	 		15
#define Q_PINCE 			16
#define Q_POSITION 			17
#define Q_PRESENT_LOAD 		18
#define Q_JACK		 		19
#define Q_JACK		 		20


void cmd(int,int,int*);

#endif /* COMMAND_H_ */
