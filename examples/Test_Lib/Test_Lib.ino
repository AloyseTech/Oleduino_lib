#include <SPI.h>
#include <Wire.h>
#include <SD.h>
#include <Time.h>

#include <Oleduino.h>

Oleduino console;
  
void setup() {
  console.init();
  
  //test functionnalities
  console.imu.readAccel();
  console.imu.readGyro();
  console.imu.readMag();
  console.imu.readTemp();
  console.joystick.read();
  console.bme.readTemperature();
  console.batteryMonitor.getRsoc();
  console.rtc.date2str(true);
  
  
}

void loop() {
  // put your main code here, to run repeatedly:
    
}



