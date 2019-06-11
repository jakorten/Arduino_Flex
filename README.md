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
terminal.png

Enjoy. Comments / questions: feel free to reach out.
