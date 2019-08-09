/******************************************************************************
  BQ27441_GPOUT_SOC_INT
  BQ27441 Library GPOUT Example - State-of-charge interval mode
  Jim Lindblom @ SparkFun Electronics
  May 9, 2016
  https://github.com/sparkfun/SparkFun_BQ27441_Arduino_Library


  Demonstrates how to use the BQ27441's SOC_INT function on GPOUT. In this mode
  GPOUT will pulse every time the state-of-charge (soc) goes up or down by a
  set percentage integer.

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
******************************************************************************/
#include <BQ27441_Flex.h>
#include <Wire.h>

// Set BATTERY_CAPACITY to the design capacity of your battery.
const unsigned int BATTERY_CAPACITY = 850; // e.g. 850mAh battery

// Arduino pin connected to BQ27441's GPOUT pin
const int GPOUT_PIN = 17; // originally to D2

// Set the integer percentage change that triggers an interrupt
const int PERCENTAGE_INTERVAL = 1;

TwoWire myWire(&sercom3, 20, 21);
BQ27441_Flex _lipoGauge = BQ27441_Flex(&myWire);

#if defined(ARDUINO_SAMD_ZERO) && !defined(Serial)
  // Resolves the need to use SerialUSB instead of Serial
  // for RobotPatient SAMD21 firmware...
  #define Serial SerialUSB
#endif

void setupBQ27441(void)
{
  //pinMode(GPOUT_PIN, INPUT_PULLUP); // Set the GPOUT pin as an input w/ pullup
  pinMode(GPOUT_PIN, INPUT);
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
  _lipoGauge.setGPOUTPolarity(LOW); // Set GPOUT to active-low
  _lipoGauge.setGPOUTFunction(SOC_INT); // Set GPOUT to SOC_INT mode
  _lipoGauge.setSOCIDelta(PERCENTAGE_INTERVAL); // Set percentage change integer
  _lipoGauge.exitConfig(); // Exit config mode to save changes

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

  // Read _lipoGauge.sociDelta() to confirm the integer change value
  Serial.println("SOCI Delta: " + String(_lipoGauge.sociDelta()));
  Serial.println();

  // Use _lipoGauge.pulseGPOUT() to trigger a pulse on GPOUT. This only works
  // in SOC_INT mode.
  Serial.println("Testing GPOUT Pulse");
  _lipoGauge.pulseGPOUT();
  int timeout = 10000; // The pulse can take a while to occur. Set max to 10s
  while ((digitalRead(GPOUT_PIN)) && timeout--)
    delay(1); // Wait for GPOUT to go high, or timeout to occur
  if (timeout > 0)
  {
    // If GPOUT pulsed, print success message.
    Serial.print("GPOUT test successful!");
    Serial.println("(" + String(10000 - timeout) + ")");
    Serial.print("GPOUT will pulse whenever the SoC ");
    Serial.println("value changes by SOCI delta.");
    Serial.print("Or when the battery changes from");
    Serial.println(" charging to discharging, or vice-versa.");
    Serial.println();
  }
  else
  {
    // If GPOUT didn't pulse, something went wrong. Print error message
    // and loop forever.
    Serial.println("GPOUT didn't pulse.");
    Serial.print("Make sure it's connected to pin ");
    Serial.print(GPOUT_PIN);
    Serial.println(" and reset.");
    while (1) ;
  }
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
  String toPrint = "[" + String(millis() / 1000) + "] ";
  toPrint += String(soc) + "% | ";
  toPrint += String(volts) + " mV | ";
  toPrint += String(current) + " mA | ";
  toPrint += String(capacity) + " / ";
  toPrint += String(fullCapacity) + " mAh | ";
  toPrint += String(power) + " mW | ";
  toPrint += String(health) + "%";

  // Print the string
  Serial.println(toPrint);
}

void setup()
{
  Serial.begin(115200);
  myWire.begin();
  setupBQ27441();
  delay(2000);
  Serial.println("GPOUT_PIN example");
}

void loop()
{
  // The interrupt is set to active-low. If the GPOUT pin goes low...
  if (digitalRead(GPOUT_PIN) == LOW)
  {
    // ...SOC_INT occurred. Print battery stats.
    printBatteryStats();
  } else {
    //Serial.println("GPOUT_PIN == HIGH");
  }
}
