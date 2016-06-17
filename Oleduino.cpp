/*
  Oleduino.h, used by Arduino IDE at build time
*/

// include core arduino API
#include "arduino.h"

// include this library's description file
#include "Oleduino.h"
#include <SPI.h>

// include description files for other libraries used (if any)

// Constructor /////////////////////////////////////////////////////////////////
// Function that handles the creation and setup of instances

Oleduino::Oleduino()
{

}

// Public Methods //////////////////////////////////////////////////////////////

bool Oleduino::init()
{
    pinMode(13, OUTPUT);
    pinMode(WIFI_ENABLE, OUTPUT);
    pinMode(SWITCH_A_PIN, INPUT_PULLUP);
    pinMode(SWITCH_B_PIN, INPUT_PULLUP);
    pinMode(SWITCH_C_PIN, INPUT_PULLUP);
    pinMode(VIBRATOR_PIN,OUTPUT);
    
    digitalWrite(WIFI_ENABLE,LOW); //disable wifi by default
    digitalWrite(VIBRATOR_PIN,LOW); //disable vibrator by default
    digitalWrite(13, HIGH);

    analogReadResolution(12);
    analogWriteResolution(10);
    
    pinMode(A0,OUTPUT);
    digitalWrite(A0, LOW);
    
#ifndef DEBUG
    SD.begin(SD_CS_PIN);
#else
    while(!SerialUSB);
    SerialUSB.println("SD begin : " + String(SD.begin(SD_CS_PIN)));
#endif
    
    display.begin();
    display.fillScreen(0);
    
    Wire.begin();
    Wire.setClock(10000); // 200000 = around 1.2MHz, maximum is around 1.6Mhz with 3000000 clock value.
    if(!imu.begin())return false;
    if(!bme.begin())return false;
    if(!batteryMonitor.begin())return false;

    

    
    digitalWrite(13, LOW);
    return true;
}



void Oleduino::keyboard(char text[],uint8_t length, String title){
    display.fillScreen(0);
    display.setTextSize(1);
    text[length - 1] = '\0';
    //active character in the typing area
    int8_t activeChar = 0;
    //selected char on the keyboard
    int8_t activeX = 1;
    int8_t activeY = 4;
    //position of the keyboard on the screen
    int8_t currentX = KEYBOARD_POS_X;
    int8_t currentY = KEYBOARD_POS_Y;
    int lastX = currentX;
    int lastY = currentY;
    int lastActX = activeX;
    int lastActY = activeY;
    
    bool alternate = true;;
    unsigned long lastUpadte = millis();
    
    //draw the keyboard
    for (int8_t y = 0; y < KEYBOARD_H; y++) {
        for (int8_t x = 0; x < KEYBOARD_W; x++) {
            display.drawChar(KEYBOARD_POS_X + x * (FONT_WIDTH + 1), KEYBOARD_POS_Y + y * (FONT_HEIGHT + 1), x + y * KEYBOARD_W, WHITE, BLACK, 1);
        }
    }
    display.setTextColor(GREEN);
    display.setCursor(0, 0);
    display.println(title);
    
    display.setTextColor(BLUE);
    display.setCursor(32, 90);
    display.println("A:Select");
    display.setCursor(32, 98);
    display.println("B:Backspace");
    display.setCursor(32, 106);
    display.println("C:Done");
    
    while (1) {
        joystick.read();
        int _joyX = joystick.getCenteredX() - 300;
        int _joyY = joystick.getCenteredY() + 90;
        _joyX /= 300;
        _joyY /= 300;
        
        
        if (millis() - lastUpadte > 110/*update()*/) {
            lastUpadte = millis();
            //move the character selector
            if (_joyY > 2) {
                activeY++;
                //sound.playTick();
            }
            if (_joyY < -2) {
                activeY--;
                //sound.playTick();
            }
            if (_joyX > 2) {
                activeX++;
                //sound.playTick();
            }
            if (_joyX < -2) {
                activeX--;
                //sound.playTick();
            }
            //don't go out of the keyboard
            if (activeX == KEYBOARD_W) activeX = 0;
            if (activeX < 0) activeX = KEYBOARD_W - 1;
            if (activeY == KEYBOARD_H) activeY = 0;
            if (activeY < 0) activeY = KEYBOARD_H - 1;
            
            //type character
            if (A.isPressed() && A.justPressed()) {
                if (activeChar < (length - 1)) {
                    byte thisChar = activeX + KEYBOARD_W * activeY;
                    if ((thisChar == 0) || (thisChar == 10) || (thisChar == 13)) //avoid line feed and carriage return
                        continue;
                    text[activeChar] = thisChar;
                    text[activeChar + 1] = '\0';
                }
                activeChar++;
                //sound.playOK();
                if (activeChar > length)
                    activeChar = length;
            }
            //erase character
            if (B.isPressed() && B.justPressed()) {
                activeChar--;
                //sound.playCancel();
                if (activeChar >= 0)
                    text[activeChar] = 0;
                else
                    activeChar = 0;
            }
            //leave menu
            if (C.isPressed() && C.justPressed()) {
                //sound.playOK();
                display.fillScreen(0);
                while (1) {
                    display.setCursor(0, 0);
                    display.setTextColor(GREEN);
                    display.println(F("You entered :\n"));
                    String t = text;
                    display.setTextColor(SPECOLOR);
                    display.print(text);
                    display.setTextColor(BLUE);
                    display.setCursor(0,120);
                    display.println(F("A:okay  B:edit"));
                    if (A.isPressed() && A.justPressed()) {
                        //sound.playOK();
                        //display.fillScreen(0);
                        return;
                    }
                    if (B.isPressed() && B.justPressed()) {
                        //sound.playCancel();
                        display.fillScreen(0);
                        //draw the keyboard
                        for (int8_t y = 0; y < KEYBOARD_H; y++) {
                            for (int8_t x = 0; x < KEYBOARD_W; x++) {
                                display.drawChar(KEYBOARD_POS_X + x * (FONT_WIDTH + 1), KEYBOARD_POS_Y + y * (FONT_HEIGHT + 1), x + y * KEYBOARD_W, WHITE, BLACK, 1);
                            }
                        }
                        display.setTextColor(GREEN);
                        display.setCursor(0, 0);
                        display.println(title);
                        
                        display.setTextColor(BLUE);
                        display.setCursor(32, 90);
                        display.println("A:Select");
                        display.setCursor(32, 98);
                        display.println("B:Backspace");
                        display.setCursor(32, 106);
                        display.println("C:Done");
                        
                        break;
                    }
                }
            }
            
            
            if (alternate) {    //erase selection rectangle
                display.drawRoundRect(lastX + lastActX * (FONT_WIDTH + 1) - 2, lastY + lastActY * (FONT_HEIGHT + 1) - 1, (FONT_WIDTH + 2) + (FONT_WIDTH - 1) % 2, (FONT_HEIGHT + 2), 2, BLACK);
            }
            else {    //draw the selection rectangle
                display.drawRoundRect(currentX + activeX * (FONT_WIDTH + 1) - 2, currentY + activeY * (FONT_HEIGHT + 1) - 1, (FONT_WIDTH + 2) + (FONT_WIDTH - 1) % 2, (FONT_HEIGHT + 2), 2, RED);
            }
            
            lastX = currentX;
            lastY = currentY;
            lastActX = activeX;
            lastActY = activeY;
            alternate = !alternate;
            //draw keyboard outline
            display.drawRoundRect(currentX - 4, currentY - 4, KEYBOARD_W * (FONT_WIDTH+1) + 7, KEYBOARD_H * (FONT_HEIGHT+1) + 6, 4, SPECOLOR);
            
            //text field
            display.drawFastHLine(0, DISPLAY_HEIGHT - FONT_HEIGHT - 2, DISPLAY_WIDTH, WHITE);
            //display.fillRect(0, DISPLAY_HEIGHT - FONT_HEIGHT - 1, DISPLAY_WIDTH, FONT_HEIGHT + 1, BLACK);
            //typed text
            display.setCursor(0, DISPLAY_HEIGHT - FONT_HEIGHT);
            display.setTextColor(WHITE);
            
            for (int k = 0; k < length - 1; k++) {
                display.drawChar(FONT_WIDTH * k, DISPLAY_HEIGHT - FONT_HEIGHT, ' ', WHITE, BLACK, 1);
                if (k < activeChar)
                    display.print(text[k]);
            }
            
            //blinking cursor
            if (((millis() % 8) < 4) && (activeChar < (length - 1))) {
                //display.drawChar(FONT_WIDTH * activeChar, DISPLAY_HEIGHT - FONT_HEIGHT, '_', WHITE, BLACK, 1);
                display.print("_");
            }
            else if (activeChar < (length - 1)) {
                //display.drawChar(FONT_WIDTH * activeChar, DISPLAY_HEIGHT - FONT_HEIGHT, '_', BLACK, BLACK, 1);
                display.print(" ");
            }
            
            
            
        }
    }
}

String Oleduino::display_Running_Sketch (void){
    String the_path = __FILE__;
    int slash_loc = the_path.lastIndexOf('/');
    String the_cpp_name = the_path.substring(slash_loc+1);
    int dot_loc = the_cpp_name.lastIndexOf('.');
    return the_cpp_name.substring(0, dot_loc);
}


void Oleduino::loadApp(char* filename)
{
    if (SD.exists("System/Loader/nextapp.txt"))
        SD.remove("System/Loader/nextapp.txt");
    
    File executable;
    executable = SD.open("System/Loader/NEXTAPP.TXT", O_WRITE | O_CREAT);
    executable.println(filename);
    executable.close();
    
    SPI.end();
    
    //reset GEMS to enter loader. The loader will find the instruction file with which .bin to execute.
    NVIC_SystemReset();
}


/*
void Oleduino::deepSleep(void)
{
 
}

void Oleduino::displayOff(Oled_Display *dis)
{
  dis->writeCommand(SSD1351_CMD_DISPLAYOFF);
}

void Oleduino::displayOn(Oled_Display *dis)
{
//  dis->begin(SSD1306_SWITCHCAPVCC);
}
 
 */


