#ifndef APP_SENSOR_APP_H
#define APP_SENSOR_APP_H

#include <jenlib/events/EventDispatcher.h>
#include <jenlib/time/Time.h>
#include <jenlib/state/SensorStateMachine.h>
#include <jenlib/ble/Roles.h>
#include <jenlib/ble/Ids.h>
#include <jenlib/ble/Messages.h>
#include <jenlib/gpio/drivers/ArduinoGpioDriver.h>
#include <jenlib/ble/drivers/ArduinoBleDriver.h>
#include <jenlib/time/drivers/ArduinoTimeDriver.h>
#include <jenlib/measurement/Measurement.h>
#include <string_view>
#include "../lib/TemperatureSensor/Tmp36.h"


namespace app {

class SensorApp {
    public:
        SensorApp() noexcept;
        void setup() noexcept;
        void loop() noexcept;

    private:
        // Core services
        static constexpr jenlib::ble::DeviceId kDeviceId_ = jenlib::ble::DeviceId(0x12345678);
        std::string_view kDeviceName_ = "Arduino BLE Sensor";
        jenlib::ble::Sensor sensor_ { kDeviceId_ };

        // Arduino drivers (static storage duration is not required when wrapped here)
        jenlib::gpio::ArduinoGpioDriver gpio_driver_;
        jenlib::ble::ArduinoBleDriver ble_driver_{kDeviceName_, kDeviceId_};
        jenlib::time::ArduinoTimeDriver time_driver_;

        // State machine
        jenlib::state::SensorStateMachine sensor_sm_;

        // Hardware sensor
        TemperatureSensor::Tmp36 tmp36_;

        // BLE callbacks
        static void on_connection_static(bool connected);
        static void on_start_static(jenlib::ble::DeviceId sender_id, const jenlib::ble::StartBroadcastMsg &msg);
        static void on_receipt_static(jenlib::ble::DeviceId sender_id, const jenlib::ble::ReceiptMsg &msg);
        static void on_generic_static(jenlib::ble::DeviceId sender_id, const jenlib::ble::BlePayload &payload);

        // Instance forwarding for static callbacks
        static SensorApp* instance_;
        void on_connection(bool connected) noexcept;
        void on_start(jenlib::ble::DeviceId sender_id, const jenlib::ble::StartBroadcastMsg &msg) noexcept;
        void on_receipt(jenlib::ble::DeviceId sender_id, const jenlib::ble::ReceiptMsg &msg) noexcept;
        void on_generic(jenlib::ble::DeviceId sender_id, const jenlib::ble::BlePayload &payload) noexcept;

        // Event handlers
        static void on_measurement_timer_static();
        void on_measurement_timer() noexcept;
        void handle_ble_message_event(const jenlib::events::Event& event) noexcept;
        void handle_connection_state_event(const jenlib::events::Event& event) noexcept;
        void handle_time_tick_event(const jenlib::events::Event& event) noexcept;

        // Helpers
        void start_measurement_session(const jenlib::ble::StartBroadcastMsg& msg) noexcept;
        void stop_measurement_session() noexcept;
        void drain_sensor_and_broadcast() noexcept;
};

} // namespace app

#endif // APP_SENSOR_APP_H


