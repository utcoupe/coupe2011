#ifndef MESSAGE_H_
#define MESSAGE_H_

#define SERIAL_BAUD 115200

void initSerialLink();

void readIncomingData();

void sendMessage(int,int);
void sendMessage(int,char*);
void sendMessage(int,int,int*);

#endif /* MESSAGE_H_ */
