#ifndef MESSAGE_H_
#define MESSAGE_H_

#define SERIAL_BAUD 115200

// cf. protocole
#define C_SEP_SEND	'.' // tout est séparé par ça. Ex : 5:0:300:45



void initSerialLink();
void readIncomingData();
void sendMessage(int id, char* str);
void sendMessage(int id, int i);
void sendMessage(int id, int *tabi, int size);
void sendMessage(int id, char** tabs, int nbStr, int *tabi, int nbInt);
void sendMessage(int id, int* tabi, int nbInt, char** tabs, int nbStr);

// all
#define Q_IDENT 			0
#define Q_PING 				1
#define Q_DEBUG				99
#define Q_STOP 				13

// autre
#define Q_IDENT 			0
#define Q_PING 				1
#define Q_SHARP 			31
//#define Q_TSHARP 			32 //TRIGGERSHARP //obselete
#define Q_MS	 			33
#define Q_TMS	 			34 //TRIGGERMS
#define Q_PINCE 			35
#define Q_SETPOSITION 		36 //PINCE POSITION
#define Q_LOAD 				37 //PRESENT_LOAD
#define Q_JACK		 		38
#define Q_LED		 		39
#define Q_PION		 		40
#define Q_RESET		 		45
#define Q_RECALAGE	 		46
#define Q_COLOR				50
/*debug*/
#define Q_MOTOR		 		41
#define Q_TXAX12	 		42 //write
#define Q_RXAX12	 		43 //read
#define Q_CODEUR	 		44 

// asserv
#define Q_GOAL_ABS 			2
#define Q_GOAL_REL 			3
#define Q_ANGLE_ABS 		4
#define Q_ANGLE_REL 		5
#define Q_POSITION 			6
#define Q_AUTO_CALIB		7
#define Q_MANUAL_CALIB		8
#define Q_DELAY 			9
#define Q_PWM 				10
#define Q_MODIF_GOAL_ABS 	11
#define Q_MODIF_GOAL_REL 	12
#define Q_PAUSE				14
#define Q_RESUME			15

// erreurs
#define	E_INVALID_CMD						-1
#define	E_INVALID_PARAMETERS_NUMBERS		-2
#define W_STOP								-42

#endif /* MESSAGE_H_ */
