#include "MCP23008.h"

#include <Wire.h>           // the device can interface I2C or SPI. This driver only does I2C, so we include the I2C library
#include <Arduino.h>        // for definition of INPUT, OUTPUT, etc

MCP23008::MCP23008(uint8_t theI2Caddress) : I2Caddress(theI2Caddress) {
}

void MCP23008::initialize() {
    Wire.begin();
    mcuCopyOfRegisters[static_cast<uint8_t>(registerAddress::IODIR)]   = 0xFF;
    mcuCopyOfRegisters[static_cast<uint8_t>(registerAddress::IPOL)]    = 0x00;
    mcuCopyOfRegisters[static_cast<uint8_t>(registerAddress::GPINTEN)] = 0x00;
    mcuCopyOfRegisters[static_cast<uint8_t>(registerAddress::DEFVAL)]  = 0x00;
    mcuCopyOfRegisters[static_cast<uint8_t>(registerAddress::INTCON)]  = 0x00;
    mcuCopyOfRegisters[static_cast<uint8_t>(registerAddress::IOCON)]   = 0x00;
    mcuCopyOfRegisters[static_cast<uint8_t>(registerAddress::GPPU)]    = 0x00;
    mcuCopyOfRegisters[static_cast<uint8_t>(registerAddress::INTF)]    = 0x00;
    mcuCopyOfRegisters[static_cast<uint8_t>(registerAddress::INTCAP)]  = 0x00;
    mcuCopyOfRegisters[static_cast<uint8_t>(registerAddress::GPIO)]    = 0x00;
    mcuCopyOfRegisters[static_cast<uint8_t>(registerAddress::OLAT)]    = 0x00;
    refresh();
}

uint8_t MCP23008::readRegister(registerAddress theRegister) const {
    Wire.beginTransmission(I2Caddress);                   // start an I2C communication to device with this address
    Wire.write(static_cast<uint8_t>(theRegister));        // write registerAddress to read
    Wire.endTransmission();                               // end writing...
    Wire.requestFrom(I2Caddress, 1);                      // ...switch to reading
    return static_cast<uint8_t>(Wire.read());             // read 1 byte from I2C
}

void MCP23008::writeHardwareRegister(registerAddress theRegister, uint8_t theValue) {
    Wire.beginTransmission(I2Caddress);                   // start an I2C communication to device with this address
    Wire.write(static_cast<uint8_t>(theRegister));        // write registerAddress to write
    Wire.write(theValue);                                 // then write value
    Wire.endTransmission();                               // ready
}

void MCP23008::writeRegister(registerAddress address, uint8_t value) {
    mcuCopyOfRegisters[static_cast<uint8_t>(address)] = value;
    writeHardwareRegister(address, value);
}

void MCP23008::refresh() {
    for (uint8_t i = 0; i < nmbrOfRegisters; i++) {
        writeHardwareRegister(static_cast<registerAddress>(i), mcuCopyOfRegisters[i]);
    }
}

uint8_t MCP23008::getI2cAddress() const {
    return I2Caddress;
}

// void MCP23008::pinMode(uint8_t pin, uint8_t theMode) {
//     if (pin <= 7) {
//         uint8_t currentIOdir  = read8(registerAddress::IODIR);        // read the current IODIR config
//         uint8_t currentPullup = read8(registerAddress::GPPU);         // read the current GPPU config
//         switch (theMode) {
//             default:
//             case INPUT:
//                 write8(registerAddress::IODIR, currentIOdir | (1 << pin));        // set the corresponding bit to '1' to set this pin as input
//                 break;

//             case INPUT_PULLUP:
//                 write8(registerAddress::IODIR, currentIOdir | (1 << pin));        // set the corresponding bit to '1' to set this pin as input
//                 write8(registerAddress::GPPU, currentPullup | (1 << pin));        // set the corresponding bit to '1' to enable the pullup at this pin
//                 break;

//             case OUTPUT:
//                 write8(registerAddress::IODIR, (currentIOdir & (~(1 << pin))));        // clear corresponding bit to '0' to set this pin as output
//                 break;
//         }
//     }
// }

// void MCP23008::digitalWrite(uint8_t pin, uint8_t value) {
//     if (pin <= 7) {
//         uint8_t currentIOvalue = read8(registerAddress::GPIO);                      // read the current GPIO register
//         if (value == 1) {                                                           //
//             write8(registerAddress::GPIO, currentIOvalue | (1 << pin));             // set the corresponding bit to '1'
//         } else {                                                                    //
//             write8(registerAddress::GPIO, (currentIOvalue & (~(1 << pin))));        // clear corresponding bit to '0'
//         }
//     }
// }

// uint8_t MCP23008::digitalRead(uint8_t pin) {
//     if (pin <= 7) {                                                // we can only read pin 0 .. 7
//         return (read8(registerAddress::GPIO) >> pin) & 0x1;        // read the register, shift the bits and mask-out everything else
//     } else {
//         return 0;
//     }
// }
