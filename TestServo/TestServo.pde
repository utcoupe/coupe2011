#include <Servo.h> 
 
Servo myservo;  // create servo object to control a servo 
Servo myservo2; 
 
//int potpin = 0;  // analog pin used to connect the potentiometer
int val;    // variable to read the value from the analog pin 
int a=90,b=90;
void setup() 
{
	Serial.begin(9600);
	myservo.attach(9);  // attaches the servo on pin 9 to the servo object 
	myservo2.attach(10);  // attaches the servo on pin 9 to the servo object 
} 
 
void loop() 
{ 
	/*if (Serial.available() > 0) {
		//int incomingByte = Serial.read();
		if(incomingByte=='a'){
					a=90;                  // sets the servo position according to the scaled value 
					b=90;                  // sets the servo position according to the scaled value 
		}if(incomingByte=='b'){
					a=80;                  // sets the servo position according to the scaled value 
					b=100;                  // sets the servo position according to the scaled value 
		}
	}
	myservo.write(a);
	myservo2.write(b); */
	myservo.write(90);
	myservo2.write(90);
	delay(1000);
	myservo.write(70);
	myservo2.write(120);
	delay(1000);
}
