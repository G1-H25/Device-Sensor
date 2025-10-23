#ifndef TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_H

#include <jenlib/gpio/GPIO.h>
#include "jenlib/measurement/Measurement.h"
#include <cstdint>
#include <array>
#include <cstddef>

namespace TemperatureSensor {

    using Measurement = measurement::Measurement;

    //! @brief ring buffer for storing measurements
    struct RingBuffer {
        static constexpr std::size_t capacity = 10;
        std::array<Measurement, capacity> measurements{};
        std::size_t head = 0;
        std::size_t tail = 0;
        //! @brief get the next index in the ring buffer
        //! @param i the current index
        //! @return the next index
        constexpr std::size_t next(std::size_t i) const noexcept {
            return (i + 1 == capacity) ? 0 : (i + 1);
        }
        //! @brief check if the ring buffer is empty
        //! @return true if the ring buffer is empty, false otherwise
        constexpr bool empty() const noexcept { return head == tail; }
        //! @brief get the number of measurements in the ring buffer
        //! @return the number of measurements in the ring buffer
        constexpr std::size_t count() const noexcept {
            return head >= tail ? (head - tail) : (capacity - (tail - head));
        }
    };

    class TemperatureSensor {

        protected:
            GPIO::Pin pin_;
            RingBuffer ring_buffer_{};
            bool is_initialized_ = false;
            bool is_active_ = false;
            //! @brief push a measurement to the ring buffer
            //! @param m the measurement to push
            void push_measurement(const Measurement& m) noexcept;

        public:
            explicit TemperatureSensor(GPIO::Pin pin) noexcept;
            virtual ~TemperatureSensor() noexcept = default;
            //! retrieval
            //! @brief Try to pop a measurement from the ring buffer
            //! @param out output parameter to store the popped measurement
            //! @return True if a measurement was popped, false otherwise
            bool try_pop(Measurement& out) noexcept;
            //! @brief check if the sensor has data
            //! @return true if the sensor has data, false otherwise
            bool has_data() const noexcept;
            //! @brief get the number of measurements in the ring buffer
            //! @return the number of measurements in the ring buffer
            std::size_t available() const noexcept;
            //! lifecycle interface
            virtual void begin() noexcept = 0;
            virtual void update() noexcept = 0;
            virtual void read() noexcept = 0;
            virtual void write() noexcept = 0;
            virtual void end() noexcept = 0;
            //! @brief check if the sensor is initialized and active
            //! @return true if the sensor is initialized and active, false otherwise
            explicit operator bool() const noexcept;
    };

}

#endif // TEMPERATURE_SENSOR_H