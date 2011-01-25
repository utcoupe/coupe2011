#ifndef MESSAGE_H_
#define MESSAGE_H_

#define SERIAL_BAUD 115200

void initSerialLink();
void readIncomingData();
void analyzeMessage(int bufferIndex, unsigned char* buffer);


#endif /* MESSAGE_H_ */
