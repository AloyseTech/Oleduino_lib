//
//  BatteryMonitor.h
//  
//
//  Created by Théo Meyer on 05/09/2015.
//
//

#ifndef _BatteryMonitor_h
#define _BatteryMonitor_h

#include "Arduino.h"
#include <Wire.h>
#include "Oleduino_Pin_Map.h"


class BatteryMonitor {
    
public:
    //constructor
    BatteryMonitor(void){};
    
    //init
    bool begin(void){
        if(read(0x11)==0x0)
            return false;
        
        write(0x07, 0xAA55);        //set initial RSOC (necessary?)
        write(0x15, 0x0001);        //set power mode : Operating
        write(0x0A, 0x0000);        //set auto current detection mode
        write(0x16, 0x0000);        //disable thermistor
        write(0x12, 0x0000);        //choose default battery profile (see datasheet)
        write(0x13, 0x0005);        //activate alarm if RSOC lower than 5%
        write(0x14, 0x2800);        //activate alarm if voltage under 2,8volts
        
        return true;
    };
    
    uint16_t getRsoc(void){
        return read(0x0D);
    };
    
    uint16_t getVoltage(void){
        return read(0x09);
    };
    
    uint16_t getCapacity(void){
        return read(0x0F);
    };
    
    
    void setRsocAlarm(uint8_t value){
        write(0x13, value);
    };
    void setVoltageAlarm(uint16_t value){
        write(0x14, value);
    };
    void adjust(uint16_t value){
        write(0x0B, value);
    };
    
    
private:

    uint16_t read(uint8_t cmd){
        uint16_t result; // returned value
        uint16_t r;
        Wire.beginTransmission(BATTERY_MONITOR_ADDRESS);
        Wire.write(cmd);
        r=Wire.endTransmission();
        
        Wire.requestFrom(BATTERY_MONITOR_ADDRESS,2); // now get the byte of data...
        result = Wire.read() | (Wire.read()<< 8);   //LSB first
        
        return result; // and return it as a result of the function readData
    };
    
    void write(uint8_t cmd, uint16_t value){
        Wire.beginTransmission(BATTERY_MONITOR_ADDRESS);
        Wire.write(cmd);
        Wire.write(value & 0xFF);       //LSB first 
        Wire.write(value >> 8);
        Wire.endTransmission();
    };
    
    
    
};

#endif
