/* 
 * Project myProject
 * Author: Your Name
 * Date: 
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 */

// Include Particle Device OS APIs
#include "Particle.h"
#include "neopixel.h"

// Define constants
const int BUTTON1 = D18;
const int BUTTON2 = D17;
const int BUTTON3 = D15;
const int MOTIONPIN = D6;
const int HALLPIN = D7;
const int OUTPIN = A1;
const int RANGE = A2;

// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(AUTOMATIC);


void setup() {
  // Initialize serial port
  Serial.begin(9600);
  waitFor(Serial.isConnected, 10000);

  // Buttons
  pinMode(BUTTON1, INPUT);
  pinMode(BUTTON2, INPUT);
  pinMode(BUTTON3, INPUT);

  // Motion Sensor
  pinMode(MOTIONPIN, INPUT);

  //Hall Sensor
  pinMode(HALLPIN, INPUT);

  //Ultrasonic Sensor
  pinMode(OUTPIN, INPUT);
  pinMode(RANGE, INPUT);
  

}

void loop() {
/*   Serial.printf("Button1: %ld\n", digitalRead(BUTTON1));
  delay(5000);
 Serial.printf("Button2: %ld\n", digitalRead(BUTTON2));
  delay(5000);
 Serial.printf("Button3: %ld\n\n\n", digitalRead(BUTTON3));
  delay(5000);
   Serial.printf("MOTION: %ld\n\n", digitalRead(MOTIONPIN));
  delay(5000);
  Serial.printf("HALL: %ld\n\n", digitalRead(HALLPIN));
  delay(5000);
 */
  // Send out pulse
  // digitalWrite(OUTPIN, LOW);
  // delayMicroseconds(2); // Give it time to go low
  // digitalWrite(OUTPIN, HIGH);
  // delayMicroseconds(20); // Give it time to go high
  // digitalWrite(OUTPIN, LOW); 

   //Listen for return pulse
     // Want to know how long it pulse is high
   Serial.printf("Range value: %f\n\n", analogRead(OUTPIN));


}
