#include "App/SensorApp.h"

using measurement::Measurement;

namespace app {

SensorApp* SensorApp::instance_ = nullptr;

SensorApp::SensorApp() noexcept
    : tmp36_(GPIO::PinMap()[14]) { // default to A0 (pin 14 on Arduino Uno); can be parameterized later
    instance_ = this;
}

void SensorApp::setup() noexcept {
    jenlib::time::Time::initialize();

    // Initialize BLE and callbacks
    sensor_.begin();
    sensor_.configure_callbacks(jenlib::ble::BleCallbacks{
        .on_connection = &SensorApp::on_connection_static,
        .on_start = &SensorApp::on_start_static,
        .on_reading = nullptr,
        .on_receipt = &SensorApp::on_receipt_static,
        .on_generic = &SensorApp::on_generic_static,
    });

    // Optional: observe state transitions
    sensor_sm_.set_state_action_callback(
        [](jenlib::state::StateAction /*action*/, jenlib::state::SensorState /*state*/) {
            // hook for logging
        });

    // Register event handlers
    jenlib::events::EventDispatcher::register_callback(
        jenlib::events::EventType::kBleMessage,
        [this](const jenlib::events::Event& e){ this->handle_ble_message_event(e); });
    jenlib::events::EventDispatcher::register_callback(
        jenlib::events::EventType::kConnectionStateChange,
        [this](const jenlib::events::Event& e){ this->handle_connection_state_event(e); });
    jenlib::events::EventDispatcher::register_callback(
        jenlib::events::EventType::kTimeTick,
        [this](const jenlib::events::Event& e){ this->handle_time_tick_event(e); });

    // Initialize physical sensor
    tmp36_.begin();
}

void SensorApp::loop() noexcept {
    jenlib::events::EventDispatcher::process_events();
    sensor_.process_events();
    jenlib::time::Time::process_timers();

    // Update physical sensor to push new readings into its ring buffer
    tmp36_.update();
}

// Static thunks
void SensorApp::on_connection_static(bool connected) {
    if (instance_) instance_->on_connection(connected);
}
void SensorApp::on_start_static(jenlib::ble::DeviceId sender_id, const jenlib::ble::StartBroadcastMsg &msg) {
    if (instance_) instance_->on_start(sender_id, msg);
}
void SensorApp::on_receipt_static(jenlib::ble::DeviceId sender_id, const jenlib::ble::ReceiptMsg &msg) {
    if (instance_) instance_->on_receipt(sender_id, msg);
}
void SensorApp::on_generic_static(jenlib::ble::DeviceId sender_id, const jenlib::ble::BlePayload &payload) {
    if (instance_) instance_->on_generic(sender_id, payload);
}

// Instance handlers
void SensorApp::on_connection(bool connected) noexcept {
    sensor_sm_.handle_connection_change(connected);
    jenlib::events::Event event(
        jenlib::events::EventType::kConnectionStateChange,
        jenlib::time::Time::now(),
        connected ? 1u : 0u);
    jenlib::events::EventDispatcher::dispatch_event(event, nullptr);
}

void SensorApp::on_start(jenlib::ble::DeviceId sender_id, const jenlib::ble::StartBroadcastMsg &msg) noexcept {
    if (msg.device_id != kDeviceId_) return;
    bool ok = sensor_sm_.handle_start_broadcast(sender_id, msg);
    if (ok) {
        start_measurement_session(msg);
    }
    jenlib::events::Event event(
        jenlib::events::EventType::kBleMessage,
        jenlib::time::Time::now(),
        static_cast<std::uint32_t>(jenlib::ble::MessageType::StartBroadcast));
    jenlib::events::EventDispatcher::dispatch_event(event, nullptr);
}

void SensorApp::on_receipt(jenlib::ble::DeviceId sender_id, const jenlib::ble::ReceiptMsg &msg) noexcept {
    sensor_sm_.handle_receipt(sender_id, msg);
    jenlib::events::Event event(
        jenlib::events::EventType::kBleMessage,
        jenlib::time::Time::now(),
        static_cast<std::uint32_t>(jenlib::ble::MessageType::Receipt));
    jenlib::events::EventDispatcher::dispatch_event(event, nullptr);
}

void SensorApp::on_generic(jenlib::ble::DeviceId /*sender_id*/, const jenlib::ble::BlePayload &/*payload*/) noexcept {
    jenlib::events::Event event(
        jenlib::events::EventType::kBleMessage,
        jenlib::time::Time::now(),
        static_cast<std::uint32_t>(jenlib::ble::MessageType::Reading));
    jenlib::events::EventDispatcher::dispatch_event(event, nullptr);
}

void SensorApp::on_measurement_timer_static() {
    if (instance_) instance_->on_measurement_timer();
}

void SensorApp::on_measurement_timer() noexcept {
    sensor_sm_.handle_measurement_timer();
    drain_sensor_and_broadcast();
}

void SensorApp::handle_time_tick_event(const jenlib::events::Event& event) noexcept {
    (void)event;
    sensor_sm_.handle_event(event);
}

void SensorApp::handle_ble_message_event(const jenlib::events::Event& /*event*/) noexcept {
    // optional logging hook
}

void SensorApp::handle_connection_state_event(const jenlib::events::Event& event) noexcept {
    bool connected = (event.data != 0);
    if (!connected && sensor_sm_.is_session_active()) {
        sensor_sm_.handle_session_end();
        stop_measurement_session();
    }
}

void SensorApp::start_measurement_session(const jenlib::ble::StartBroadcastMsg& /*msg*/) noexcept {
    stop_measurement_session();
    sensor_sm_.set_measurement_interval_ms(1000);
    drain_sensor_and_broadcast();
    jenlib::time::schedule_repeating_timer(
        sensor_sm_.get_measurement_interval_ms(),
        &SensorApp::on_measurement_timer_static);
}

void SensorApp::stop_measurement_session() noexcept {
    // cleanup hook if needed
}

void SensorApp::drain_sensor_and_broadcast() noexcept {
    if (!sensor_sm_.is_session_active()) return;

    // Pull one or more measurements from ring buffer and broadcast
    Measurement m{};
    while (tmp36_.try_pop(m)) {
        jenlib::ble::ReadingMsg reading_msg{
            .sender_id = kDeviceId_,
            .session_id = sensor_sm_.get_current_session_id(),
            .offset_ms = m.timestamp_ms,
            .temperature_c_centi = measurement::temperature_to_centi(m.temperature_c),
            .humidity_bp = measurement::humidity_to_basis_points(m.humidity_bp)
        };
        sensor_.broadcast_reading(reading_msg);
    }
}

} // namespace app


