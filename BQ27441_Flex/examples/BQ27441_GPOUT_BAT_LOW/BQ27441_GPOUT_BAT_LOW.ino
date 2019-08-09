/******************************************************************************
  BQ27441_GPOUT_BAT_LOW
  BQ27441 Library GPOUT Example - Battery low Mode
  Jim Lindblom @ SparkFun Electronics
  May 9, 2016
  https://github.com/sparkfun/SparkFun_BQ27441_Arduino_Library

  Demonstrates how to use the BQ27441's BAT_LOW function on GPOUT. In this mode
  GPOUT will become active whenever the battery goes below a set threshold.

  In addition to I2C, connect GPOUT to pin 2 of your Arduino.

  After uploading, open up the serial monitor to 115200 baud to view your
  battery's stats. They should only print when the percentage goes up or down
  by 1%.

  Hardware Resources:
  - Arduino Development Board
  - SparkFun Battery Babysitter

  Development environment specifics:
  Arduino 1.6.7
  SparkFun Battery Babysitter v1.0
  Arduino Uno (any 'duino should do)

  Flex Library Modifications V1.0 with SERCOM (SAMD21/51)
  RobotPatient Simulators 2019
  Jannes Bloemert
  Johan Korten
******************************************************************************/
#include <BQ27441_Flex.h>
#include <Wire.h>

// Set BATTERY_CAPACITY to the design capacity of your battery.
const unsigned int BATTERY_CAPACITY = 850; // e.g. 850mAh battery

const byte SOCI_SET = 15; // Interrupt set threshold at 20%
const byte SOCI_CLR = 20; // Interrupt clear threshold at 25%
const byte SOCF_SET = 5; // Final threshold set at 5%
const byte SOCF_CLR = 10; // Final threshold clear at 10%

// Arduino pin connected to BQ27441's GPOUT pin
const int GPOUT_PIN = 2;

TwoWire myWire(&sercom3, 20, 21);
BQ27441_Flex _lipoGauge = BQ27441_Flex(&myWire);

#if defined(ARDUINO_SAMD_ZERO) && !defined(Serial)
// Resolves the need to use SerialUSB instead of Serial
// for RobotPatient SAMD21 firmware...
  #define Serial SerialUSB
#endif

void setupBQ27441(void)
{
  pinMode(GPOUT_PIN, INPUT_PULLUP); // Set the GPOUT pin as an input w/ pullup

  // Use _lipoGauge.begin() to initialize the BQ27441-G1A and confirm that it's
  // connected and communicating.
  if (!_lipoGauge.begin()) // begin() will return true if communication is successful
  {
    // If communication fails, print an error message and loop forever.
    Serial.println("Error: Unable to communicate with BQ27441.");
    Serial.println("  Check wiring and try again.");
    Serial.println("  (Battery must be plugged into Battery Babysitter!)");
    while (1) ;
  }
  Serial.println("Connected to BQ27441!");

  // In this example, we'll manually enter and exit config mode. By controlling
  // config mode manually, you can set the chip up faster -- completing all of
  // the set up in a single config mode sweep.
  _lipoGauge.enterConfig(); // To configure the values below, you must be in config mode
  _lipoGauge.setCapacity(BATTERY_CAPACITY); // Set the battery capacity
  _lipoGauge.setGPOUTPolarity(LOW); // Set GPOUT to active-high
  _lipoGauge.setGPOUTFunction(BAT_LOW); // Set GPOUT to BAT_LOW mode
  _lipoGauge.setSOC1Thresholds(SOCI_SET, SOCI_CLR); // Set SOCI set and clear thresholds
  _lipoGauge.setSOCFThresholds(SOCF_SET, SOCF_CLR); // Set SOCF set and clear thresholds
  _lipoGauge.exitConfig();

  // Use _lipoGauge.GPOUTPolarity to read from the chip and confirm the changes
  if (_lipoGauge.GPOUTPolarity())
    Serial.println("GPOUT set to active-HIGH");
  else
    Serial.println("GPOUT set to active-LOW");

  // Use _lipoGauge.GPOUTFunction to confirm the functionality of GPOUT
  if (_lipoGauge.GPOUTFunction())
    Serial.println("GPOUT function set to BAT_LOW");
  else
    Serial.println("GPOUT function set to SOC_INT");

  // Read the set and clear thresholds to make sure they were set correctly
  Serial.println("SOC1 Set Threshold: " + String(_lipoGauge.SOC1SetThreshold()));
  Serial.println("SOC1 Clear Threshold: " + String(_lipoGauge.SOC1ClearThreshold()));
  Serial.println("SOCF Set Threshold: " + String(_lipoGauge.SOCFSetThreshold()));
  Serial.println("SOCF Clear Threshold: " + String(_lipoGauge.SOCFClearThreshold()));
}

void printBatteryStats()
{
  // Read battery stats from the BQ27441-G1A
  unsigned int soc = _lipoGauge.soc();  // Read state-of-charge (%)
  unsigned int volts = _lipoGauge.voltage(); // Read battery voltage (mV)
  int current = _lipoGauge.current(AVG); // Read average current (mA)
  unsigned int fullCapacity = _lipoGauge.capacity(FULL); // Read full capacity (mAh)
  unsigned int capacity = _lipoGauge.capacity(REMAIN); // Read remaining capacity (mAh)
  int power = _lipoGauge.power(); // Read average power draw (mW)
  int health = _lipoGauge.soh(); // Read state-of-health (%)

  // Assemble a string to print
  String toPrint = String(soc) + "% | ";
  toPrint += String(volts) + " mV | ";
  toPrint += String(current) + " mA | ";
  toPrint += String(capacity) + " / ";
  toPrint += String(fullCapacity) + " mAh | ";
  toPrint += String(power) + " mW | ";
  toPrint += String(health) + "%";
  toPrint += " | 0x" + String(_lipoGauge.flags(), HEX);

  // Print the string
  Serial.println(toPrint);
}

void setup()
{
  Serial.begin(115200);
  myWire.begin();
  setupBQ27441();
}

void loop()
{
  printBatteryStats();

  // If the GPOUT interrupt is active (low)
  if (!digitalRead(GPOUT_PIN))
  {
    // Check which of the flags triggered the interrupt
    if (_lipoGauge.socfFlag())
      Serial.println("<!-- WARNING: Battery Dangerously low -->");
    else if (_lipoGauge.socFlag())
      Serial.println("<!-- WARNING: Battery Low -->");
  }
  delay(1000);
}
