#ifndef ARDUINO
#include <cstdint>
#include "HardwareInterface/GPIO.h"

namespace GPIO {

Pin::Pin() noexcept = default;
Pin::Pin(GPIO::PinIndex) noexcept = default;
Pin::~Pin() noexcept = default;

void Pin::pinMode(PinMode) {}
void Pin::digitalWrite(DigitalValue) const noexcept {}
void Pin::digitalRead(DigitalValue& value) const noexcept { value = DigitalValue::LOW; }
std::uint16_t Pin::analogRead() const noexcept { return 0; }
void Pin::analogRead(std::uint16_t& value) const noexcept { value = 0; }
void Pin::analogWrite(std::uint16_t) const noexcept {}
DigitalValue Pin::digitalRead() const noexcept { return DigitalValue::LOW; }

PinMap::PinMap() {}
PinMap::~PinMap() {}
Pin PinMap::operator[](PinIndex index) const noexcept { return Pin(index); }

void setAnalogReadResolution(std::uint8_t) noexcept {}
void setAnalogWriteResolution(std::uint8_t) noexcept {}
std::uint8_t getAnalogReadResolution() noexcept { return 10; }
std::uint8_t getAnalogWriteResolution() noexcept { return 8; }

} // namespace GPIO
#endif // ARDUINO
