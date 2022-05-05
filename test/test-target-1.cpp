#include <Arduino.h>
#include <unity.h>
#include <MCP23008.h>

MCP23008 ioExtender;        // create an instance, default address

void test_read_write_registers() {
    // TEST_ASSERT_EQUAL_UINT8(0xFF, ioExtender.readRegister(MCP23008::registerAddress::IODIR));        // should be FF after POWERON reset
    // Using IPOL register as a test to write and read values, as this cannot do any harm to the hardware
    ioExtender.writeRegister(MCP23008::registerAddress::IPOL, 0);
    TEST_ASSERT_EQUAL_UINT8(0x00, ioExtender.readRegister(MCP23008::registerAddress::IPOL));
    ioExtender.writeRegister(MCP23008::registerAddress::IPOL, 0x0F);
    TEST_ASSERT_EQUAL_UINT8(0x0F, ioExtender.readRegister(MCP23008::registerAddress::IPOL));
    ioExtender.writeRegister(MCP23008::registerAddress::IPOL, 0xF0);
    TEST_ASSERT_EQUAL_UINT8(0xF0, ioExtender.readRegister(MCP23008::registerAddress::IPOL));
}

void setup() {
    UNITY_BEGIN();
    ioExtender.initialize();
    RUN_TEST(test_read_write_registers);
    UNITY_END();
}

void loop() {
}
