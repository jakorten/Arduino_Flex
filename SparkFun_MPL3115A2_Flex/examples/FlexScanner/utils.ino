char pastring[10];
char tmpstring[10];

float pressure;
float temperature;

int ipress;
int itemp;

float lastPressure = 0.0;

boolean use_hPa = true; // then use millibar
//boolean mpl3115a2_available = false;

unsigned long tempPressureSensorTimer = 0;
boolean isInit_ps = true;

float readTemperatureFromPressureSensor(MPL3115A2_Flex *sensor) {
  return pressure = sensor->readTemp();
}

float readPressureSensor(MPL3115A2_Flex *sensor, bool hPa) {
  if (hPa) {
    return pressure = (sensor->readPressure() / 100); // return in hPa
  } else {
    return pressure = (sensor->readPressure() / 10000); // convert to mb
  }
}
