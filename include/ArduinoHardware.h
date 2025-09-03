#include <cstdint>

namespace GPIO {
	enum class PinIndex : std::uint8_t {
		D0, // reserverad för UART TX
		D1, // reserverad för UART RX
		D2,
		D3, // ledig, PWM
		D4,
		D5, // ledig, PWM
		D6, // ledig, PWM
		D7,
		D8,
		D9, // PWM, SPI MOSI
		D10, // PWM, SPI MISO
		D11, // PWM, SPI CS
		D12, // SPI SCLK
		D13, // används för inbyggd led
		A0, // ADC
		A1,
		A2,
		A3,
		A4,
		A5,
        NUM_PINS
	};
}