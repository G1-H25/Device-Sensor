#include "../include/Tmp36.h"
#include <Arduino.h>

//! concrete implementation of the TemperatureSensor class using analog read and voltage conversion on Arduino R4 Wifi
//! no humidity sensor available on just TMP36
namespace TemperatureSensor {
    void Tmp36::begin() noexcept {
        pin_.pinMode(GPIO::PinMode::INPUT);
        ring_buffer_.head = 0;
        ring_buffer_.tail = 0;
        is_initialized_ = true;
        is_active_ = true;
    }
    void Tmp36::update() noexcept {
        read();
    }
    void Tmp36::read() noexcept {
        std::uint16_t analog_value = pin_.analogRead();
        float voltage = analog_value * 5.0f / 1023.0f;
        float temperature = (voltage - 0.5f) * 100.0f;
        Measurement m{};
        m.temperature = temperature;
        m.humidity = 0.0f;
        m.timestamp = millis();
        push_measurement(m);
    }
    void Tmp36::write() noexcept {}

    void Tmp36::end() noexcept {
        pin_.pinMode(GPIO::PinMode::INPUT);
        is_active_ = false;
    }
}