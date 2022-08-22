#include <Arduino.h>
#include <unity.h>
#include <MCP23008.h>

MCP23008 ioExtender;        // create an instance, default address

void test_read_write_registers() {
    ioExtender.writeRegister(MCP23008::registerAddress::IPOL, 0);
    TEST_ASSERT_EQUAL_UINT8(0x00, ioExtender.readRegister(MCP23008::registerAddress::IPOL));
    ioExtender.writeRegister(MCP23008::registerAddress::IPOL, 0x0F);
    TEST_ASSERT_EQUAL_UINT8(0x0F, ioExtender.readRegister(MCP23008::registerAddress::IPOL));
    ioExtender.writeRegister(MCP23008::registerAddress::IPOL, 0xF0);
    TEST_ASSERT_EQUAL_UINT8(0xF0, ioExtender.readRegister(MCP23008::registerAddress::IPOL));
}

void test_getAddress() {
    TEST_ASSERT_EQUAL_UINT8(0x20, ioExtender.getI2cAddress());
}

void test_refresh() {
    ioExtender.writeRegister(MCP23008::registerAddress::IPOL, 0xAA);
    TEST_ASSERT_EQUAL_UINT8(0xAA, ioExtender.readRegister(MCP23008::registerAddress::IPOL));
    ioExtender.writeHardwareRegister(MCP23008::registerAddress::IPOL, 0x55);
    TEST_ASSERT_EQUAL_UINT8(0x55, ioExtender.readRegister(MCP23008::registerAddress::IPOL));
    ioExtender.refresh();
    TEST_ASSERT_EQUAL_UINT8(0xAA, ioExtender.readRegister(MCP23008::registerAddress::IPOL));
}

void setup() {
    UNITY_BEGIN();
    ioExtender.initialize();
    RUN_TEST(test_read_write_registers);
    RUN_TEST(test_getAddress);
    RUN_TEST(test_refresh);
    UNITY_END();
}

void loop() {
}
