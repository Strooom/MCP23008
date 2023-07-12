#include "MCP23008.h"

#include <Wire.h>           // the device can interface I2C or SPI. This driver only does I2C, so we include the I2C library
#include <Arduino.h>        // for definition of INPUT, OUTPUT, etc

void MCP23008::discoverI2cAddress(uint8_t startAddress) {
    for (uint8_t candidateI2cAddress = startAddress; candidateI2cAddress <= 0x27; candidateI2cAddress++) {        // scan all possible I2C addresses for this device
        Wire.beginTransmission(candidateI2cAddress);
        if (Wire.endTransmission() == 0) {
            I2Caddress = candidateI2cAddress;
            return;
        }
    }
    I2Caddress = 0;
    return;
}

bool MCP23008::isPresent() const {
    return (I2Caddress != 0);
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
    Wire.beginTransmission(I2Caddress);                       // start an I2C communication to device with this address
    Wire.write(static_cast<uint8_t>(theRegister));            // write registerAddress to read
    Wire.endTransmission();                                   // end writing...
    Wire.requestFrom(static_cast<int>(I2Caddress), 1);        // ...switch to reading
    return static_cast<uint8_t>(Wire.read());                 // read 1 byte from I2C
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
        registerAddress theAddress = static_cast<registerAddress>(i);
        switch (theAddress) {
            case registerAddress::INTF:
            case registerAddress::INTCAP:
            case registerAddress::GPIO:
                // Skip as these are read-only registers, so writing them makes no sense
                // Writing GPIO actually writes OLAT
                break;

            default:
                writeHardwareRegister(static_cast<registerAddress>(i), mcuCopyOfRegisters[i]);
                break;
        }
    }
}

void MCP23008::refresh(registerAddress address) {
    writeHardwareRegister(address, mcuCopyOfRegisters[static_cast<int>(address)]);
}

uint8_t MCP23008::getI2cAddress() const {
    return I2Caddress;
}

bool MCP23008::verify() const {
    bool result{true};
    for (uint8_t i = 0; i < nmbrOfRegisters; i++) {
        registerAddress theAddress = static_cast<registerAddress>(i);
        switch (theAddress) {
            default:
                break;

                // In a verify operation, we only check the following HW registers
            case registerAddress::OLAT:
            case registerAddress::IODIR:
            case registerAddress::IPOL:
                if (readRegister(static_cast<registerAddress>(i)) != mcuCopyOfRegisters[i]) {
                    result = false;
                }
                break;
        }
    }
    return result;
}

void MCP23008::pinMode(uint8_t pin, uint8_t theMode) {
    if (pin <= 7) {
        uint8_t currentIOdir  = readRegister(registerAddress::IODIR);        // read the current IODIR config
        uint8_t currentPullup = readRegister(registerAddress::GPPU);         // read the current GPPU config
        switch (theMode) {
            default:
            case INPUT:
                writeRegister(registerAddress::IODIR, currentIOdir | (1 << pin));             // set the corresponding bit to '1' to set this pin as input
                writeRegister(registerAddress::GPPU, (currentPullup & (~(1 << pin))));        // clear the corresponding bit to '0' to disable the pullup at this pin
                break;

            case INPUT_PULLUP:
                writeRegister(registerAddress::IODIR, currentIOdir | (1 << pin));        // set the corresponding bit to '1' to set this pin as input
                writeRegister(registerAddress::GPPU, currentPullup | (1 << pin));        // set the corresponding bit to '1' to enable the pullup at this pin
                break;

            case OUTPUT:
                writeRegister(registerAddress::GPPU, (currentPullup & (~(1 << pin))));        // clear the corresponding bit to '0' to disable the pullup at this pin
                writeRegister(registerAddress::IODIR, (currentIOdir & (~(1 << pin))));        // clear corresponding bit to '0' to set this pin as output
                break;
        }
    }
}

void MCP23008::digitalWrite(uint8_t pin, uint8_t value) {
    if (pin <= 7) {
        uint8_t currentIOvalue = readRegister(registerAddress::OLAT);                      // read the current GPIO-OUTPUT register = OLAT ! (reading GPIO reads the input pins)
        if (value == 1) {                                                                  //
            writeRegister(registerAddress::OLAT, currentIOvalue | (1 << pin));             // set the corresponding bit to '1'
        } else {                                                                           //
            writeRegister(registerAddress::OLAT, (currentIOvalue & (~(1 << pin))));        // clear corresponding bit to '0'
        }
    }
}

uint8_t MCP23008::digitalRead(uint8_t pin) const {
    if (pin <= 7) {                                                       // we can only read pin 0 .. 7
        return (readRegister(registerAddress::GPIO) >> pin) & 0x1;        // read the register, shift the bits and mask-out everything else
    } else {
        return 0;
    }
}
