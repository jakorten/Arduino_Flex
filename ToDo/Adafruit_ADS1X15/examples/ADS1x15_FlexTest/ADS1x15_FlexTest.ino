
#include <Wire.h>
#include "wiring_private.h" // pinPeripheral() function
#include <Adafruit_ADS1015_Flex.h>

TwoWire myWire(&sercom2, 4, 3);

// Adafruit_ADS1115_alt ads;     // Use this for the 16-bit version
//Adafruit_ADS1015_alt ads;     // Use this for the 12-bit version
Adafruit_ADS1015_Flex ads1 = Adafruit_ADS1015_Flex(&myWire);
Adafruit_ADS1015_Flex ads2 = Adafruit_ADS1015_Flex(&Wire);

// ToDo: Robotpatient library Touch checken...


bool adsOnBus1 = false;
bool adsOnBus2 = false;

float Voltage = 0.0;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(3000);

  myWire.begin();
  Wire.begin();

  pinPeripheral(4, PIO_SERCOM_ALT);
  pinPeripheral(3, PIO_SERCOM_ALT);

  delay(1000);
  Serial.println("Example for Adafruit_ADS1X15_Flex");
  // Assign pins 4 & 3 to SERCOM functionality

  //initADS1X15();

  scanDevices(0x48); // ADS1X15_ADDRESS (default address 0x48)

}

void loop() {
  // put your main code here, to run repeatedly:
  int16_t adc0;  // we read from the ADC, we have a sixteen bit integer as a result
  
  if (adsOnBus1) {
    Serial.println("Try to read... Bus 1");
    adc0 = ads1.readADC_SingleEnded(0);
  } 
  if (adsOnBus2) {
    Serial.println("Try to read... Bus 2");
    adc0 = ads2.readADC_SingleEnded(0);
  }
  Voltage = (adc0 * 0.1875)/1000;
  Serial.print("AIN0: "); 
  Serial.println(adc0);

  delay(2000);
}

void scanDevices(byte address)
{
  byte error;
  int nDevices;

  Serial.print("Scanning bus 1: PA08/PA09 on SERCOM2: ");
  myWire.beginTransmission(address);
  error = myWire.endTransmission();
  if (error == 0) {
    adsOnBus1 = true;
    Serial.println("ADS1x15 found on bus 1 (SERCOM2)");
  } else {
    adsOnBus1 = false;
    Serial.println("ADS1x15 not found on bus 1 (SERCOM2)");
  }

  Serial.print("Scanning bus 2: PA22/PA23 on SERCOM3: ");
  Wire.beginTransmission(address);
  error = Wire.endTransmission();

  if (error == 0) {
    adsOnBus2 = true;
    Serial.println("ADS1x15 found on bus 2 (SERCOM3)");
  } else {
    adsOnBus2 = false;
    Serial.println("ADS1x15 not found on bus 2 (SERCOM3)");
  }

}
