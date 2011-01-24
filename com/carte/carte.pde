
/*
  Petit test !
 */



void setup() {
 	Serial.begin(115200); 
	pinMode(12, OUTPUT); // Rouge   
	pinMode(13, OUTPUT); // Vert
}

void loop() {
  	int sensorValue = analogRead(A0);
  	if (Serial.available() > 0)
  	{
  		int inByte = Serial.read();
  		Serial.println(inByte);
  		//Serial.println(sensorValue, DEC);
  	}
}
