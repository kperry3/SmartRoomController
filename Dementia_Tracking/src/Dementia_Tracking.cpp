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
#include "Adafruit_SSD1306.h"
#include "Button.h"
#include "IoTTimer.h"
#include "Colors.h"

// Define constants
const int BUTTON1 = D18;
const int BUTTON2 = D17;
const int BUTTON3 = D15;
const int MOTIONPIN = D6;
const int HALLPIN = D7;
const int ULTRASONIC = A0;
const int OLED_RESET= -1;
const int TEXTSIZE = 1;
const int PIXELCOUNT = 8;
const int BRIGHTNESS = 5;

// Declare objects
Adafruit_SSD1306 display(OLED_RESET);
Adafruit_NeoPixel pixel(PIXELCOUNT, SPI1, WS2812B);  
Button onOffButton(BUTTON1);
Button resetButton1(BUTTON2);
Button resetButton2(BUTTON3);
IoTTimer resetTimer;

enum SystemState {
  OFF,
  ON,
  RESETTING,
  SAFE,
  CAUTION,
  WARNING,
  EMERGENCY
};

// Functions
SystemState determineSystemState(SystemState state);
void pixelFill(int startPixel, int endPixel, int hexColor);
void displayNotification(String message);
float getDistance();

// Variables
SystemState systemState = ON;
SystemState previousSystemState = ON;

// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(AUTOMATIC);

void setup() {
  // Initialize serial port
  Serial.begin(9600);
  waitFor(Serial.isConnected, 10000);

  // Motion Sensor
  pinMode(MOTIONPIN, INPUT);

  //Hall Sensor
  pinMode(HALLPIN, INPUT);

  //Ultrasonic Sensor
  pinMode(ULTRASONIC, INPUT);

  // OLED
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  // Neopixels
  pixel.begin();
  pixel.setBrightness(BRIGHTNESS);
  pixel.show(); 
}

void loop() {
  // Read Buttons
  if(onOffButton.isPressed()){
    Serial.printf("Button pressed!!\n\n\n");
    if(systemState != OFF){
     systemState = OFF;
    }
    else {
      systemState = ON;
    }
  }
  else if(resetButton1.isPressed() || resetButton2.isPressed()){
    resetTimer.startTimer(30000);  // 30 seconds should be enough time for a person to get through the door
    systemState = RESETTING;  
  }

  systemState = determineSystemState(systemState);

}

SystemState determineSystemState (SystemState state){
  float distance;
  int hallValue, motionValue;

  switch(state){
    case OFF:
      pixelFill(0, PIXELCOUNT, blue);
      displayNotification("System Off");
    break;
    case ON:  //don't think i need this state!!
      pixelFill(0, PIXELCOUNT, green); 
      displayNotification("System On");
      distance = getDistance();
      if((distance > 3) || (distance < 0)){
        state = SAFE;
      }
      else if((distance > 0 && distance <= 3)) {
        state = CAUTION;
      }
    break;
    case SAFE:
      pixelFill(0, PIXELCOUNT, green); 
       displayNotification("Safe - Person Safe");

      motionValue = digitalRead(MOTIONPIN);
      Serial.printf("Motion value: %d\n\n", motionValue);

      distance = getDistance();
      if((distance > 3) || (distance < 0)){
        state = SAFE;
      }
      else if((distance > 0 && distance <= 3)) {
        state = CAUTION;
      }
    break;
    case RESETTING:
      pixel.clear();  // Want to flash
      pixelFill(0, PIXELCOUNT, purple); 
      displayNotification("System Resetting....");
     if(resetTimer.isTimerReady()){
      state = ON;
      }
    break;
    case CAUTION:
    pixel.clear(); // want to flash
      pixelFill(0, PIXELCOUNT, yellow); 
      displayNotification("Caution - Person Near Door");
      // Read hall sensor
      // if hall sensor is 0 meaning the door knob is being turned and we get a distance then set state to warning
      distance = getDistance();
      hallValue = digitalRead(HALLPIN);
      if((distance > 0 && distance <= 3) && hallValue == LOW){ // CHANGE BACK TO 36
        state = WARNING;
      }
      else if((distance > 3) || (distance < 0)){
        state = SAFE;
      }
      
    break;
    case WARNING:
      pixelFill(0, PIXELCOUNT, orange); 
      displayNotification("Warning - Person Turning Door Knob");
      // If we are in warning that means we turned the knob so see if person is going out the door or could have moved back away from the door so check distance also if motion is 0
      // Read pir motion sensor if 1 set to emergency
      // if distance sensor <= 3 feet and motion 0 caution > 3 feet safe  Warning - Person Turning Door Knob
      distance = getDistance();
      motionValue = digitalRead(MOTIONPIN);
      hallValue = digitalRead(HALLPIN);
      Serial.printf("In warning state ----- Motion value: %d\n\n", motionValue);
      if(motionValue == HIGH){
        state = EMERGENCY;
      }
      else if (hallValue == HIGH && (distance > 0 && distance <= 3)){
        state = CAUTION;
      }
      else if((distance < 0 || distance > 3)){ //CHANGE BACK TO 36
        state = SAFE;
      }

    break;
    case EMERGENCY:
      pixelFill(0, PIXELCOUNT, red); 
      displayNotification("Emergency - Person Opened Door");
      // Read hall and distance if person is on the other side of the door there should be no distance stay in emergency
      // if we get a hall reading and a distance reading warning if just a distance <= 3ft caution if just distance > 3ft safe
      distance = getDistance();
      hallValue = digitalRead(HALLPIN);
      if(hallValue == HIGH && (distance > 0 && distance <= 3)){  //CHANGE BACK TO 36 INCHES
        state = CAUTION;
      }
      else if(hallValue == LOW && (distance > 0 && distance <= 3)){ //CHANGE BACK TO 36
        state = WARNING;
      }
    break;
    default:
     state = OFF;
    break;
  }

  return state; 
}

void displayNotification(String message) {
 // Serial.printf("Message: %s\n", message.c_str());
  display.clearDisplay();
  display.setTextSize(TEXTSIZE);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.printf("%s\n", message.c_str());
  display.display();
  //delay(2000);
}
 

// Lights up a segment of the pixel strip
void pixelFill(int startPixel, int endPixel, int hexColor){
  for(int i = startPixel; i < endPixel; i++){
      pixel.setPixelColor(i, hexColor);
      pixel.show();
  }
}

 // Reads the ultrasonic sensor and converts the value into inches
float getDistance(){
  float value = analogRead(ULTRASONIC);
  float multiplier = ((1253.33*12)/(2*1000000.0)); // Convert value read to inches
   // Serial.printf("analog read value is: %f\n\n", value);
  value = value*multiplier;
 //Serial.printf("DIstance is: %f\n\n", value);
  return value;
 }