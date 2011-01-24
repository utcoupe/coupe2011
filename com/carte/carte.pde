/*
  Petit test !
 */

void cmd(int name);
inline void identification();	// 0
inline void getSharp();			// 1
inline void ledOn();			// 2
inline void ledOff();			// 3
inline void indexError();		// -1

void setup()
{
 	Serial.begin(9600);
	pinMode(12, OUTPUT); // Rouge   
	pinMode(13, OUTPUT); // Vert
}

void loop()
{
  	if (Serial.available() > 0)
  	{
  		int inByte = Serial.read();
		//Serial.println(inByte);
  		cmd(inByte);
  	}
}

void cmd(int name)
{
	switch (name)
	{
		case 0: // identification
			identification();
			break;
		case 1: // sharp
			getSharp();
			break;
		case 2: //ledOn
			ledOn();
			break;
		case 3:
			ledOff();
			break;
		default:
			indexError();
			break;
	}
}

void identification()
{
	Serial.println(3);
}

void getSharp()
{
  	int sensorValue = analogRead(A0);
	Serial.println(sensorValue);
}

void indexError()
{
	Serial.println(-1);
}

void ledOn()
{
	digitalWrite(12, HIGH);
}

void ledOff()
{
	digitalWrite(12, LOW);
}




