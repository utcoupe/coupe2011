
void cmd(int c, float* message, int m);
void identification();	// 0
void getSharp();			// 1
void ledOn();			// 2
void ledOff();			// 3
void indexError();		// -1

extern int value_pwm_right;
extern int value_pwm_left;

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