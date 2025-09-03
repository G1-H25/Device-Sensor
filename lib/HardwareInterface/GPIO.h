#ifndef HARDWARE_INTERFACE_GPIO_H
#define HARDWARE_INTERFACE_GPIO_H

#include <cstdint>

namespace GPIO {
    class Pin {
        public:
            Pin();
            explicit Pin(PinIndex pin_index) noexcept : raw_pin_(static_cast<std::uint8_t>(pin_index)) {}
            void pinMode(PinMode mode) const noexcept;
            void digitalWrite(DigitalValue value) const noexcept;
            void digitalRead(DigitalValue& value) const noexcept;
            void analogRead(std::uint16_t& value) const noexcept;
            void analogWrite(std::uint16_t value) const noexcept;
            DigitalValue digitalRead() const noexcept;
            std::uint16_t analogRead() const noexcept;
            
            ~Pin();
        private:
            std::uint8_t raw_pin_;
    };

    enum class PinMode : std::uint8_t {
        INPUT,
        OUTPUT,
        INPUT_PULLUP,
        INPUT_PULLDOWN,
    };

    enum class DigitalValue : std::uint8_t {
        LOW,
        HIGH,
    };

    enum class PinIndex : std::uint8_t; //! implementation-defined elsewhere based on the hardware
    
    //! Configure analog conversion and PWM resolution for the platform
    void setAnalogReadResolution(std::uint8_t bits) noexcept;
    void setAnalogWriteResolution(std::uint8_t bits) noexcept;
    std::uint8_t getAnalogReadResolution() noexcept;
    std::uint8_t getAnalogWriteResolution() noexcept;
    
    //! class that when accessed with [] accesses the GPIO pin with that index
    //! @note this is a singleton
    class PinMap {
        public:
            PinMap();
            ~PinMap();
            //! access the GPIO pin with that index
            //! precondition: the pin index is valid
            //! postcondition: the pin is accessed
            Pin operator[](PinIndex index) const noexcept;
    };



    //! GPIO::PinMap pins;
    //! pins[GPIO::PinIndex::D3].digitalWrite(GPIO::DigitalValue::HIGH);
}

#endif // HARDWARE_INTERFACE_GPIO_H