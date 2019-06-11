// -------------------------------------------------------
// HTU21 Example
// i2c scanner on SERCOM2 / SERCOM3 busses
// SERCOM3 connects to sensor on the hat / board
// SERCOM2 connects to (external) sensor in sensor hut
// (if connected) via RAIN sensor input.
//
// Should find HTU21 sensor(s) on address(es): 0x40
// J.A. Korten - June 10, 2019
//
// -------------------------------------------------------

#include <Wire.h>
#include "wiring_private.h" // pinPeripheral() function
#include "Adafruit_HTU21DF_Flex.h"

#define sensorAddress 0x40
#define serialSpeed 115200

TwoWire myWire(&sercom2, 4, 3);
bool htuOnBus1 = false;
bool htuOnBus2 = false;
Adafruit_HTU21DF_Flex htu1 = Adafruit_HTU21DF_Flex(&myWire);
Adafruit_HTU21DF_Flex htu2 = Adafruit_HTU21DF_Flex(&Wire);

void setup()
{
  Serial.begin(serialSpeed);

  myWire.begin(); // master SERCOM 2
  Wire.begin(); // master SERCOM 3

  // Assign pins 4 & 3 to SERCOM functionality
  pinPeripheral(4, PIO_SERCOM_ALT);
  pinPeripheral(3, PIO_SERCOM_ALT);

  delay(2500); // Wait for Serial...


  Serial.println("Scans primary (Sensors/Extra) and secondary (RAIN/HMI) busses...");
  delay(1000);

  scanDevices(sensorAddress);

  if (htuOnBus1) {
    htu1.begin();
  }
  if (htuOnBus2) {
    htu2.begin();
  }
}


void scanDevices(byte address)
{
  byte error;
  int nDevices;

  Serial.print("Scanning bus 1: PA08/PA09 on SERCOM2: ");
  myWire.beginTransmission(address);
  error = myWire.endTransmission();
  if (error == 0) {
    htuOnBus1 = true;
    Serial.println("HTU21DF found on bus 1 (SERCOM2)");
  } else {
    htuOnBus1 = false;
    Serial.println("HTU21DF not found on bus 1 (SERCOM2)");
  }

  Serial.print("Scanning bus 2: PA22/PA23 on SERCOM3: ");
  Wire.beginTransmission(address);
  error = Wire.endTransmission();

  if (error == 0) {
    htuOnBus2 = true;
    Serial.println("HTU21DF found on bus 2 (SERCOM3)");
  } else {
    htuOnBus2 = false;
    Serial.println("HTU21DF not found on bus 2 (SERCOM3)");
  }

}

void loop() {
  if (htuOnBus1) {
    reportSensor(&htu1, 1, " (hut).");
  }
  if (htuOnBus2) {
    reportSensor(&htu2, 2, " (board).");
  }

  delay(500);
}

void reportSensor(Adafruit_HTU21DF_Flex *sensor, byte bus, String location) {
   Serial.print("HTU on Bus ");
   Serial.print(bus);
   Serial.println(location);
   Serial.print("Temp: "); Serial.print(sensor->readTemperature());
   Serial.print("\t\tHum: "); Serial.println(sensor->readHumidity());
   Serial.println();
}
