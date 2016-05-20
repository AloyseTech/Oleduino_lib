/*************************************************** 
Display library code for Oleduino
 
 Part from Adafruit SSD1351 lib
 ****************************************************/

#define SSD1351WIDTH 128
#define SSD1351HEIGHT 128
#define swap(a, b) { uint16_t t = a; a = b; b = t; }

#include "Arduino.h"


typedef volatile RwReg PortReg;
typedef uint32_t PortMask;

#define SSD1351_COLORORDER_RGB

// Timing Delays
#define SSD1351_DELAYS_HWFILL	    (3)
#define SSD1351_DELAYS_HWLINE       (1)

// SSD1351 Commands
#define SSD1351_CMD_SETCOLUMN 		0x15
#define SSD1351_CMD_SETROW    		0x75
#define SSD1351_CMD_WRITERAM   		0x5C
#define SSD1351_CMD_READRAM   		0x5D
#define SSD1351_CMD_SETREMAP 		0xA0
#define SSD1351_CMD_STARTLINE 		0xA1
#define SSD1351_CMD_DISPLAYOFFSET 	0xA2
#define SSD1351_CMD_DISPLAYALLOFF 	0xA4
#define SSD1351_CMD_DISPLAYALLON  	0xA5
#define SSD1351_CMD_NORMALDISPLAY 	0xA6
#define SSD1351_CMD_INVERTDISPLAY 	0xA7
#define SSD1351_CMD_FUNCTIONSELECT 	0xAB
#define SSD1351_CMD_DISPLAYOFF 		0xAE
#define SSD1351_CMD_DISPLAYON     	0xAF
#define SSD1351_CMD_PRECHARGE 		0xB1
#define SSD1351_CMD_DISPLAYENHANCE	0xB2
#define SSD1351_CMD_CLOCKDIV 		0xB3
#define SSD1351_CMD_SETVSL 		0xB4
#define SSD1351_CMD_SETGPIO 		0xB5
#define SSD1351_CMD_PRECHARGE2 		0xB6
#define SSD1351_CMD_SETGRAY 		0xB8
#define SSD1351_CMD_USELUT 		0xB9
#define SSD1351_CMD_PRECHARGELEVEL 	0xBB
#define SSD1351_CMD_VCOMH 		0xBE
#define SSD1351_CMD_CONTRASTABC		0xC1
#define SSD1351_CMD_CONTRASTMASTER	0xC7
#define SSD1351_CMD_MUXRATIO            0xCA
#define SSD1351_CMD_COMMANDLOCK         0xFD
#define SSD1351_CMD_HORIZSCROLL         0x96
#define SSD1351_CMD_STOPSCROLL          0x9E
#define SSD1351_CMD_STARTSCROLL         0x9F

// OLED GPIO mode for setGPIO()
enum OLED_GPIO_Mode {
    OLED_HIZ = 0,
    OLED_LOW = 2,
    OLED_HIGH = 3,
};

class Oled_Display  : public virtual GFX {
 public:
    Oled_Display(uint8_t CS, uint8_t RS, uint8_t RST);

  uint16_t Color565(uint8_t r, uint8_t g, uint8_t b);

    
  //bitmaps
//#ifdef __SD_H__
    /*
    uint32_t bmpDrawFromSD(char *filename, char x, char y);
    uint32_t bmpDrawFromSD(char *filename, char x, char y, uint16_t skipped_color);
*/
     //#endif
    void drawBitmapFromMem(int8_t x, int8_t y,const uint16_t *bitmap, uint8_t w, uint8_t h);
    void drawBitmapFromMem(int8_t x, int8_t y,const uint16_t *bitmap, uint8_t w, uint8_t h, uint16_t transparent_color);
        
  // drawing primitives!
  void drawPixel(int16_t x, int16_t y, uint16_t color);
  void fillRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t color);
  void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
  void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
  void fillScreen(uint16_t fillcolor);

  void invert(boolean);
  // commands
  void begin(void);
  void goTo(int x, int y);
  void setLeftLeds(OLED_GPIO_Mode mode);
  void setRightLeds(OLED_GPIO_Mode mode);

  void reset(void);

  /* low level */

  void writeData(uint8_t d);
  void writeCommand(uint8_t c);


  void writeData_unsafe(uint16_t d);

  void setWriteDir(void);
  void write8(uint8_t d);

 private:
  void spiwrite(uint8_t);
  void rawFillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t fillcolor);
  void rawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
  void rawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);

  uint8_t _cs, _rs, _rst, _sid, _sclk;
  PortReg *csport, *rsport, *sidport, *sclkport;
  PortMask cspinmask, rspinmask, sidpinmask, sclkpinmask;

protected:
    uint8_t gpio_status;
};
