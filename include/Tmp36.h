#ifndef TMP36_H
#define TMP36_H

#include "../lib/TemperatureSensor/TemperatureSensor.h"

namespace TemperatureSensor {
    class Tmp36 : public TemperatureSensor {
        public:
            explicit Tmp36(GPIO::Pin pin) noexcept : TemperatureSensor(pin) {}
            ~Tmp36() noexcept override = default;
            void begin() noexcept override;
            void update() noexcept override;
            void read() noexcept override;
            void write() noexcept override;
            void end() noexcept override;
    };
}

#endif // TMP36_H