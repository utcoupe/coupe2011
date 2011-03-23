#ifndef MESSAGE_H_
#define MESSAGE_H_

#define SERIAL_BAUD 115200

// cf. protocole
// concernant l'envoie
#define C_SEP1	'|' // séparation id|message
#define C_SEP2	'/' // séparation infos
#define C_SEP3	'_' // séparations sous-infos
// concernant la reception
#define C_SEND	':' // tout est séparé par ça. Ex : 5:0:300:45 (id:cmd:parametre1:parametre2)



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
#define SHARP			52
#define CHECK_SHARP		53

// asserv
#define GOAL_A			2
#define GOAL_R			3
#define ANGLE_A			4
#define ANGLE_R			5
#define POSITION		6
#define RECALAGE		7

#endif /* MESSAGE_H_ */
