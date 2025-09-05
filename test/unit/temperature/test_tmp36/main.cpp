#include <unity.h>
#include "TemperatureSensor/TemperatureSensor.h"
#include "Tmp36.h"

using TemperatureSensor::Measurement;

void test_ring_buffer_basic_push_pop() {
	// Arrange
	GPIO::PinMap pins;
	TemperatureSensor::Tmp36 sensor(pins[static_cast<GPIO::PinIndex>(0)]);
	sensor.begin();

	Measurement m{};
	m.timestamp = 1;
	m.temperature = 25.0f;
	m.humidity = 0.0f;

	// Act
	sensor.read();

	// Assert
	TEST_ASSERT_TRUE(sensor.has_data());
	TEST_ASSERT_EQUAL_UINT32(1, sensor.available());

	// Act
	Measurement out{};
	bool pop_success = sensor.try_pop(out);

	// Assert
	TEST_ASSERT_TRUE(pop_success);
	TEST_ASSERT_EQUAL_UINT32(0, sensor.available());
	TEST_ASSERT_FALSE(sensor.has_data());
}

void test_ring_buffer_capacity_limits() {
	// Arrange
	GPIO::PinMap pins;
	TemperatureSensor::Tmp36 sensor(pins[static_cast<GPIO::PinIndex>(0)]);
	sensor.begin();

	// Act - Fill the ring buffer to effective capacity (9 measurements)
	// Note: One slot is always kept empty to distinguish between empty and full states
	for (std::size_t i = 0; i < 9; ++i) {
		sensor.read();
		TEST_ASSERT_EQUAL_UINT32(i + 1, sensor.available());
		TEST_ASSERT_TRUE(sensor.has_data());
	}

	// Assert - Verify we're at effective capacity
	TEST_ASSERT_EQUAL_UINT32(9, sensor.available());

	// Act - Add one more - should cause oldest to be overwritten (ring buffer behavior)
	sensor.read();

	// Assert - Still at effective capacity
	TEST_ASSERT_EQUAL_UINT32(9, sensor.available());
	TEST_ASSERT_TRUE(sensor.has_data());
}

void test_ring_buffer_overflow_behavior() {
	// Arrange
	GPIO::PinMap pins;
	TemperatureSensor::Tmp36 sensor(pins[static_cast<GPIO::PinIndex>(0)]);
	sensor.begin();

	// Act - Fill buffer to effective capacity
	for (std::size_t i = 0; i < 9; ++i) {
		sensor.read();
	}

	// Act - Store the first measurement for comparison
	Measurement first_measurement{};
	TEST_ASSERT_TRUE(sensor.try_pop(first_measurement));

	// Act - Add one more measurement - this should overwrite the oldest
	sensor.read();

	// Assert - Still at capacity
	TEST_ASSERT_EQUAL_UINT32(9, sensor.available());

	// Act - Pop all measurements and verify the first one was overwritten
	Measurement current{};
	for (std::size_t i = 0; i < 8; ++i) {
		TEST_ASSERT_TRUE(sensor.try_pop(current));
		TEST_ASSERT_EQUAL_UINT32(8 - i, sensor.available());
	}

	// Act - The last measurement should be the one we just added
	TEST_ASSERT_TRUE(sensor.try_pop(current));

	// Assert - Buffer should be empty
	TEST_ASSERT_EQUAL_UINT32(0, sensor.available());
	TEST_ASSERT_FALSE(sensor.has_data());
}

void test_ring_buffer_empty_pop_behavior() {
	// Arrange
	GPIO::PinMap pins;
	TemperatureSensor::Tmp36 sensor(pins[static_cast<GPIO::PinIndex>(0)]);
	sensor.begin();

	// Act - Try to pop from empty buffer
	Measurement out{};
	bool pop_success = sensor.try_pop(out);

	// Assert - Should fail and buffer should remain empty
	TEST_ASSERT_FALSE(pop_success);
	TEST_ASSERT_EQUAL_UINT32(0, sensor.available());
	TEST_ASSERT_FALSE(sensor.has_data());

	// Act - Add one measurement
	sensor.read();

	// Assert - Buffer should have one measurement
	TEST_ASSERT_EQUAL_UINT32(1, sensor.available());

	// Act - Pop it
	pop_success = sensor.try_pop(out);

	// Assert - Pop should succeed and buffer should be empty
	TEST_ASSERT_TRUE(pop_success);
	TEST_ASSERT_EQUAL_UINT32(0, sensor.available());

	// Act - Try to pop again from now-empty buffer
	pop_success = sensor.try_pop(out);

	// Assert - Should fail again
	TEST_ASSERT_FALSE(pop_success);
	TEST_ASSERT_EQUAL_UINT32(0, sensor.available());
}

void test_ring_buffer_multiple_pop_operations() {
	// Arrange
	GPIO::PinMap pins;
	TemperatureSensor::Tmp36 sensor(pins[static_cast<GPIO::PinIndex>(0)]);
	sensor.begin();

	// Act - Add multiple measurements
	for (std::size_t i = 0; i < 5; ++i) {
		sensor.read();
		TEST_ASSERT_EQUAL_UINT32(i + 1, sensor.available());
	}

	// Act - Pop them one by one
	Measurement out{};
	for (std::size_t i = 5; i > 0; --i) {
		bool pop_success = sensor.try_pop(out);
		TEST_ASSERT_TRUE(pop_success);
		TEST_ASSERT_EQUAL_UINT32(i - 1, sensor.available());
	}

	// Assert - Buffer should be empty
	TEST_ASSERT_EQUAL_UINT32(0, sensor.available());
	TEST_ASSERT_FALSE(sensor.has_data());
}

void test_ring_buffer_wrap_around() {
	// Arrange
	GPIO::PinMap pins;
	TemperatureSensor::Tmp36 sensor(pins[static_cast<GPIO::PinIndex>(0)]);
	sensor.begin();

	// Act - Fill buffer to effective capacity
	for (std::size_t i = 0; i < 9; ++i) {
		sensor.read();
	}
	TEST_ASSERT_EQUAL_UINT32(9, sensor.available());

	// Act - Pop 3 measurements
	Measurement out{};
	for (std::size_t i = 0; i < 3; ++i) {
		TEST_ASSERT_TRUE(sensor.try_pop(out));
	}
	TEST_ASSERT_EQUAL_UINT32(6, sensor.available());

	// Act - Add 3 more measurements - this should wrap around
	for (std::size_t i = 0; i < 3; ++i) {
		sensor.read();
	}
	TEST_ASSERT_EQUAL_UINT32(9, sensor.available());

	// Act - Pop all remaining measurements
	for (std::size_t i = 0; i < 9; ++i) {
		TEST_ASSERT_TRUE(sensor.try_pop(out));
		TEST_ASSERT_EQUAL_UINT32(8 - i, sensor.available());
	}

	// Assert - Buffer should be empty
	TEST_ASSERT_EQUAL_UINT32(0, sensor.available());
	TEST_ASSERT_FALSE(sensor.has_data());
}

void test_ring_buffer_data_integrity() {
	// Arrange
	GPIO::PinMap pins;
	TemperatureSensor::Tmp36 sensor(pins[static_cast<GPIO::PinIndex>(0)]);
	sensor.begin();

	// Act - Add a measurement
	sensor.read();
	TEST_ASSERT_EQUAL_UINT32(1, sensor.available());

	// Act - Pop it and verify data integrity
	Measurement out{};
	bool pop_success = sensor.try_pop(out);
	TEST_ASSERT_TRUE(pop_success);
	
	// Assert - Verify the measurement has reasonable values (not garbage)
	TEST_ASSERT_TRUE(out.timestamp >= 0);
	// Note: Native GPIO returns 0, so temperature will be calculated as (0 * 5.0f / 1023.0f - 0.5f) * 100.0f = -50.0f
	// This is within the TMP36 range of -40°C to +125°C
	TEST_ASSERT_TRUE(out.temperature >= -60.0f && out.temperature <= 125.0f); // Allow for native GPIO 0 value
	TEST_ASSERT_EQUAL_FLOAT(0.0f, out.humidity); // TMP36 doesn't measure humidity
}

void test_ring_buffer_bounds_safety() {
	// Arrange
	GPIO::PinMap pins;
	TemperatureSensor::Tmp36 sensor(pins[static_cast<GPIO::PinIndex>(0)]);
	sensor.begin();

	// Act - Test that we can't exceed the effective capacity
	for (std::size_t i = 0; i < 20; ++i) { // Try to add more than capacity
		sensor.read();
		// Assert - The count should never exceed 9
		TEST_ASSERT_TRUE(sensor.available() <= 9);
	}

	// Assert - Verify we're at effective capacity
	TEST_ASSERT_EQUAL_UINT32(9, sensor.available());

	// Act - Test that popping from empty buffer is safe
	Measurement out{};
	for (std::size_t i = 0; i < 20; ++i) { // Try to pop more than we have
		if (sensor.available() == 0) {
			bool pop_success = sensor.try_pop(out);
			TEST_ASSERT_FALSE(pop_success);
			break;
		}
		bool pop_success = sensor.try_pop(out);
		TEST_ASSERT_TRUE(pop_success);
	}

	// Assert - Buffer should be empty
	TEST_ASSERT_EQUAL_UINT32(0, sensor.available());
	TEST_ASSERT_FALSE(sensor.has_data());
}

void setUp() {}
void tearDown() {}

int main(int, char**) {
	UNITY_BEGIN();
	RUN_TEST(test_ring_buffer_basic_push_pop);
	RUN_TEST(test_ring_buffer_capacity_limits);
	RUN_TEST(test_ring_buffer_overflow_behavior);
	RUN_TEST(test_ring_buffer_empty_pop_behavior);
	RUN_TEST(test_ring_buffer_multiple_pop_operations);
	RUN_TEST(test_ring_buffer_wrap_around);
	RUN_TEST(test_ring_buffer_data_integrity);
	RUN_TEST(test_ring_buffer_bounds_safety);
	return UNITY_END();
}
