#pragma once
#include <stdint.h>

class MCP23008 {
  public:
    void discoverI2cAddress(uint8_t startAddress = 0x20);        // scans I2C addresses 0x20 to 0x27 to detect an MCP23008 and stores the found address inside the object. To detect multiple MCP23008, first discover with no startAddress parameter (taking default 0x20), then repeat with startAddress = found address + 1, until not found or found on 0x27
    bool isPresent() const;                                      // returns true if the device is present on the I2C bus
    void initialize();                                           // device is ready for use after powerOn reset. This resets all settings to powerOn reset defaults.

    uint8_t getI2cAddress() const;
    void pinMode(uint8_t pin, uint8_t theMode);           // Arduino-style configuration of each individual IO
    void digitalWrite(uint8_t pin, uint8_t value);        // Arduino-style writing of a HIGH or LOW to an individual IO
    uint8_t digitalRead(uint8_t pin) const;               // Arduino-style reading an individual IO

    enum class registerAddress : uint8_t {                // definition of all the registers this device has
        IODIR   = 0x00,                                   // I/O Direction
        IPOL    = 0x01,                                   // Input Polarity
        GPINTEN = 0x02,                                   // Interrupt On Change pins
        DEFVAL  = 0x03,                                   // Default Value
        INTCON  = 0x04,                                   // Interrupt Control
        IOCON   = 0x05,                                   // I/O Expander Configuration
        GPPU    = 0x06,                                   // GPIO PullUp Resistor
        INTF    = 0x07,                                   // Interrupt Flag
        INTCAP  = 0x08,                                   // Interrupt Capture
        GPIO    = 0x09,                                   // General Purpose IO
        OLAT    = 0x0A,                                   // Output Latch
        nmbrOfRegisters
    };
    static constexpr uint32_t nmbrOfRegisters = static_cast<uint32_t>(registerAddress::nmbrOfRegisters);

    uint8_t readRegister(registerAddress address) const;               // read the register at address xxx from the device
    void writeRegister(registerAddress address, uint8_t value);        // write a value to a register. Writes to mcu copy first, then to matching register in HW
    bool verify() const;                                               // verifies if RAM copy of registers and hardware registers in MCP23008 match. Returns False if some bits got corrupt due to whatever HW problem
    void refresh();                                                    // refreshes all hardware registers from values in the MCU
    void refresh(registerAddress address);                             // refreshes 1 hardware register from value in the MCU

#ifndef unitTesting
  private:
#endif

    void writeHardwareRegister(registerAddress address, uint8_t value);        // write a value to a register at address xx to the HW device
    uint8_t I2Caddress{0};                                                     // remembers the I2C address of each instance - 0 means : no device found
    uint8_t mcuCopyOfRegisters[nmbrOfRegisters];                               // RAM copies of all register values, so we can refresh the HW
};
