#ifndef MESSAGE_H_
#define MESSAGE_H_

#define SERIAL_BAUD 9600

void initSerialLink();
void readIncomingData();
int checksum(int size, int* buffer);
void analyzeMessage(int bufferIndex, int* buffer);


#endif /* MESSAGE_H_ */
