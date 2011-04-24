#ifndef COMMAND_H_
#define COMMAND_H_

#include "WProgram.h"
#include "message.h"
#include "pinceControl.h"
#include "sensor.h"
#include "tourelle.h"

#define Q_IDENT 			0
#define Q_PING 				1
#define Q_SHARP 			31
#define Q_TSHARP 			32 //TRIGGERSHARP
#define Q_MS	 			33
#define Q_TMS	 			34 //TRIGGERMS
#define Q_PINCE 			35
#define Q_PPOSITION 		36 //PINCE POSITION
#define Q_LOAD 				37 //PRESENT_LOAD
#define Q_JACK		 		38
#define Q_LED		 		39
#define Q_PION		 		40
/*debug*/
#define Q_MOTOR		 		41
#define Q_TXAX12	 		42 //write
#define Q_RXAX12	 		43 //read
#define Q_CODEUR	 		44 
#define Q_RESET		 		45
#define Q_RECALAGE	 		46

void cmd(int,int,int*,int); // from, id_cmd, *args, sizeArgs

#endif /* COMMAND_H_ */
