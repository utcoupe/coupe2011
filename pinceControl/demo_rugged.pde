#include <Stepper.h>
/* This Rugged Motor Shield application demonstrates keyboard control
 * of a stepper motor. Assumptions:
 *
 *   - stepper motor connected to the 2 phases
 *   - 8V-30V supply connected to Vin (optional, else Arduino Vin is used)
 *   - Vin jumper is cut (J21) (only required if Vin>15V)
 *   - FAULT1 and FAULT2 jumpers (J10/J14) installed (optional)
 *   - LED jumpers (J15, J16, J6, J7) installed (optional)
 *
 * The behavior is as follows:
 *
 *   - The 'f' key sets forward motion
 *   - The 'b' key sets backwards motion
 *   - The 'a' key accelerates rotational speed
 *   - The 'd' key decelerates rotation speed
 *   - The 'o' key increases motor power (PWM higher duty cycle)
 *   - The 'm' key decreases motor power (PWM lower duty cycle)
 * 
 * This software is licensed under the GNU General Public License (GPL) Version
 * 3 or later. This license is described at
 * http://www.gnu.org/licenses/gpl.html
 *
 * Application Version 1.0 -- October 2010 Rugged Circuits LLC
 * http://www.ruggedcircuits.com
 */
 
// Define how many steps there are in 1 revolution of your motor
#define STEPS_PER_REVOLUTION 200

/* Define which board you are using. Only uncomment one of the
   following #define's. Or comment them all and let this file 
   guess the board based on the processor:
      ATmega328P --> Duemilanove/Uno
      ATmega324P --> Gator
      ATmega1280 --> Mega (also ATmega2560)
 */
//#define BOARD  0   /* Arduino Duemilanove/Uno (ATmega328P) */
#define BOARD  1   /* Arduino Mega (ATmega1280) */
//#define BOARD  2   /* Rugged Circuits Gator (ATmega324P) */

/**********************************************************/
/* YOU SHOULDN'T HAVE TO CHANGE ANYTHING BELOW THIS POINT */
/**********************************************************/

// In case no board is defined, guess from the processor
#ifndef BOARD
#  if defined(__AVR_ATmega328P__)
#    define BOARD 0
#  elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
#    define BOARD 1
#  elif defined(__AVR_ATmega324P__)
#    define BOARD 2
#  else
#    error You must define BOARD near the top of this file
#  endif
#endif

#if (BOARD!=0) && (BOARD!=1) && (BOARD!=2)
#error Unknown board
#endif

// Enable (PWM) outputs
#define EN1_PIN 3
#define EN2_PIN 11

// Direction outputs
#define DIR1_PIN 12
#define DIR2_PIN 13

// Fault inputs, active low
#define FAULT1_PIN 5
#define FAULT2_PIN 8

// General-purpose LED's, active high
#define LED0_PIN 9    // For motor A
#define LED1_PIN 10   // For motor A
#define LED2_PIN 16   // For motor B
#define LED3_PIN 17   // For motor B

Stepper stepper(STEPS_PER_REVOLUTION, DIR1_PIN, DIR2_PIN);

// Set initial default values
unsigned RPM = 10;
unsigned PWM = 25;
unsigned DIR = 1;

void setup()
{
  // Configure all outputs off for now
  pinMode(EN1_PIN, OUTPUT); digitalWrite(EN1_PIN, LOW);
  pinMode(EN2_PIN, OUTPUT); digitalWrite(EN2_PIN, LOW);
  pinMode(DIR1_PIN, OUTPUT); digitalWrite(DIR1_PIN, LOW);
  pinMode(DIR2_PIN, OUTPUT); digitalWrite(DIR2_PIN, LOW);

  pinMode(LED0_PIN, OUTPUT); digitalWrite(LED0_PIN, LOW);
  pinMode(LED1_PIN, OUTPUT); digitalWrite(LED1_PIN, LOW);
  pinMode(LED2_PIN, OUTPUT); digitalWrite(LED2_PIN, LOW);
  pinMode(LED3_PIN, OUTPUT); digitalWrite(LED3_PIN, LOW);

  // Configure fault inputs with pullups
  pinMode(FAULT1_PIN, INPUT); digitalWrite(FAULT1_PIN, HIGH);
  pinMode(FAULT2_PIN, INPUT); digitalWrite(FAULT2_PIN, HIGH);

  Serial.begin(9600); // only necessary for debugging

  // Change from divide-by-64 prescale on Timer 2 to divide by 8 to get
  // 8-times faster PWM frequency (976 Hz --> 7.8 kHz). This should prevent
  // overcurrent conditions for steppers with high voltages and low inductance.
  TCCR2B = _BV(CS21);

  // Now enable PWM and start motion
  analogWrite(EN1_PIN, PWM);
  analogWrite(EN2_PIN, PWM);
  stepper.setSpeed(RPM);

}

void printHelp()
{
  Serial.println(
"f - forward motion\n"
"b - backwards motion\n"
"a - faster\n"
"d - slower\n"
"o - more power\n"
"m - less power\n"
);
}

void loop()
{
  if (Serial.available()) {
    switch (Serial.read()) {
      case 'f': case 'F': DIR = 1; break;
      case 'b': case 'B': DIR = -1; break;
      case 'o': case 'O':
                          if (PWM <= 250) PWM += 5;
                          Serial.print("PWM="); Serial.println(PWM);
                          analogWrite(EN1_PIN, PWM); analogWrite(EN2_PIN, PWM);
                          break;
      case 'm': case 'M':
                          if (PWM >= 5) PWM -= 5;
                          Serial.print("PWM="); Serial.println(PWM);
                          analogWrite(EN1_PIN, PWM); analogWrite(EN2_PIN, PWM);
                          break;
      case 'a': case 'A':
                          RPM++;
                          Serial.print("RPM="); Serial.println(RPM);
                          stepper.setSpeed(RPM);
                          break;
      case 'd': case 'D':
                          if (RPM > 1) RPM--;
                          Serial.print("RPM="); Serial.println(RPM);
                          stepper.setSpeed(RPM);
                          break;
      case 'h': case 'H':
                          printHelp();
                          break;
      default:
                          Serial.println("Unknown command");
                          break;
    }
  }

  // This is a busy-wait loop until the inter-step time passes
  stepper.step(DIR);

  // Mirror phase outputs in LED's
  digitalWrite(LED1_PIN, digitalRead(DIR1_PIN));
  digitalWrite(LED3_PIN, digitalRead(DIR2_PIN));

  // Drive fault output LED's as mirror of fault inputs
  digitalWrite(LED0_PIN, !digitalRead(FAULT1_PIN));
  digitalWrite(LED2_PIN, !digitalRead(FAULT2_PIN));
}