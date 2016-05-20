/*
  Oleduino.h
Library for the OpenOledGamingConsole
*/


//#ifndef __SAMD21G18A__
//  #error "This library is only for the Oleduino, which runs on SAMD21 ARM CPU Hardware! »
//#endif

// ensure this library description is only included once
#ifndef Oleduino_h
#define Oleduino_h

// include types & constants of arduino core API
#include <arduino.h>

#ifndef SD_FAT_VERSION
#include <SD.h>
#endif

#include "Oleduino_Pin_Map.h"
#include "GFX.h"
#include "Display.h"
#include "Buttons.h"
#include "Orientation_Sensor.h"
#include "BME280.h"
#include "BatteryMonitor.h"
#include "glcdfont.c"
//#include "RTClock.h"


#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 128
#define KEYBOARD_W 16
#define KEYBOARD_H 8
#define FONT_HEIGHT 8
#define FONT_WIDTH 6
#define KEYBOARD_POS_X 8
#define KEYBOARD_POS_Y 14

#define BLACK           0x0000
#define BLUE            0x001F
#define RED             0xF800
#define RED_2           0b0111100000000000
#define GREEN           0x07E0
#define GREEN_2         0b0000001111100000
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0
#define ORANGE          0xFB20
#define SPECOLOR        0b1110011000000000
#define YELLOW2         0b1110011000000000
#define WHITE           0xFFFF
#define BROWN           0x7403
#define FLESH           0xFBEA //0x9f65//0xdfb6
#define GREY            0x75ad
#define PURPLE          0x624F


// library interface description
class Oleduino
{
    // user-accessible "public" interface
    public:
    
    
    //Part of the console
    //dislay
    Oled_Display display = Oled_Display(DISPLAY_CS_PIN, DISPLAY_DC_PIN, DISPLAY_RESET_PIN);
    
    //buttons
    Buttons A = Buttons(SWITCH_A_PIN);
    Buttons B = Buttons(SWITCH_B_PIN);
    Buttons C = Buttons(SWITCH_C_PIN);
    //joystick
    Joystick joystick;
    //inertial mesurement unit  (accelerometer, gyroscope and magnetometer)
    LSM9DS1 imu;
    
    //ambient sensor (BME280)
    BME280 bme;
    //battery monitor
    BatteryMonitor batteryMonitor;
    //real time clock
    //RTClock rtc;
    
    
    
    //Constructor
    Oleduino();

    //show keyBoard to get text input
    void keyboard(char text[],uint8_t length, String title);
    
    //init
    bool init(void);
/*
    //power management
    void deepSleep(void);
    void displayOff(Oled_Display *dis);
    void displayOn(Oled_Display *dis);
*/    
    //control
    void readButtons(void);
    
    
    //Loader, SD, filesystem
    String display_Running_Sketch (void);

#ifndef SD_FAT_VERSION
    void loadApp(String filename);
#endif
  
  // library-accessible "private" interface
  private:

};

#endif

