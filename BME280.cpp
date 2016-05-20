#include "Arduino.h"
#include <Wire.h>
#include <SPI.h>
#include "BME280.h"

static bme280_calib_data bme280_calib;


BME280::BME280(){ }



bool BME280::begin() {

    //Wire.begin(); !!!! already done

  if (read8(BME280_REGISTER_CHIPID) != 0x60)
    return false;

  readCoefficients();
  write8(BME280_REGISTER_CONTROLHUMID, 0x03); // Set before CONTROL (DS 5.4.3)
  write8(BME280_REGISTER_CONTROL, 0x3F);
  return true;
}


/**************************************************************************/
/*!
    @brief  Writes an 8 bit value over I2C/SPI
*/
/**************************************************************************/
void BME280::write8(byte reg, byte value)
{
    Wire.beginTransmission((uint8_t)BME_ADDRESS);
    Wire.write((uint8_t)reg);
    Wire.write((uint8_t)value);
    Wire.endTransmission();
 }

/**************************************************************************/
/*!
    @brief  Reads an 8 bit value over I2C
*/
/**************************************************************************/
uint8_t BME280::read8(byte reg)
{
  uint8_t value;

    Wire.beginTransmission((uint8_t)BME_ADDRESS);
    Wire.write((uint8_t)reg);
    Wire.endTransmission();
    Wire.requestFrom((uint8_t)BME_ADDRESS, (byte)1);
    value = Wire.read();
    Wire.endTransmission();

  return value;
}

/**************************************************************************/
/*!
    @brief  Reads a 16 bit value over I2C
*/
/**************************************************************************/
uint16_t BME280::read16(byte reg)
{
  uint16_t value;

    Wire.beginTransmission((uint8_t)BME_ADDRESS);
    Wire.write((uint8_t)reg);
    Wire.endTransmission();
    Wire.requestFrom((uint8_t)BME_ADDRESS, (byte)2);
    value = (Wire.read() << 8) | Wire.read();
    Wire.endTransmission();


  return value;
}

uint16_t BME280::read16_LE(byte reg) {
  uint16_t temp = read16(reg);
  return (temp >> 8) | (temp << 8);

}

/**************************************************************************/
/*!
    @brief  Reads a signed 16 bit value over I2C
*/
/**************************************************************************/
int16_t BME280::readS16(byte reg)
{
  return (int16_t)read16(reg);

}

int16_t BME280::readS16_LE(byte reg)
{
  return (int16_t)read16_LE(reg);

}

/**************************************************************************/
/*!
    @brief  Reads the factory-set coefficients
*/
/**************************************************************************/
void BME280::readCoefficients(void)
{
    bme280_calib.dig_T1 = read16_LE(BME280_REGISTER_DIG_T1);
    bme280_calib.dig_T2 = readS16_LE(BME280_REGISTER_DIG_T2);
    bme280_calib.dig_T3 = readS16_LE(BME280_REGISTER_DIG_T3);

    bme280_calib.dig_P1 = read16_LE(BME280_REGISTER_DIG_P1);
    bme280_calib.dig_P2 = readS16_LE(BME280_REGISTER_DIG_P2);
    bme280_calib.dig_P3 = readS16_LE(BME280_REGISTER_DIG_P3);
    bme280_calib.dig_P4 = readS16_LE(BME280_REGISTER_DIG_P4);
    bme280_calib.dig_P5 = readS16_LE(BME280_REGISTER_DIG_P5);
    bme280_calib.dig_P6 = readS16_LE(BME280_REGISTER_DIG_P6);
    bme280_calib.dig_P7 = readS16_LE(BME280_REGISTER_DIG_P7);
    bme280_calib.dig_P8 = readS16_LE(BME280_REGISTER_DIG_P8);
    bme280_calib.dig_P9 = readS16_LE(BME280_REGISTER_DIG_P9);

    bme280_calib.dig_H1 = read8(BME280_REGISTER_DIG_H1);
    bme280_calib.dig_H2 = readS16_LE(BME280_REGISTER_DIG_H2);
    bme280_calib.dig_H3 = read8(BME280_REGISTER_DIG_H3);
    bme280_calib.dig_H4 = (read8(BME280_REGISTER_DIG_H4) << 4) | (read8(BME280_REGISTER_DIG_H4+1) & 0xF);
    bme280_calib.dig_H5 = (read8(BME280_REGISTER_DIG_H5+1) << 4) | (read8(BME280_REGISTER_DIG_H5) >> 4);
    bme280_calib.dig_H6 = (int8_t)read8(BME280_REGISTER_DIG_H6);
}

/**************************************************************************/
/*!
    return temperature in °C
*/
/**************************************************************************/
float BME280::readTemperature(void)
{
  int32_t var1, var2;

  int32_t adc_T = read16(BME280_REGISTER_TEMPDATA);
  adc_T <<= 8;
  adc_T |= read8(BME280_REGISTER_TEMPDATA+2);
  adc_T >>= 4;

  var1  = ((((adc_T>>3) - ((int32_t)bme280_calib.dig_T1 <<1))) *
	   ((int32_t)bme280_calib.dig_T2)) >> 11;

  var2  = (((((adc_T>>4) - ((int32_t)bme280_calib.dig_T1)) *
	     ((adc_T>>4) - ((int32_t)bme280_calib.dig_T1))) >> 12) *
	   ((int32_t)bme280_calib.dig_T3)) >> 14;

  t_fine = var1 + var2;

  float T  = (t_fine * 5 + 128) >> 8;
  return T/100.0;
}

/**************************************************************************/
/*!
    return pressure in Pa
*/
/**************************************************************************/
float BME280::readPressure(void) {
  int64_t var1, var2, p;

  int32_t adc_P = read16(BME280_REGISTER_PRESSUREDATA);
  adc_P <<= 8;
  adc_P |= read8(BME280_REGISTER_PRESSUREDATA+2);
  adc_P >>= 4;

  var1 = ((int64_t)t_fine) - 128000;
  var2 = var1 * var1 * (int64_t)bme280_calib.dig_P6;
  var2 = var2 + ((var1*(int64_t)bme280_calib.dig_P5)<<17);
  var2 = var2 + (((int64_t)bme280_calib.dig_P4)<<35);
  var1 = ((var1 * var1 * (int64_t)bme280_calib.dig_P3)>>8) +
    ((var1 * (int64_t)bme280_calib.dig_P2)<<12);
  var1 = (((((int64_t)1)<<47)+var1))*((int64_t)bme280_calib.dig_P1)>>33;

  if (var1 == 0) {
    return 0;  // avoid exception caused by division by zero
  }
  p = 1048576 - adc_P;
  p = (((p<<31) - var2)*3125) / var1;
  var1 = (((int64_t)bme280_calib.dig_P9) * (p>>13) * (p>>13)) >> 25;
  var2 = (((int64_t)bme280_calib.dig_P8) * p) >> 19;

  p = ((p + var1 + var2) >> 8) + (((int64_t)bme280_calib.dig_P7)<<4);
  return (float)p/256;
}

/**************************************************************************/
/*!
    Return humidity in %rel
*/
/**************************************************************************/
float BME280::readHumidity(void) {

  int32_t adc_H = read16(BME280_REGISTER_HUMIDDATA);

  int32_t v_x1_u32r;

  v_x1_u32r = (t_fine - ((int32_t)76800));

  v_x1_u32r = (((((adc_H << 14) - (((int32_t)bme280_calib.dig_H4) << 20) -
		  (((int32_t)bme280_calib.dig_H5) * v_x1_u32r)) + ((int32_t)16384)) >> 15) *
	       (((((((v_x1_u32r * ((int32_t)bme280_calib.dig_H6)) >> 10) *
		    (((v_x1_u32r * ((int32_t)bme280_calib.dig_H3)) >> 11) + ((int32_t)32768))) >> 10) +
		  ((int32_t)2097152)) * ((int32_t)bme280_calib.dig_H2) + 8192) >> 14));

  v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) *
			     ((int32_t)bme280_calib.dig_H1)) >> 4));

  v_x1_u32r = (v_x1_u32r < 0) ? 0 : v_x1_u32r;
  v_x1_u32r = (v_x1_u32r > 419430400) ? 419430400 : v_x1_u32r;
  float h = (v_x1_u32r>>12);
  return  h / 1024.0;
}

/**************************************************************************/
/*!
    Calculates the altitude (in meters) from the specified atmospheric
    pressure (in hPa), and sea-level pressure (in hPa).

    @param  seaLevel      Sea-level pressure in hPa
    @param  atmospheric   Atmospheric pressure in hPa
*/
/**************************************************************************/
float BME280::readAltitude(float seaLevel)
{
  // Equation taken from BMP180 datasheet (page 16):
  //  http://www.adafruit.com/datasheets/BST-BMP180-DS000-09.pdf

  // Note that using the equation from wikipedia can give bad results
  // at high altitude.  See this thread for more information:
  //  http://forums.adafruit.com/viewtopic.php?f=22&t=58064

  float atmospheric = readPressure() / 100.0F;
  return 44330.0 * (1.0 - pow(atmospheric / seaLevel, 0.1903));
}


float BME280::estimateSealevel(float altitude)
{
    float atmospheric = readPressure() / 100.0F;
    return(atmospheric/pow(1-(altitude/44330.0),5.255));
}
