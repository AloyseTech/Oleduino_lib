/*************************************************** 
  This is a library for the 1.5" & 1.27" 16-bit Color OLEDs 
  with SSD1331 driver chip

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/1431
  ------> http://www.adafruit.com/products/1673

  These displays use SPI to communicate, 4 or 5 pins are required to  
  interface
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include "arduino.h"
#include "GFX.h"
#include "Display.h"
#include "glcdfont.c"
#include <avr/pgmspace.h>
#include "pins_arduino.h"
#include "wiring_private.h"
#include <SPI.h>


#ifndef _BV
    #define _BV(bit) (1<<(bit))
#endif

/********************************** low level pin interface */

inline void Oled_Display::spiwrite(uint8_t c) {
    
    SPI.transfer(c);

    }


void Oled_Display::writeCommand(uint8_t c) {
    //*rsport &= ~ rspinmask;
    //digitalWrite(_rs, LOW);
    PORT->Group[g_APinDescription[_rs].ulPort].OUTCLR.reg = (1ul << g_APinDescription[_rs].ulPin) ;

    
    noInterrupts();
    //*csport &= ~ cspinmask;
    //digitalWrite(_cs, LOW);
    PORT->Group[g_APinDescription[_cs].ulPort].OUTCLR.reg = (1ul << g_APinDescription[_cs].ulPin) ;
    
    SPI.transfer(c);
    
    //*csport |= cspinmask;
    PORT->Group[g_APinDescription[_cs].ulPort].OUTSET.reg = (1ul << g_APinDescription[_cs].ulPin) ;
    
    interrupts();
    
    //digitalWrite(_cs, HIGH);
}


void Oled_Display::writeData(uint8_t c) {
    //*rsport |= rspinmask;
    //digitalWrite(_rs, HIGH);
    PORT->Group[g_APinDescription[_rs].ulPort].OUTSET.reg = (1ul << g_APinDescription[_rs].ulPin) ;
    
    noInterrupts();

    //*csport &= ~ cspinmask;
    //digitalWrite(_cs, LOW);
    PORT->Group[g_APinDescription[_cs].ulPort].OUTCLR.reg = (1ul << g_APinDescription[_cs].ulPin) ;
    
    SPI.transfer(c);

    //*csport |= cspinmask;
    PORT->Group[g_APinDescription[_cs].ulPort].OUTSET.reg = (1ul << g_APinDescription[_cs].ulPin) ;
    interrupts();

    //digitalWrite(_cs, HIGH);
} 

/***********************************/

void Oled_Display::goTo(int x, int y) {
  if ((x >= SSD1351WIDTH) || (y >= SSD1351HEIGHT)) return;
  
  // set x and y coordinate
  writeCommand(SSD1351_CMD_SETCOLUMN);
  writeData(x);
  writeData(SSD1351WIDTH-1);

  writeCommand(SSD1351_CMD_SETROW);
  writeData(y);
  writeData(SSD1351HEIGHT-1);

  writeCommand(SSD1351_CMD_WRITERAM);  
}

uint16_t Oled_Display::Color565(uint8_t r, uint8_t g, uint8_t b) {
  uint16_t c;
  c = r >> 3;
  c <<= 6;
  c |= g >> 2;
  c <<= 5;
  c |= b >> 3;

  return c;
}

void Oled_Display::fillScreen(uint16_t fillcolor) {
  fillRect(0, 0, SSD1351WIDTH, SSD1351HEIGHT, fillcolor);
}

// Draw a filled rectangle with no rotation.
void Oled_Display::rawFillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t fillcolor) {
  // Bounds check
  if ((x >= SSD1351WIDTH) || (y >= SSD1351HEIGHT))
    return;

  // Y bounds check
  if (y+h > SSD1351HEIGHT)
  {
    h = SSD1351HEIGHT - y - 1;
  }

  // X bounds check
  if (x+w > SSD1351WIDTH)
  {
    w = SSD1351WIDTH - x - 1;
  }
  


  // set location
  writeCommand(SSD1351_CMD_SETCOLUMN);
  writeData(x);
  writeData(x+w-1);
  writeCommand(SSD1351_CMD_SETROW);
  writeData(y);
  writeData(y+h-1);
  // fill!
  writeCommand(SSD1351_CMD_WRITERAM);  

  for (uint16_t i=0; i < w*h; i++) {
    writeData(fillcolor >> 8);
    writeData(fillcolor);
  }
}

/**************************************************************************/
/*!
    @brief  Draws a filled rectangle using HW acceleration
*/
/**************************************************************************/
void Oled_Display::fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t fillcolor) {
  // Transform x and y based on current rotation.
  switch (getRotation()) {
  case 0:  // No rotation
    rawFillRect(x, y, w, h, fillcolor);
    break;
  case 1:  // Rotated 90 degrees clockwise.
    swap(x, y);
    x = WIDTH - x - h;
    rawFillRect(x, y, h, w, fillcolor);
    break;
  case 2:  // Rotated 180 degrees clockwise.
    x = WIDTH - x - w;
    y = HEIGHT - y - h;
    rawFillRect(x, y, w, h, fillcolor);
    break;
  case 3:  // Rotated 270 degrees clockwise.
    swap(x, y);
    y = HEIGHT - y - w;
    rawFillRect(x, y, h, w, fillcolor);
    break;
  }
}

// Draw a horizontal line ignoring any screen rotation.
void Oled_Display::rawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
  // Bounds check
  if ((x >= SSD1351WIDTH) || (y >= SSD1351HEIGHT))
    return;

  // X bounds check
  if (x+w > SSD1351WIDTH)
  {
    w = SSD1351WIDTH - x - 1;
  }

  if (w < 0) return;

  // set location
  writeCommand(SSD1351_CMD_SETCOLUMN);
  writeData(x);
  writeData(x+w-1);
  writeCommand(SSD1351_CMD_SETROW);
  writeData(y);
  writeData(y);
  // fill!
  writeCommand(SSD1351_CMD_WRITERAM);  

  for (uint16_t i=0; i < w; i++) {
    writeData(color >> 8);
    writeData(color);
  }
}

// Draw a vertical line ignoring any screen rotation.
void Oled_Display::rawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
  // Bounds check
  if ((x >= SSD1351WIDTH) || (y >= SSD1351HEIGHT))
  return;

  // X bounds check
  if (y+h > SSD1351HEIGHT)
  {
    h = SSD1351HEIGHT - y - 1;
  }

  if (h < 0) return;

  // set location
  writeCommand(SSD1351_CMD_SETCOLUMN);
  writeData(x);
  writeData(x);
  writeCommand(SSD1351_CMD_SETROW);
  writeData(y);
  writeData(y+h-1);
  // fill!
  writeCommand(SSD1351_CMD_WRITERAM);  

  for (uint16_t i=0; i < h; i++) {
    writeData(color >> 8);
    writeData(color);
  }
}

void Oled_Display::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
  // Transform x and y based on current rotation.
  switch (getRotation()) {
  case 0:  // No rotation
    rawFastVLine(x, y, h, color);
    break;
  case 1:  // Rotated 90 degrees clockwise.
    swap(x, y);
    x = WIDTH - x - h;
    rawFastHLine(x, y, h, color);
    break;
  case 2:  // Rotated 180 degrees clockwise.
    x = WIDTH - x - 1;
    y = HEIGHT - y - h;
    rawFastVLine(x, y, h, color);
    break;
  case 3:  // Rotated 270 degrees clockwise.
    swap(x, y);
    y = HEIGHT - y - 1;
    rawFastHLine(x, y, h, color);
    break;
  }
}

void Oled_Display::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
  // Transform x and y based on current rotation.
  switch (getRotation()) {
  case 0:  // No rotation.
    rawFastHLine(x, y, w, color);
    break;
  case 1:  // Rotated 90 degrees clockwise.
    swap(x, y);
    x = WIDTH - x - 1;
    rawFastVLine(x, y, w, color);
    break;
  case 2:  // Rotated 180 degrees clockwise.
    x = WIDTH - x - w;
    y = HEIGHT - y - 1;
    rawFastHLine(x, y, w, color);
    break;
  case 3:  // Rotated 270 degrees clockwise.
    swap(x, y);
    y = HEIGHT - y - w;
    rawFastVLine(x, y, w, color);
    break;
  }
}

void Oled_Display::drawPixel(int16_t x, int16_t y, uint16_t color)
{
  // Transform x and y based on current rotation.
  switch (getRotation()) {
  // Case 0: No rotation
  case 1:  // Rotated 90 degrees clockwise.
    swap(x, y);
    x = WIDTH - x - 1;
    break;
  case 2:  // Rotated 180 degrees clockwise.
    x = WIDTH - x - 1;
    y = HEIGHT - y - 1;
    break;
  case 3:  // Rotated 270 degrees clockwise.
    swap(x, y);
    y = HEIGHT - y - 1;
    break;
  }

  // Bounds check.
  if ((x >= SSD1351WIDTH) || (y >= SSD1351HEIGHT)) return;
  if ((x < 0) || (y < 0)) return;

  goTo(x, y);
  
  // setup for data
  *rsport |= rspinmask;
    
  noInterrupts();

  *csport &= ~ cspinmask;
  
  SPI.transfer(color >> 8);
  SPI.transfer(color);

  *csport |= cspinmask;
  
  interrupts();

}

void Oled_Display::begin(void) {
    // set pin directions
    pinMode(_rs, OUTPUT);
    
        // using the hardware SPI
        //SPI.beginTransaction(SPISettings(12000000, MSBFIRST, SPI_MODE0));
        SPI.begin();
        //SPI.setDataMode(SPI_MODE3);
        //SPI.setClockDivider(4);

    
	
    // Toggle RST low to reset; CS low so it'll listen to us
    pinMode(_cs, OUTPUT);
    digitalWrite(_cs, LOW);
    
    if (_rst) {
        pinMode(_rst, OUTPUT);
        digitalWrite(_rst, HIGH);
        digitalWrite(_rst, LOW);
        delay(4);
        digitalWrite(_rst, HIGH);
        delay(4);
    }

    // Initialization Sequence
    writeCommand(SSD1351_CMD_COMMANDLOCK);  // set command lock
    writeData(0x12);  
    writeCommand(SSD1351_CMD_COMMANDLOCK);  // set command lock
    writeData(0xB1);

    writeCommand(SSD1351_CMD_DISPLAYOFF);  		// 0xAE

    writeCommand(SSD1351_CMD_CLOCKDIV);  		// 0xB3
    writeCommand(0xF1);  						// 7:4 = Oscillator Frequency, 3:0 = CLK Div Ratio (A[3:0]+1 = 1..16)
    
    writeCommand(SSD1351_CMD_MUXRATIO);
    writeData(127);
    
    writeCommand(SSD1351_CMD_SETREMAP);
    writeData(0x74);
  
    writeCommand(SSD1351_CMD_SETCOLUMN);
    writeData(0x00);
    writeData(0x7F);
    writeCommand(SSD1351_CMD_SETROW);
    writeData(0x00);
    writeData(0x7F);

    writeCommand(SSD1351_CMD_STARTLINE); 		// 0xA1
    if (SSD1351HEIGHT == 96) {
      writeData(96);
    } else {
      writeData(0);
    }


    writeCommand(SSD1351_CMD_DISPLAYOFFSET); 	// 0xA2
    writeData(0x0);

    writeCommand(SSD1351_CMD_SETGPIO);
    writeData(0x00);
    
    writeCommand(SSD1351_CMD_FUNCTIONSELECT);
    writeData(0x01); // internal (diode drop)
    //writeData(0x01); // external bias

//    writeCommand(SSSD1351_CMD_SETPHASELENGTH);
//    writeData(0x32);

    writeCommand(SSD1351_CMD_PRECHARGE);  		// 0xB1
    writeCommand(0x32);
 
    writeCommand(SSD1351_CMD_VCOMH);  			// 0xBE
    writeCommand(0x05);

    writeCommand(SSD1351_CMD_NORMALDISPLAY);  	// 0xA6

    writeCommand(SSD1351_CMD_CONTRASTABC);
    writeData(0xC8);
    writeData(0x80);
    writeData(0xC8);

    writeCommand(SSD1351_CMD_CONTRASTMASTER);
    writeData(0x0F);

    writeCommand(SSD1351_CMD_SETVSL );
    writeData(0xA0);
    writeData(0xB5);
    writeData(0x55);
    
    writeCommand(SSD1351_CMD_PRECHARGE2);
    writeData(0x01);
    
    gpio_status=0b00001010;
    
    writeCommand(SSD1351_CMD_DISPLAYON);		//--turn on oled panel    
}

void  Oled_Display::invert(boolean v) {
   if (v) {
     writeCommand(SSD1351_CMD_INVERTDISPLAY);
   } else {
     writeCommand(SSD1351_CMD_NORMALDISPLAY);
   }
 }


Oled_Display::Oled_Display(uint8_t cs, uint8_t rs,  uint8_t rst) : GFX(SSD1351WIDTH, SSD1351HEIGHT) {
    _cs = cs;
    _rs = rs;
    _sid = 0;
    _sclk = 0;
    _rst = rst;

    csport      = portOutputRegister(digitalPinToPort(cs));
    cspinmask   = digitalPinToBitMask(cs);
    
    rsport      = portOutputRegister(digitalPinToPort(rs));
    rspinmask   = digitalPinToBitMask(rs);

}



void Oled_Display::drawBitmapFromMem(int8_t x, int8_t y,const uint16_t *bitmap, uint8_t w, uint8_t h) {
    
    uint16_t width_pointer, heigth_pointer, pixel_color;
    
    for(heigth_pointer=0; heigth_pointer<h; heigth_pointer++) {
        for(width_pointer=0; width_pointer<w; width_pointer++ ) {
            pixel_color=pgm_read_word_near(bitmap + (heigth_pointer * w + width_pointer));
            drawPixel(x+width_pointer, y+heigth_pointer, pixel_color);
        }
    }
}

void Oled_Display::drawBitmapFromMem(int8_t x, int8_t y,const uint16_t *bitmap, uint8_t w, uint8_t h, uint16_t transparent_color) {
    
    uint16_t width_pointer, heigth_pointer, pixel_color;
    
    for(heigth_pointer=0; heigth_pointer<h; heigth_pointer++) {
        for(width_pointer=0; width_pointer<w; width_pointer++ ) {
            pixel_color=pgm_read_word_near(bitmap + (heigth_pointer * w + width_pointer));
            if(pixel_color!=transparent_color)
            drawPixel(x+width_pointer, y+heigth_pointer, pixel_color);
        }
    }
}



void Oled_Display::setLeftLeds(OLED_GPIO_Mode mode){
    writeCommand(0xB5);
    gpio_status=(gpio_status & ~0x03) | (uint8_t)mode;
    writeData(gpio_status);
}

void Oled_Display::setRightLeds(OLED_GPIO_Mode mode){
    writeCommand(0xB5);
    gpio_status=(gpio_status & ~0x0C) | (uint8_t)mode << 2;
    writeData(gpio_status);
}











