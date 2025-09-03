#include <Arduino.h>
#include "../lib/HardwareInterface/GPIO.h"
#include <ArduinoHardware.h>
#include "../include/Tmp36.h"

static GPIO::PinMap pinMap;

//! min kassa analoga TMP36 skulle bli typ så här
static GPIO::PinIndex temp_pin = GPIO::PinIndex::A3;
static TemperatureSensor::Tmp36 temp_sensor(pinMap[temp_pin]);

void setup() {
  temp_sensor.begin();
}

void loop() {
  temp_sensor.update();
  //! print the head of the ring buffer
  if (temp_sensor.has_data()) {
    TemperatureSensor::Measurement m;
      if (temp_sensor.try_pop(m)) {
        Serial.println(m.temperature);
      }
  }
  delay(1000);
}