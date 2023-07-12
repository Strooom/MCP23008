#include <Arduino.h>
#include <Wire.h>
#include <unity.h>
#include <MCP23008.h>

MCP23008 ioExtender;        // create an instance, default address. As it is a global instance, there are side effects from one test to the next tests !

void test_discoverAddress() {
    TEST_ASSERT_FALSE(ioExtender.isPresent());
    TEST_ASSERT_EQUAL_UINT8(0, ioExtender.getI2cAddress());

    ioExtender.discoverI2cAddress(0x22);                              // there is only one device on our test HW, so this should fail
    TEST_ASSERT_FALSE(ioExtender.isPresent());

    ioExtender.discoverI2cAddress();
    TEST_ASSERT_TRUE(ioExtender.isPresent());
    TEST_ASSERT_EQUAL_UINT8(0x21, ioExtender.getI2cAddress());        // this test was done with powerpcb v2, which has the MCP23008 at address 0x21
}

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

void test_digitalWrite() {
    ioExtender.pinMode(0, OUTPUT);
    ioExtender.digitalWrite(0, 0);
    TEST_ASSERT_EQUAL_UINT8(0x00, ioExtender.readRegister(MCP23008::registerAddress::OLAT));
    ioExtender.digitalWrite(0, 1);
    TEST_ASSERT_EQUAL_UINT8(0x01, ioExtender.readRegister(MCP23008::registerAddress::OLAT));
}

void test_digitalWriteRead() {
    // this test requires a hw loopback from output GPIO3 to input GPIO6 to pass this test
    // the loopback is inverting, ie a low output, results in a high input and vice versa
    ioExtender.pinMode(3, OUTPUT);
    ioExtender.pinMode(6, INPUT);

    ioExtender.digitalWrite(3, 0);
    TEST_ASSERT_EQUAL_UINT8(1, ioExtender.digitalRead(6));

    ioExtender.digitalWrite(3, 1);
    TEST_ASSERT_EQUAL_UINT8(0, ioExtender.digitalRead(6));
}

void test_relay() {
    // This test  switches the latching relay on GPIO4 and GPIO5 on and off, which can be heard as a click
    // No automatic test
    ioExtender.pinMode(4, OUTPUT);
    ioExtender.pinMode(5, OUTPUT);
    for (auto i = 0; i < 8; i++) {
        ioExtender.digitalWrite(5, 1);
        delay(100);
        ioExtender.digitalWrite(5, 0);
        delay(500);
        ioExtender.digitalWrite(4, 1);
        delay(100);
        ioExtender.digitalWrite(4, 0);
        delay(500);
    }
    TEST_IGNORE_MESSAGE("No automatic testresults - please listen to the relay clicks");
}

void setup() {
    delay(1000);         // some delay to allow the PCs serial monitor to start up
    Wire.begin();        // enables I2C communication in general

    UNITY_BEGIN();
    RUN_TEST(test_discoverAddress);
    RUN_TEST(test_read_write_registers);
    RUN_TEST(test2_read_write_registers);
    RUN_TEST(test_refresh);
    RUN_TEST(test2_refresh);
    RUN_TEST(test_verify);
    RUN_TEST(test_pinmode);
    RUN_TEST(test_digitalWrite);
    RUN_TEST(test_digitalWriteRead);
    RUN_TEST(test_relay);
    UNITY_END();
}

void loop() {
}
