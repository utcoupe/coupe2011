#ifndef MESSAGE_H_
#define MESSAGE_H_

#define SERIAL_BAUD 115200

void initSerialLink();
void readIncomingData();
//void analyzeMessage(int bufferIndex, unsigned char* buffer);
void sendMessage(unsigned char c, char* str);
void sendMessage(char cmd, int i);
void sendMessage(char cmd, int *tabi, int size);
void sendMessage(unsigned char cmd, char** tabs, int nbStr, int *tabi, int nbInt);
void sendMessage(unsigned char cmd, int* tabi, int nbInt, char** tabs, int nbStr);

#endif /* MESSAGE_H_ */
