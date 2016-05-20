//
//  Oleduino_Pin_Map.h
//  
//
//  Created by Théo Meyer on 19/08/2015.
//
//

#ifndef _Oleduino_Pin_Map_h
#define _Oleduino_Pin_Map_h


//pin mapping of components
#define DISPLAY_CS_PIN              38      //ATN pin           PA13
#define DISPLAY_DC_PIN              8       //digital pin 8     PA06
#define DISPLAY_RESET_PIN           5       //digital pin 5     PA15
#define SD_CS_PIN                   6       //digital pin 6     PA20
#define DAC_PIN                     14      //analog pin A0     PA02
#define JOYSTICK_X_PIN              15      //analog pin A1     PB08
#define JOYSTICK_Y_PIN              16      //analog pin A2     PB09
#define SWITCH_A_PIN                4       //digital pin 4     PA08
#define SWITCH_B_PIN                3       //digital pin 3     PA09
#define SWITCH_C_PIN                2       //digital pin 2     PA14
#define L_LED_PIN                   13      //digital pin 13    PA17
#define MAGNETOMETER_DRY_PIN        27      //USB_HOST_ENABLE   PA28    //host mode impossible : lack of 5V power
#define ACCEL_GYRO_INT1_PIN         10      //digital pin 10    PA18
#define ACCEL_GYRO_INT2_PIN         11      //digital pin 11    PA16
#define VIBRATOR_PIN                7       //digital pin 7     PA21    //only with Oleduino Shield
#define GPIO_0_PIN                  7       //digital pin 7     PA21
#define WIFI_ENABLE                 9       //digital pin 9     PA07    //only with Oleduino Shield
#define GPIO_1_PIN                  9       //digital pin 9     PA07
#define RTC_ALARM                   12      //digital pin 12    PA19    //only with Oleduino Shield
#define GPIO_2_PIN                  12      //digital pin 12    PA19
#define GPIO_3_PIN                  30      //TXD               PB22
#define BATTERY_ALARM               31      //RXD               PB23
#define GPIO_4_PIN                  31      //RXD               PB23
#define AUDIO_IN                    A5      //analog pin 5      xxxx      //only with shield

//address of I2C components
#define MAGNETOMETER_ADDRESS        0x1E    //pin SDO_M to VCC via pull-up resistor
#define ACCEL_GYRO_ADDRESS          0x6B    //pin SDO_AG to VCC via pull-up resistor
#define BME_ADDRESS                 0x76    //pin SDO to GND via pull-down resistor
#define RTC_ADDRESS                 0x6F    //Fixed, see datasheet
#define BATTERY_MONITOR_ADDRESS     0xB    //Fixed 0x16, see datasheet //scanner says 0xB (11)???

#endif
