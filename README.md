# Arduino Flex Libraries

Arduino is great for multiple reasons but one includes the availability of libraries
from companies like Adafruit and Sparkfun for popular sensors.
They do a great job by taking away a lot of tedious work for developers.

They however do not seem to program their libraries in such a way that
they are flexible to use. This is problematic since it severely limits it's
usability especially. Problems include:
- i2c-bus issues (because libraries might issue multiple Wire.begin commands)
- not being able to combine sensors
- not being able to use the sensor on a different bus

We try to convert their libraries to a more flexible version that uses
dependency injection of TwoWire (for now we only converted i2c libraries).

See the output of using two HTU21DF sensors (at two different i2c busses using SAMD21):
![alt text](https://github.com/jakorten/Arduino_Flex/blob/master/images/terminal.png "Example Arduino terminal output.")

## Notes
Those libraries often do a begin on Wire inside the library. These lines were commented out. For convenience a init was implemented. Some libraries do need sensor initialization.
We also linked "begin" to this init method which calls init.
Always first issue a Wire (or custom e.g. myWire) begin, only then call the library method to avoid bus issues.

Warning: some libraries (including HTU21DF) do not seem to like operating when Wire is used as a slave (e.g. Wire.begin(mySlaveAddress)): in that case the bus hangs.

## Supported Libraries:
* HTU21DF    - Based on Adafruit Library
* FXAS21002C - Based on Adafruit Library
* FXOS8700   - Based on Adafruit Library
* MPL3115A2  - Based on Sparkfun Library

## Other Information:
Enjoy. Comments / questions: feel free to reach out.
