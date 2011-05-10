#include <Servo.h> 

Servo myservo;  // create servo object to control a servo 
Servo myservo2;
 
//int potpin = 0;  // analog pin used to connect the potentiometer
int val;    // variable to read the value from the analog pin 
int a=90,b=90;
void setup() 
{
	Serial.begin(9600);
	myservo.attach(12);  // attaches the servo on pin 9 to the servo object 
	myservo2.attach(13);  // attaches the servo on pin 9 to the servo object 
} 
 
void loop()
{ 
	if (Serial.available() > 0) {
		int incomingByte = Serial.read();
		switch (incomingByte)
		{
			case 'a':
				a+=10;                  // sets the servo position according to the scaled value
				Serial.println(a);
				Serial.println(b);
				break;

			case 'q':
				Serial.println(a);
				Serial.println(b);
				a-=10;
				break;

			case 'z':
				b+=10;
				Serial.println(a);
				Serial.println(b);
				break;

			case 's':
				b-=10;
				Serial.println(a);
				Serial.println(b);
				break;

			default:
				break;
		}
	}
	
	myservo.write(a);
	myservo2.write(b);
}
