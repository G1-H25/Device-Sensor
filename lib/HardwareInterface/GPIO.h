#ifndef HARDWARE_INTERFACE_GPIO_H
#define HARDWARE_INTERFACE_GPIO_H

#include <cstdint>

namespace GPIO {

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

    class Pin {
        public:
            //! default constructor
            Pin() noexcept;

            //! constructor with pin index
            explicit Pin(PinIndex pin_index) noexcept;


            //! set the mode of the pin
            void pinMode(PinMode mode) const noexcept;
            //! write to the pin
            void digitalWrite(DigitalValue value) const noexcept;
            void analogWrite(std::uint16_t value) const noexcept;
            //! read from the pin
            DigitalValue digitalRead() const noexcept;
            std::uint16_t analogRead() const noexcept;
            
            ~Pin() = default;
        private:
            std::uint8_t raw_pin_; //! the raw pin number, to be passed to the hardware
    };

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
}

#endif // HARDWARE_INTERFACE_GPIO_H