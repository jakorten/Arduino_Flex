/***************************************************
  This is a library for the FXAS21002C GYROSCOPE

  Designed specifically to work with the Adafruit FXAS21002C Breakout
  ----> https://www.adafruit.com/products/

  These sensors use I2C to communicate, 2 pins (I2C)
  are required to interface.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Kevin "KTOWN" Townsend for Adafruit Industries.
  BSD license, all text above must be included in any redistribution

  Changes by J.A. Korten to support SERCOM (TwoWire)
 ****************************************************/
#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include <Wire.h>
#include <limits.h>

#include "RP_FXAS21002C.h"
#include "wiring_private.h" // pinPeripheral() function

TwoWire myWire(&sercom2, 4, 3);

/***************************************************************************
 PRIVATE FUNCTIONS
 ***************************************************************************/

/**************************************************************************/
/*!
    @brief  Abstract away platform differences in Arduino wire library
*/
/**************************************************************************/
void RP_FXAS21002C::write8(byte reg, byte value)
{
  myWire.beginTransmission(FXAS21002C_ADDRESS);
  #if ARDUINO >= 100
    myWire.write((uint8_t)reg);
    myWire.write((uint8_t)value);
  #else
    myWire.send(reg);
    myWire.send(value);
  #endif
  myWire.endTransmission();
}

/**************************************************************************/
/*!
    @brief  Abstract away platform differences in Arduino wire library
*/
/**************************************************************************/
byte RP_FXAS21002C::read8(byte reg)
{
  byte value;

  myWire.beginTransmission((byte)FXAS21002C_ADDRESS);
  #if ARDUINO >= 100
    myWire.write((uint8_t)reg);
  #else
    myWire.send(reg);
  #endif
  if (myWire.endTransmission(false) != 0) return 0;
  myWire.requestFrom((byte)FXAS21002C_ADDRESS, (byte)1);
  #if ARDUINO >= 100
    value = myWire.read();
  #else
    value = myWire.receive();
  #endif

  return value;
}

/***************************************************************************
 CONSTRUCTOR
 ***************************************************************************/

/**************************************************************************/
/*!
    @brief  Instantiates a new RP_FXAS21002C class
*/
/**************************************************************************/
RP_FXAS21002C::RP_FXAS21002C(int32_t sensorID) {
  _sensorID = sensorID;
}

/***************************************************************************
 PUBLIC FUNCTIONS
 ***************************************************************************/

/**************************************************************************/
/*!
    @brief  Setups the HW
*/
/**************************************************************************/
bool RP_FXAS21002C::begin(gyroRange_t rng)
{
  /* Enable I2C */
  myWire.begin();
  pinPeripheral(4, PIO_SERCOM_ALT);
  pinPeripheral(3, PIO_SERCOM_ALT);

  /* Set the range the an appropriate value */
  _range = rng;

  /* Clear the raw sensor data */
  raw.x = 0;
  raw.y = 0;
  raw.z = 0;

  /* Make sure we have the correct chip ID since this checks
     for correct address and that the IC is properly connected */
  uint8_t id = read8(GYRO_REGISTER_WHO_AM_I);
  // Serial.print("WHO AM I? 0x"); Serial.println(id, HEX);
  if (id != FXAS21002C_ID)
  {
    return false;
  }

  /* Set CTRL_REG1 (0x13)
   ====================================================================
   BIT  Symbol    Description                                   Default
   ---  ------    --------------------------------------------- -------
     6  RESET     Reset device on 1                                   0
     5  ST        Self test enabled on 1                              0
   4:2  DR        Output data rate                                  000
                  000 = 800 Hz
                  001 = 400 Hz
                  010 = 200 Hz
                  011 = 100 Hz
                  100 = 50 Hz
                  101 = 25 Hz
                  110 = 12.5 Hz
                  111 = 12.5 Hz
     1  ACTIVE    Standby(0)/Active(1)                                0
     0  READY     Standby(0)/Ready(1)                                 0

  /* Reset then switch to active mode with 100Hz output */
  write8(GYRO_REGISTER_CTRL_REG1, 0x00);
  write8(GYRO_REGISTER_CTRL_REG1, (1<<6));
  write8(GYRO_REGISTER_CTRL_REG1, 0x0E);
  delay(100); // 60 ms + 1/ODR
  /* ------------------------------------------------------------------ */

  return true;
}

/**************************************************************************/
/*!
    @brief  Gets the most recent sensor event
*/
/**************************************************************************/
bool RP_FXAS21002C::getEvent(sensors_event_t* event)
{
  bool readingValid = false;

  /* Clear the event */
  memset(event, 0, sizeof(sensors_event_t));

  /* Clear the raw data placeholder */
  raw.x = 0;
  raw.y = 0;
  raw.z = 0;

  event->version   = sizeof(sensors_event_t);
  event->sensor_id = _sensorID;
  event->type      = SENSOR_TYPE_GYROSCOPE;
  event->timestamp = millis();

  /* Read 7 bytes from the sensor */
  myWire.beginTransmission((byte)FXAS21002C_ADDRESS);
  #if ARDUINO >= 100
    myWire.write(GYRO_REGISTER_STATUS | 0x80);
  #else
    myWire.send(GYRO_REGISTER_STATUS | 0x80);
  #endif
  myWire.endTransmission();
  myWire.requestFrom((byte)FXAS21002C_ADDRESS, (byte)7);

  #if ARDUINO >= 100
    uint8_t status = myWire.read();
    uint8_t xhi = myWire.read();
    uint8_t xlo = myWire.read();
    uint8_t yhi = myWire.read();
    uint8_t ylo = myWire.read();
    uint8_t zhi = myWire.read();
    uint8_t zlo = myWire.read();
  #else
    uint8_t status = myWire.receive();
    uint8_t xhi = myWire.receive();
    uint8_t xlo = myWire.receive();
    uint8_t yhi = myWire.receive();
    uint8_t ylo = myWire.receive();
    uint8_t zhi = myWire.receive();
    uint8_t zlo = myWire.receive();
  #endif

  /* Shift values to create properly formed integer */
  event->gyro.x = (int16_t)((xhi << 8) | xlo);
  event->gyro.y = (int16_t)((yhi << 8) | ylo);
  event->gyro.z = (int16_t)((zhi << 8) | zlo);

  /* Assign raw values in case someone needs them */
  raw.x = event->gyro.x;
  raw.y = event->gyro.y;
  raw.z = event->gyro.z;

  /* Compensate values depending on the resolution */
  switch(_range)
  {
    case GYRO_RANGE_250DPS:
      event->gyro.x *= GYRO_SENSITIVITY_250DPS;
      event->gyro.y *= GYRO_SENSITIVITY_250DPS;
      event->gyro.z *= GYRO_SENSITIVITY_250DPS;
      break;
    case GYRO_RANGE_500DPS:
      event->gyro.x *= GYRO_SENSITIVITY_500DPS;
      event->gyro.y *= GYRO_SENSITIVITY_500DPS;
      event->gyro.z *= GYRO_SENSITIVITY_500DPS;
      break;
    case GYRO_RANGE_1000DPS:
      event->gyro.x *= GYRO_SENSITIVITY_1000DPS;
      event->gyro.y *= GYRO_SENSITIVITY_1000DPS;
      event->gyro.z *= GYRO_SENSITIVITY_1000DPS;
      break;
    case GYRO_RANGE_2000DPS:
      event->gyro.x *= GYRO_SENSITIVITY_2000DPS;
      event->gyro.y *= GYRO_SENSITIVITY_2000DPS;
      event->gyro.z *= GYRO_SENSITIVITY_2000DPS;
      break;
  }

  /* Convert values to rad/s */
  event->gyro.x *= SENSORS_DPS_TO_RADS;
  event->gyro.y *= SENSORS_DPS_TO_RADS;
  event->gyro.z *= SENSORS_DPS_TO_RADS;

  return true;
}

/**************************************************************************/
/*!
    @brief  Gets the sensor_t data
*/
/**************************************************************************/
void  RP_FXAS21002C::getSensor(sensor_t* sensor)
{
  /* Clear the sensor_t object */
  memset(sensor, 0, sizeof(sensor_t));

  /* Insert the sensor name in the fixed length char array */
  strncpy (sensor->name, "FXAS21002C", sizeof(sensor->name) - 1);
  sensor->name[sizeof(sensor->name)- 1] = 0;
  sensor->version     = 1;
  sensor->sensor_id   = _sensorID;
  sensor->type        = SENSOR_TYPE_GYROSCOPE;
  sensor->min_delay   = 0;
  sensor->max_value   = (float)this->_range * SENSORS_DPS_TO_RADS;
  sensor->min_value   = (this->_range * -1.0) * SENSORS_DPS_TO_RADS;
  sensor->resolution  = 0.0F; // TBD
}
