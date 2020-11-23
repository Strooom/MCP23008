#include "MCP23008_SPI.h"
#include <Wire.h>        // the device can interface I2C or SPI. This driver only does I2C, so we include the I2C library

MCP23008::MCP23008(uint8_t theI2Caddress) : I2cAddress(theI2Caddress) {
}

void MCP23008::initialize() {
    Wire.begin();
}

void MCP23008::pinMode(uint8_t pin, uint8_t theMode) {
    if (pin <= 7) {
        uint8_t currentIOdir = read8(registerAddress::IODIR);        // read the current IODIR config
        uint8_t currentPullup = read8(registerAddress::GPPU);        // read the current GPPU config
        switch (theMode) {
            default:
            case INPUT:
                write8(registerAddress::IODIR, currentIOdir | (1 << pin));        // set the corresponding bit to '1' to set this pin as input
                break;

            case INPUT_PULLUP:
                write8(registerAddress::IODIR, currentIOdir | (1 << pin));        // set the corresponding bit to '1' to set this pin as input
                write8(registerAddress::GPPU, currentPullup | (1 << pin));        // set the corresponding bit to '1' to enable the pullup at this pin
                break;

            case OUTPUT:
                write8(registerAddress::IODIR, currentIOdir) & (~(1 << pin));        // clear corresponding bit to '0' to set this pin as output
                break;
        }
    }
}

void MCP23008::digitalWrite(uint8_t pin, uint8_t value) {
    if (pin <= 7) {
        uint8_t currentIOvalue = read8(registerAddress::GPIO);                    // read the current GPIO register
        if (value == HIGH) {                                                      //
            write8(registerAddress::GPIO, currentIOvalue | (1 << pin));           // set the corresponding bit to '1'
        } else {                                                                  //
            write8(registerAddress::GPIO, currentIOvalue) & (~(1 << pin));        // clear corresponding bit to '0'
        }
    }
}

uint8_t MCP23008::digitalRead(uint8_t pin) {
    if (pin <= 7) {                                              // we can only read pin 0 .. 7
        return (read8(registerAddress::GPIO) >> p) & 0x1;        // read the register, shift the bits and mask-out everything else
    } else {
        return 0;
    }
}

uint8_t MCP23008::read8(uint8_t address) {
    if (address <= registerAddress::OLAT) {
        Wire.beginTransmission(I2Caddress);        // start an I2C communication to device with this address
        Wire.write((byte)address);                 // write registerAddress to read
        Wire.endTransmission();                    // end writing...
        Wire.requestFrom(I2Caddresss, 1);          // ...switch to reading
        return Wire.read();                        // read 1 byte from I2C
    } else {
        return 0;
    }
}

void MCP23008::write8(uint8_t address, uint8_t value) {
    if (address <= registerAddress::OLAT) {
        Wire.beginTransmission(I2Caddress);        // start an I2C communication to device with this address
        Wire.write((byte)address);                 // write registerAddress to read
        Wire.write((byte)value);                   // then write value
        Wire.endTransmission();                    // ready
    }
}
