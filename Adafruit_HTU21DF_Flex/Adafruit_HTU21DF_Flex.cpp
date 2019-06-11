/***************************************************
  This is a library for the HTU21DF Humidity & Temp Sensor

  Designed specifically to work with the HTU21DF sensor from Adafruit
  ----> https://www.adafruit.com/products/1899

  These displays use I2C to communicate, 2 pins are required to
  interface
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution

  Flexible extensions J.A. Korten 2019
  version for SERCOM Wire
 ****************************************************/

#ifndef _HTU21DF_FLEX
#define _HTU21DF_FLEX
#include "Adafruit_HTU21DF_Flex.h"
#endif

#if defined(__AVR__)
#include <util/delay.h>
#endif


Adafruit_HTU21DF_Flex::Adafruit_HTU21DF_Flex(TwoWire *wire) {
   this->_wire = wire;
}


boolean Adafruit_HTU21DF_Flex::begin(void) {
  //_wire->begin();

  reset();

  _wire->beginTransmission(HTU21DF_I2CADDR);
  _wire->write(HTU21DF_READREG);
  _wire->endTransmission();
  _wire->requestFrom(HTU21DF_I2CADDR, 1);
  return (_wire->read() == 0x2); // after reset should be 0x2
}

void Adafruit_HTU21DF_Flex::reset(void) {
  _wire->beginTransmission(HTU21DF_I2CADDR);
  _wire->write(HTU21DF_RESET);
  _wire->endTransmission();
  delay(15);
}


float Adafruit_HTU21DF_Flex::readTemperature(void) {

  // OK lets ready!
  _wire->beginTransmission(HTU21DF_I2CADDR);
  _wire->write(HTU21DF_READTEMP);
  _wire->endTransmission();

  delay(50); // add delay between request and actual read!

  _wire->requestFrom(HTU21DF_I2CADDR, 3);
  while (!_wire->available()) {}

  uint16_t t = _wire->read();
  t <<= 8;
  t |= _wire->read();

  uint8_t crc = _wire->read();

  float temp = t;
  temp *= 175.72;
  temp /= 65536;
  temp -= 46.85;

  return temp;
}


float Adafruit_HTU21DF_Flex::readHumidity(void) {
  // OK lets ready!
  _wire->beginTransmission(HTU21DF_I2CADDR);
  _wire->write(HTU21DF_READHUM);
  _wire->endTransmission();

  delay(50); // add delay between request and actual read!

  _wire->requestFrom(HTU21DF_I2CADDR, 3);
  while (!_wire->available()) {}

  uint16_t h = _wire->read();
  h <<= 8;
  h |= _wire->read();

  uint8_t crc = _wire->read();

  float hum = h;
  hum *= 125;
  hum /= 65536;
  hum -= 6;

  return hum;
}



/*********************************************************************/
