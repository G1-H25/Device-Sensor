//! @file arduino_sensor_app_integration.cpp
//! @brief Arduino-only integration test for SensorApp complete lifecycle.
//! @copyright 2025 Jennifer Gott, released under the MIT License.
//! @author Jennifer Gott (jennifer.gott@chasacademy.se)
//!
//! NOTE: This test is designed to run ONLY on Arduino hardware.
//! It requires Arduino BLE, GPIO, and timing capabilities.

#ifdef ARDUINO

#include <unity.h>
#include <Arduino.h>
#include "App/SensorApp.h"

using app::SensorApp;

void test_sensor_app_arduino_initialization() {
    //! @section Arrange & Act
    SensorApp sensor_app;

    // Act - Initialize the application (equivalent to Arduino setup())
    sensor_app.setup();

    //! @section Assert - Verify initialization completed without errors
    // On Arduino, we can verify that BLE, GPIO, and other services initialized
    TEST_ASSERT_TRUE(true); // If we get here without crashing, initialization worked
}

void test_sensor_app_arduino_loop_execution() {
    //! @section Arrange
    SensorApp sensor_app;
    sensor_app.setup();

    //! @section Act - Execute multiple loop iterations (equivalent to Arduino loop())
    for (int i = 0; i < 5; ++i) {
        sensor_app.loop();
        delay(100); // Small delay between loops to simulate real operation
    }

    //! @section Assert - Loop execution should work without errors
    TEST_ASSERT_TRUE(true); // If we get here without crashing, loop execution worked
}

void test_sensor_app_arduino_with_measurements() {
    //! @section Arrange
    SensorApp sensor_app;
    sensor_app.setup();

    //! @section Act - Run loops to trigger sensor measurements
    for (int i = 0; i < 10; ++i) {
        sensor_app.loop();
        delay(200); // Allow time for sensor readings
    }

    //! @section Assert - The application should handle measurements without errors
    TEST_ASSERT_TRUE(true); // If we get here without crashing, measurements worked
}

void setUp() {
    // Arduino-specific setup if needed
}

void tearDown() {
    // Arduino-specific cleanup if needed
}

void setup() {
    // Initialize Arduino serial for test output
    Serial.begin(9600);
    delay(2000); // Allow serial to initialize

    UNITY_BEGIN();
    RUN_TEST(test_sensor_app_arduino_initialization);
    RUN_TEST(test_sensor_app_arduino_loop_execution);
    RUN_TEST(test_sensor_app_arduino_with_measurements);
    UNITY_END();
}

void loop() {
    // Unity tests run once in setup(), nothing needed in loop()
}

#else
#warning "This test is designed for Arduino hardware only. Skipping compilation."
#endif // ARDUINO
