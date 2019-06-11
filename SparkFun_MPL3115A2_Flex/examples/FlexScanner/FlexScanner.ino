// -------------------------------------------------------
// HTU21 Example
// i2c scanner on SERCOM2 / SERCOM3 busses
// SERCOM3 connects to sensor on the hat / board
// SERCOM2 connects to (external) sensor in sensor hut
// (if connected) via RAIN sensor input.
//
// Based on previous work by Sparkfun.
//
// Should find MPL3115A2 sensor(s) on address(es): 0x60
// J.A. Korten - June 10, 2019
//
// -------------------------------------------------------

#include <Wire.h>
#include "wiring_private.h" // pinPeripheral() function
#include "SparkFunMPL3115A2_Flex.h"

#define sensorAddress 0x60
#define serialSpeed 115200

TwoWire myWire(&sercom2, 4, 3);
bool mplOnBus1 = false;
bool mplOnBus2 = false;
MPL3115A2_Flex mpl1 = MPL3115A2_Flex(&myWire);
MPL3115A2_Flex mpl2 = MPL3115A2_Flex(&Wire);

boolean mpl3115a2_available = false;

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

  if (mplOnBus1) {
    mplOnBus1 = mpl1.init();
  }
  if (mplOnBus2) {
    mplOnBus2 = mpl2.init();
  }

}


void scanDevices(byte address)
{
  byte error;
  int nDevices;

  Serial.print("Scanning bus 1: PA08/PA09 on SERCOM2: ");
  if (reportDevice(address, &myWire)) {
    mplOnBus1 = true;
    Serial.println("MPL3115A2 found on bus 1 (SERCOM2)");
  } else {
    mplOnBus1 = false;
    Serial.println("MPL3115A2 not found on bus 1 (SERCOM2)");
  }

  Serial.print("Scanning bus 2: PA22/PA23 on SERCOM3: ");
  if (reportDevice(address, &Wire)) {
    mplOnBus2 = true;
    Serial.println("MPL3115A2 found on bus 2 (SERCOM3)");
  } else {
    mplOnBus2 = false;
    Serial.println("MPL3115A2 not found on bus 2 (SERCOM3)");
  }

}

bool reportDevice(byte address, TwoWire *wire) {
  wire->beginTransmission(address);
  int error = wire->endTransmission();
  return (error == 0);
}

void loop() {
  if (mplOnBus1) {
    reportSensor(&mpl1, 1, " (hut).");
  }
  if (mplOnBus2) {
    reportSensor(&mpl2, 2, " (board).");
  }

  delay(500);
}

void reportSensor(MPL3115A2_Flex *sensor, byte bus, String location) {
   Serial.print("MPL on Bus ");
   Serial.print(bus);
   Serial.println(location);
   Serial.print("Pressure: "); Serial.print(readPressureSensor(sensor, true));
   Serial.print("hPa");
   // you could also get the temperate from the sensor (readTemp / readTempF)
   Serial.println();
}
