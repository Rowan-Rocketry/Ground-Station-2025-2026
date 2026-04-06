#include <SoftwareSerial.h>

// Pins: RX = 10, TX = 11
// The 'true' at the end enables Inverse Logic (Inverted TTL)
SoftwareSerial mySerial(10, 11, true); 

void setup() {
  // Communication with your computer (keep Serial Monitor at 9600)
  Serial.begin(9600); 
  
  // Communication with the StepperOnline Driver
  // Common rates: 38400 or 57600
  mySerial.begin(57600); 
  
  Serial.println("--- Bridge Mode (Inverted Logic) ---");
  Serial.println("Arduino Pin 10 (RX) -> Driver TX");
  Serial.println("Arduino Pin 11 (TX) -> Driver RX");
  Serial.println("Ready...");
}

void loop() {
  // Listen for data from the Driver, send to Computer
  if (mySerial.available()) {
    Serial.write(mySerial.read());
  }
  
  // Listen for data from the Computer, send to Driver
  if (Serial.available()) {
    mySerial.write(Serial.read());
  }
}

