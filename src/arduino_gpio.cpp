#ifdef ARDUINO

#include <Arduino.h>
#include <cstdint>
#include "../lib/HardwareInterface/GPIO.h"
#include "../include/ArduinoHardware.h"

namespace GPIO {

static std::uint8_t g_read_resolution_bits = 10;  // Arduino default
static std::uint8_t g_write_resolution_bits = 8;   // Arduino default

Pin::Pin() : raw_pin_(0) {}

Pin::Pin(GPIO::PinIndex pinIndex) : raw_pin_(static_cast<std::uint8_t>(pinIndex)) {}

Pin::~Pin() {}

void Pin::pinMode(PinMode mode) const noexcept {
    if (mode == PinMode::INPUT) {
        ::pinMode(raw_pin_, INPUT);
    } else if (mode == PinMode::OUTPUT) {
        ::pinMode(raw_pin_, OUTPUT);
    } else if (mode == PinMode::INPUT_PULLUP) {
        ::pinMode(raw_pin_, INPUT_PULLUP);
    } else if (mode == PinMode::INPUT_PULLDOWN) {
        ::pinMode(raw_pin_, INPUT_PULLDOWN);
    }
}

Pin PinMap::operator[](GPIO::PinIndex index) const noexcept {
	return Pin(index);
}

void Pin::digitalRead(DigitalValue& value) const noexcept {
	value = static_cast<DigitalValue>(::digitalRead(raw_pin_));
}

std::uint16_t Pin::analogRead() const noexcept {
	return ::analogRead(raw_pin_);
}

void Pin::analogRead(std::uint16_t& value) const noexcept {
	value = ::analogRead(raw_pin_);
}

void Pin::analogWrite(std::uint16_t value) const noexcept {
	::analogWrite(raw_pin_, value);
}

void Pin::digitalWrite(DigitalValue value) const noexcept {
    if (value == DigitalValue::HIGH) {
        ::digitalWrite(raw_pin_, HIGH);
    } else {
        ::digitalWrite(raw_pin_, LOW);
    }
}

DigitalValue Pin::digitalRead() const noexcept {
    if (::digitalRead(raw_pin_) == HIGH) {
        return DigitalValue::HIGH;
    } else {
        return DigitalValue::LOW;
    }
}

void setAnalogReadResolution(std::uint8_t bits) noexcept {
	g_read_resolution_bits = bits;
	#if defined(ARDUINO_ARCH_RENESAS) || defined(ARDUINO_UNOR4_WIFI)
		analogReadResolution(bits);
	#else
		analogReadResolution(bits);
	#endif
}

void setAnalogWriteResolution(std::uint8_t bits) noexcept {
	g_write_resolution_bits = bits;
	#if defined(ARDUINO_ARCH_RENESAS) || defined(ARDUINO_UNOR4_WIFI)
		analogWriteResolution(bits);
	#else
		analogWriteResolution(bits);
	#endif
}

std::uint8_t getAnalogReadResolution() noexcept {
	return g_read_resolution_bits;
}

std::uint8_t getAnalogWriteResolution() noexcept {
	return g_write_resolution_bits;
}

} // namespace GPIO

#endif // ARDUINO