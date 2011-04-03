#ifndef MESSAGE_H_
#define MESSAGE_H_

#define SERIAL_BAUD 115200

// cf. protocole
#define C_SEP_SEND	':' // tout est séparé par ça. Ex : 5:0:300:45



void initSerialLink();
void readIncomingData();
void sendMessage(int id_msg, char* str);
void sendMessage(int id_msg, int i);
void sendMessage(int id_msg, int *tabi, int size);
void sendMessage(int id_msg, char** tabs, int nbStr, int *tabi, int nbInt);
void sendMessage(int id_msg, int* tabi, int nbInt, char** tabs, int nbStr);

// all
#define Q_IDENT 			0
#define Q_PING 				1
#define Q_DEBUG				99

// autre
#define SHARP			52
#define CHECK_SHARP		53

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
#define Q_STOP 				13
#define Q_PAUSE				14
#define Q_RESUME			15

// erreurs
#define	INVALID_CMD						-1
#define	INVALID_PARAMETERS_NUMBERS		-2
#define	INVALID_PARAMETERS_TYPE			-3
#define	INVALID_ANALOGPIN				-4
#define	INVALID_DIGITALPIN				-5

#endif /* MESSAGE_H_ */
