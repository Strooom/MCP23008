#include <Arduino.h>
#include <unity.h>
#include <MCP23008.h>

MCP23008 ioExtender;        // create an instance, default address. As it is a global instance, there are side effects from one test to the next tests !

void test_read_write_registers() {
    // I am avoiding setting bit 6 as output, as on my test HW it is driven by something as an input, so setting it as output is a conflict
    ioExtender.writeRegister(MCP23008::registerAddress::IPOL, 0);
    TEST_ASSERT_EQUAL_UINT8(0x00, ioExtender.readRegister(MCP23008::registerAddress::IPOL));
    ioExtender.writeRegister(MCP23008::registerAddress::IPOL, 0x0F);
    TEST_ASSERT_EQUAL_UINT8(0x0F, ioExtender.readRegister(MCP23008::registerAddress::IPOL));
}

void test2_read_write_registers() {
    // OLAT and GPIO are a bit special, so these are extra tests
    // When writing GPIO, in fact it writes OLAT and those values are sent out to the pins configured as output
    // When reading GPIO, it really reads the values on pins configured as inputs
    // When reading OLAT, it reads the value written there, directly, or indirectly via writing GPIO
    ioExtender.writeRegister(MCP23008::registerAddress::OLAT, 0x55);
    TEST_ASSERT_EQUAL_UINT8(0x55, ioExtender.readRegister(MCP23008::registerAddress::OLAT));
    ioExtender.writeRegister(MCP23008::registerAddress::OLAT, 0xAA);
    TEST_ASSERT_EQUAL_UINT8(0xAA, ioExtender.readRegister(MCP23008::registerAddress::OLAT));
    ioExtender.writeRegister(MCP23008::registerAddress::GPIO, 0x55);                                // writing GPIO
    TEST_ASSERT_EQUAL_UINT8(0x55, ioExtender.readRegister(MCP23008::registerAddress::OLAT));        // but reading OLAT
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

void test2_refresh() {
    ioExtender.writeRegister(MCP23008::registerAddress::IPOL, 0xAA);
    TEST_ASSERT_EQUAL_UINT8(0xAA, ioExtender.readRegister(MCP23008::registerAddress::IPOL));
    ioExtender.writeHardwareRegister(MCP23008::registerAddress::IPOL, 0x55);
    TEST_ASSERT_EQUAL_UINT8(0x55, ioExtender.readRegister(MCP23008::registerAddress::IPOL));
    ioExtender.refresh(MCP23008::registerAddress::IPOL);
    TEST_ASSERT_EQUAL_UINT8(0xAA, ioExtender.readRegister(MCP23008::registerAddress::IPOL));
}

void test_verify() {
    ioExtender.initialize();
    TEST_ASSERT_TRUE(ioExtender.verify());
    // now flip one bit as if it was a hardware glitch
    ioExtender.writeHardwareRegister(MCP23008::registerAddress::IPOL, 0x01);
    TEST_ASSERT_FALSE(ioExtender.verify());
}

void test_pinmode() {
    ioExtender.initialize();
    ioExtender.pinMode(0, INPUT);
    TEST_ASSERT_EQUAL_UINT8(0xFF, ioExtender.readRegister(MCP23008::registerAddress::IODIR));
    TEST_ASSERT_EQUAL_UINT8(0x00, ioExtender.readRegister(MCP23008::registerAddress::IPOL));
    TEST_ASSERT_EQUAL_UINT8(0x00, ioExtender.readRegister(MCP23008::registerAddress::GPPU));
    ioExtender.pinMode(0, INPUT_PULLUP);
    TEST_ASSERT_EQUAL_UINT8(0xFF, ioExtender.readRegister(MCP23008::registerAddress::IODIR));
    TEST_ASSERT_EQUAL_UINT8(0x00, ioExtender.readRegister(MCP23008::registerAddress::IPOL));
    TEST_ASSERT_EQUAL_UINT8(0x01, ioExtender.readRegister(MCP23008::registerAddress::GPPU));
    ioExtender.pinMode(0, OUTPUT);
    TEST_ASSERT_EQUAL_UINT8(0xFE, ioExtender.readRegister(MCP23008::registerAddress::IODIR));
    TEST_ASSERT_EQUAL_UINT8(0x00, ioExtender.readRegister(MCP23008::registerAddress::IPOL));
    TEST_ASSERT_EQUAL_UINT8(0x00, ioExtender.readRegister(MCP23008::registerAddress::GPPU));
}

void test1_digitalWrite() {
    ioExtender.pinMode(0, OUTPUT);
    ioExtender.digitalWrite(0, 0);
    TEST_ASSERT_EQUAL_UINT8(0x00, ioExtender.readRegister(MCP23008::registerAddress::OLAT));
    ioExtender.digitalWrite(0, 1);
    TEST_ASSERT_EQUAL_UINT8(0x01, ioExtender.readRegister(MCP23008::registerAddress::OLAT));
}

void test2_digitalWrite() {
    TEST_IGNORE();        // I need a hw loopback from output to input, to complete this test
}

void test_digitalRead() {
    TEST_IGNORE();        // I need a hw loopback from output to input, to complete this test
}

void setup() {
    UNITY_BEGIN();
    ioExtender.initialize();
    RUN_TEST(test_read_write_registers);
    RUN_TEST(test2_read_write_registers);
    RUN_TEST(test_getAddress);
    RUN_TEST(test_refresh);
    RUN_TEST(test2_refresh);
    RUN_TEST(test_verify);
    RUN_TEST(test_pinmode);
    RUN_TEST(test1_digitalWrite);
    RUN_TEST(test2_digitalWrite);
    RUN_TEST(test_digitalRead);

    UNITY_END();
}

void loop() {
}
