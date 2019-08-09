/******************************************************************************
BQ27441_Basic
BQ27441 Library Basic Example
Jim Lindblom @ SparkFun Electronics
May 9, 2016
https://github.com/sparkfun/SparkFun_BQ27441_Arduino_Library

Demonstrates how to set up the BQ27441 and read state-of-charge (soc),
battery voltage, average current, remaining capacity, average power, and
state-of-health (soh).

After uploading, open up the serial monitor to 115200 baud to view your
battery's stats.

Hardware Resources:
- Arduino Development Board
- SparkFun Battery Babysitter

Development environment specifics:
Arduino 1.6.7
SparkFun Battery Babysitter v1.0
Arduino Uno (any 'duino should do)

Flex Library Modifications V1.0
RobotPatient Simulators 2019
Jannes Bloemert
Johan Korten
******************************************************************************/
#include <BQ27441_Flex.h>
#include <Wire.h>

// Set BATTERY_CAPACITY to the design capacity of your battery.
const unsigned int BATTERY_CAPACITY = 850; // e.g. 850mAh battery

TwoWire myWire(&sercom3, 20, 21);
BQ27441_Flex _lipoGauge = BQ27441_Flex(&myWire);

#if defined(ARDUINO_SAMD_ZERO) && !defined(Serial)
// Resolves the need to use SerialUSB instead of Serial
// for RobotPatient SAMD21 firmware...
  #define Serial SerialUSB
#endif

void setupBQ27441(void)
{
  // Use lipo.begin() to initialize the BQ27441-G1A and confirm that it's
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

  // Uset lipo.setCapacity(BATTERY_CAPACITY) to set the design capacity
  // of your battery.
  _lipoGauge.setCapacity(BATTERY_CAPACITY);
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

  // Now print out those values:
  String toPrint = String(soc) + "% | ";
  toPrint += String(volts) + " mV | ";
  toPrint += String(current) + " mA | ";
  toPrint += String(capacity) + " / ";
  toPrint += String(fullCapacity) + " mAh | ";
  toPrint += String(power) + " mW | ";
  toPrint += String(health) + "%";

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
  delay(1000);
}
