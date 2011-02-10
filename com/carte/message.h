#ifndef MESSAGE_H_
#define MESSAGE_H_

#define SERIAL_BAUD 115200

#define C_SEP1	"|"
#define C_SEP2	"/"
#define C_SEP3	"_"
#define C_SEND	":"



void initSerialLink();
void readIncomingData();
void sendMessage(int id_msg, char* str);
void sendMessage(int id_msg, int i);
void sendMessage(int id_msg, int *tabi, int size);
void sendMessage(int id_msg, char** tabs, int nbStr, int *tabi, int nbInt);
void sendMessage(int id_msg, int* tabi, int nbInt, char** tabs, int nbStr);

#define IDENTIFICATION	0
#define PING			1

// autre
#define SHARP			2

// asserv
#define GOAL_A			2
#define GOAL_R			3
#define ANGLE_A			4
#define ANGLE_R			5
#define POSITION		6
#define RECALAGE		7

#endif /* MESSAGE_H_ */
