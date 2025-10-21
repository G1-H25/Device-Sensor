#include "../TemperatureSensor.h"
#include <cstddef>

namespace TemperatureSensor {

    TemperatureSensor::TemperatureSensor(GPIO::Pin pin) noexcept : pin_(pin) {}


    //! @brief push a measurement to the ring buffer
    //! @param m the measurement to push
    //! basically, we check where in the ring buffer we are and push the measurement there
    void TemperatureSensor::push_measurement(const Measurement& m) noexcept {
        ring_buffer_.measurements[ring_buffer_.head] = m;
        ring_buffer_.head = ring_buffer_.next(ring_buffer_.head);
        if (ring_buffer_.head == ring_buffer_.tail) {
            ring_buffer_.tail = ring_buffer_.next(ring_buffer_.tail);
        }
    }

    //! @brief try to pop a measurement from the ring buffer
    //! @param out the measurement to pop
    //! @return true if a measurement was popped, false otherwise
    bool TemperatureSensor::try_pop(Measurement& out) noexcept {
        if (ring_buffer_.empty()) {
            return false;
        }
        out = ring_buffer_.measurements[ring_buffer_.tail];
        ring_buffer_.tail = ring_buffer_.next(ring_buffer_.tail);
        return true;
    }

    //! @brief check if the sensor has data
    //! @return true if the sensor has data, false otherwise
    bool TemperatureSensor::has_data() const noexcept {
        return !ring_buffer_.empty();
    }

    //! @brief get the number of measurements in the ring buffer
    //! @return the number of measurements in the ring buffer
    std::size_t TemperatureSensor::available() const noexcept {
        return ring_buffer_.count();
    }

    //! @brief check if the sensor is initialized and active
    //! @return true if the sensor is initialized and active, false otherwise
    TemperatureSensor::operator bool() const noexcept {
        return is_initialized_ && is_active_;
    }

} // namespace TemperatureSensor
