#include <unity.h>
#include "TemperatureSensor/TemperatureSensor.h"
#include "Tmp36.h"

using TemperatureSensor::Measurement;

void test_ring_buffer_push_pop() {
	GPIO::PinMap pins;
	TemperatureSensor::Tmp36 sensor(pins[static_cast<GPIO::PinIndex>(0)]);
	sensor.begin();

	Measurement m{};
	m.timestamp = 1;
	m.temperature = 25.0f;
	m.humidity = 0.0f;

	// simulate push via protected helper through read path
	TEST_ASSERT_EQUAL_UINT32(0, sensor.available());
	sensor.read();
	TEST_ASSERT_TRUE(sensor.has_data());
	TEST_ASSERT_EQUAL_UINT32(1, sensor.available());

	Measurement out{};
	TEST_ASSERT_TRUE(sensor.try_pop(out));
	TEST_ASSERT_TRUE(sensor.available() == 0);
}

void setUp() {}
void tearDown() {}

int main(int, char**) {
	UNITY_BEGIN();
	RUN_TEST(test_ring_buffer_push_pop);
	return UNITY_END();
}
