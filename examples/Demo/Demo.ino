#include <SPI.h>
#include <Wire.h>
#include <SD.h>
#include <Oleduino.h>

Oleduino console;
  
void setup() {
  console.joystick.read();
  console.imu.begin();
  console.display.begin();
  
  
}

void loop() {
  // put your main code here, to run repeatedly:
    
}



