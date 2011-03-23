
void cmd(int c, int* message);
void error(int id);
int getSharp(int id);			// 1
void ledOn();			// 2
void ledOff();			// 3
void surveiller(int id_msg, char id_sharp);
void indexError();		// -1
void init_sharps();
void check_sharps();


/*
typedef int (*aa)();

void cmd(int name);
int identification()
{
	Serial.println(3);
}

int getSharp()
{
  	int sensorValue = analogRead(A0);
	Serial.println(sensorValue);
}
//int identification();	// 0
//int getSharp();			// 1
inline int ledOn();			// 2
inline int ledOff();			// 3
inline int indexError();		// -1

aa tab [] = { //Etrange hein?
    identification,
    getSharp
};
*/
