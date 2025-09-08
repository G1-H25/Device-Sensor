//! @file test_smoke_temperature_sensor.cpp
//! @brief Automated smoke test for the temperature sensor

#include <unity.h> 
#include "TemperatureSensor/TemperatureSensor.h"
#include "Tmp36.h"

void setUp() {}
void tearDown() {}

void test_smoke_initialize_sensor() {
    // Arrange
    GPIO::PinMap pins;
    TemperatureSensor::Tmp36 sensor(pins[static_cast<GPIO::PinIndex>(17)]); // A3 for native
    
    // Act
    sensor.begin();
    
    // Assert
    TEST_ASSERT_TRUE(static_cast<bool>(sensor));
    TEST_ASSERT_FALSE(sensor.has_data());
    TEST_ASSERT_EQUAL_UINT32(0, sensor.available());
}

void test_smoke_measurement_pipeline() {
    // Arrange
    GPIO::PinMap pins;
    TemperatureSensor::Tmp36 sensor(pins[static_cast<GPIO::PinIndex>(17)]);
    sensor.begin();
    
    // Act
    sensor.update();
    
    // Assert
    TEST_ASSERT_TRUE(sensor.has_data());
}

void test_smoke_error_handling() {
    // Arrange
    GPIO::PinMap pins;
    TemperatureSensor::Tmp36 sensor(pins[static_cast<GPIO::PinIndex>(17)]);
    sensor.begin();
    
    // Act
    sensor.update();
    
    // Assert
    TEST_ASSERT_TRUE(sensor.has_data());
}

void test_smoke_ring_buffer_readout() {
    // Arrange
    GPIO::PinMap pins;
    TemperatureSensor::Tmp36 sensor(pins[static_cast<GPIO::PinIndex>(17)]);
    sensor.begin();
    
    // Act
    sensor.update();

    // Assert
    TEST_ASSERT_EQUAL_UINT32(1, sensor.available());
    TemperatureSensor::Measurement m;
    TEST_ASSERT_TRUE(sensor.try_pop(m));
    TEST_ASSERT_EQUAL_FLOAT(m.temperature, -50.0f);
    TEST_ASSERT_EQUAL_FLOAT(m.humidity, 0.0f);
    TEST_ASSERT_EQUAL_UINT32(m.timestamp, 0);
    TEST_ASSERT_FALSE(sensor.has_data());
    TEST_ASSERT_EQUAL_UINT32(0, sensor.available());
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_smoke_initialize_sensor);
    RUN_TEST(test_smoke_measurement_pipeline);
    RUN_TEST(test_smoke_error_handling);
    RUN_TEST(test_smoke_ring_buffer_readout);
    UNITY_END();
    return 0;
}
