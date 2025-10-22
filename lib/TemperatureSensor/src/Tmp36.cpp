//! @file Tmp36.cpp
//! @brief Concrete implementation of TemperatureSensor using TMP36

#include "TemperatureSensor/TemperatureSensor.h"
#include "jenlib/time/Time.h"

namespace TemperatureSensor {
    class Tmp36 : public TemperatureSensor {
    public:
        explicit Tmp36(GPIO::Pin pin) noexcept : TemperatureSensor(pin) {}
        virtual ~Tmp36() noexcept = default;
        void begin() noexcept override;
        void update() noexcept override;
        void read() noexcept override;
        void write() noexcept override;
        void end() noexcept override;
    };

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