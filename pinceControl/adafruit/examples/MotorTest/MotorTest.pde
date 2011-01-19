// Adafruit Motor shield library
// copyright Adafruit Industries LLC, 2009
// this code is public domain, enjoy!

#include <AFMotor.h>

AF_DCMotor motorG(1, MOTOR12_64KHZ);
AF_DCMotor motorD(2, MOTOR12_64KHZ);

void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("Motor test!");

  // turn on motor
  AF_DCMotor motorG(1, MOTOR12_64KHZ);
  motorG.setSpeed(25);
 
  AF_DCMotor motorD(2, MOTOR12_64KHZ);
  motorD.setSpeed(25);

}

void loop() {
  motorG.run(FORWARD);
  motorD.run(BACKWARD);
  delay(2000);
  motorG.run(BACKWARD);
  motorD.run(FORWARD);
  delay(2000);
/*  uint8_t i;
  
  Serial.print("tick");
  
  motor.run(FORWARD);
  for (i=0; i<255; i++) {
    motor.setSpeed(i);  
    delay(10);
 }
 
  for (i=255; i!=0; i--) {
    motor.setSpeed(i);  
    delay(10);
 }
  
  Serial.print("tock");

  motor.run(BACKWARD);
  for (i=0; i<255; i++) {
    motor.setSpeed(i);  
    delay(10);
 }
 
  for (i=255; i!=0; i--) {
    motor.setSpeed(i);  
    delay(10);
 }
  

  Serial.print("tech");
  motor.run(RELEASE);
  delay(1000);*/
}
