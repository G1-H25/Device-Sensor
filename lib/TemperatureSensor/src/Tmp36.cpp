//! @file Tmp36.cpp
//! @brief Concrete implementation of TemperatureSensor using TMP36

#include "../Tmp36.h"
#include <jenlib/time/Time.h>

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
        float temperature_c = (voltage - 0.5f) * 100.0f;
        Measurement m{};
        m.temperature_c = temperature_c;
        m.humidity_bp = 0.0f;
        m.timestamp_ms = jenlib::time::Time::now();
        push_measurement(m);
    }
    void Tmp36::write() noexcept {}

    void Tmp36::end() noexcept {
        pin_.pinMode(GPIO::PinMode::INPUT);
        is_active_ = false;
    }
} // namespace TemperatureSensor