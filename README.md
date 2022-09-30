[![Arduino Lint](https://github.com/strooom/MCP23008/workflows/Arduino%20Library%20Checks/badge.svg)](https://github.com/arduino/arduino-lint-action)

# MCP23008
Library for driving the MCP23008 Port Extender - Arduino framework

Q : What is different from the Adafruit library ?

A : We noticed that under certain circumstances, the registers of the MCP23008 can get corrupt, most likely from EM interference or radiation. The assumption to just configure the device, and then forget about it (for weeks, months, years) just doesn't work under such long timeframes. So I added a shadow copy of all registers in RAM. Rather than just writing the register in the device, we write the shadow copy first, then refresh the registers of the device. There is also a verify() function to compare both copies and detect how often the corruption occurs. Other than that, the library is API compatibel, also unit tested, and written in an easier to understand C++ style. Enjoy