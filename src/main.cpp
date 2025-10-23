//! @file main.cpp
//! @brief Main entry point for the Arduino BLE Sensor application.
//! Using the SensorApp class to manage the application lifecycle.
//! @copyright 2025 Jennifer Gott, released under the MIT License.
//! @author Jennifer Gott (jennifer.gott@chasacademy.se)


#include "App/SensorApp.h"

//! @section Global state
static app::SensorApp sensor_app;

//! @section Arduino functions
void setup() {
    sensor_app.setup();
}

void loop() {
    sensor_app.loop();
}

