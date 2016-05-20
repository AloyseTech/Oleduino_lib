//
//  Buttons.h
//  
//
//  Created by Théo Meyer on 19/08/2015.
//
//

#ifndef _Controls_h
#define _Controls_h
#include <arduino.h>

class Buttons
{
    // user-accessible "public" interface
public:
    Buttons(uint8_t p):pin(p)
    {
        currState = !digitalRead(pin);
        lastState = currState;
        count=0;
    };
    
    //true if pressed
    bool isPressed(){
        lastState = currState;
        currState = !digitalRead(pin);
        if(currState)
            count++;
        else
            count=0;
        return currState;
    }
    
    //true if was not pressed before and pressed now
    bool justPressed(void){
        return (currState)&&(!lastState);
    }
    
    //true if was pressed but not now
    bool justReleased(void){
        return (!currState)&&(lastState);
    }
    
uint16_t count;

    // library-accessible "private" interface
private:
    uint8_t pin;
    bool lastState, currState;
    
};


//uncomment if the X or Y value are invered (i.e. you get -x instead of +x)
//#define INVERSE_X_JOYSTICK
//#define INVERSE_Y_JOYSTICK

#define CENTER_X_THRESHOLD 10
#define CENTER_Y_THRESHOLD 10

#define INVERSE_Y_JOYSTICK

class Joystick
{
    // user-accessible "public" interface
public:
    Joystick(void)
    {
        
    }
    
    void read(){
        lastRaw_X=raw_X;
        lastRaw_Y=raw_Y;
        
        #ifdef INVERSE_X_JOYSTICK
        raw_X = 4095-analogRead(JOYSTICK_X_PIN);
        #else
        raw_X = analogRead(JOYSTICK_X_PIN);
        #endif
        
        #ifdef INVERSE_Y_JOYSTICK
        raw_Y = 4095-analogRead(JOYSTICK_Y_PIN);
        #else
        raw_Y = analogRead(JOYSTICK_Y_PIN);
        #endif
        
    }
    
    uint16_t getRawX(void){
        return raw_X;
    }
    uint16_t getRawY(void){
        return raw_Y;
    }
    
    uint16_t getNormalizedX(float factor){
        return raw_X/(max_X-min_X)*factor;
    }
    uint16_t getNormalizedY(float factor){
        return raw_Y/(max_Y-min_Y)*factor;
    }
    
    int16_t getCenteredX(void){
        return raw_X-center_X;
    }
    int16_t getCenteredY(void){
        return raw_Y-center_Y;
    }
    
    int16_t getSpeedX(void){
        return raw_X-lastRaw_X;
    }
    int16_t getSpeedY(void){
        return raw_Y-lastRaw_Y;
    }
    
    float getDirectionRad(void){
        return atan2(raw_X, raw_Y);
    }
    uint16_t getAmplitude(void){
        return sqrt(raw_X*raw_X+raw_Y*raw_Y);
    }
    
    void setCenter(uint16_t x,uint16_t y){
        center_X=x;
        center_Y=y;
    }
    void setXrange(uint16_t min, uint16_t max){
        min_X=min;
        max_X=max;
    }
    void setYrange(uint16_t min, uint16_t max){
        min_Y=min;
        max_Y=max;
    }
    
    // library-accessible "private" interface
private:
    uint16_t raw_X, raw_Y, lastRaw_X, lastRaw_Y, min_X=0, max_X=4095, min_Y=0, max_Y=4095, center_X=2048, center_Y=2048;
};


#endif
