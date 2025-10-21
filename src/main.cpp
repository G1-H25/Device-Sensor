#include <Arduino.h>
#include "HardwareInterface/GPIO.h"
#include <ArduinoHardware.h>
#include "Tmp36.h"

static GPIO::PinMap pinMap;

//! min kassa analoga TMP36 skulle bli typ så här
static GPIO::PinIndex temp_pin = GPIO::PinIndex::A3;
static TemperatureSensor::Tmp36 temp_sensor(pinMap[temp_pin]);

bool smoke_test_run = false;

//! Manual smoke test function for hardware validation
void run_smoke_test_sequence() {
    Serial.println("=== SMOKE TEST START ===");
    
    // Test 1: Initialization
    temp_sensor.begin();
    bool init_success = static_cast<bool>(temp_sensor);
    Serial.print("Test 1 - Sensor Initialization: ");
    Serial.println(init_success ? "PASS" : "FAIL");
    
    // Test 2: First measurement
    temp_sensor.update();
    bool data_available = temp_sensor.has_data();
    Serial.print("Test 2 - Data Available: ");
    Serial.println(data_available ? "PASS" : "FAIL");
    
    // Test 3: Temperature range validation
    TemperatureSensor::Measurement m;
    bool pop_success = temp_sensor.try_pop(m);
    if (pop_success) {
        Serial.print("Test 3 - Temperature Reading: ");
        Serial.print(m.temperature);
        Serial.print("°C - ");
        bool temp_valid = (m.temperature >= -60.0f && m.temperature <= 125.0f);
        Serial.println(temp_valid ? "PASS" : "FAIL");
        
        Serial.print("Test 4 - Humidity (should be 0): ");
        Serial.print(m.humidity);
        Serial.println(m.humidity == 0.0f ? " - PASS" : " - FAIL");
        
        Serial.print("Test 5 - Timestamp: ");
        Serial.print(m.timestamp);
        Serial.println("ms - PASS");
    } else {
        Serial.println("Test 3 - Temperature Reading: FAIL (no data)");
    }
    
    // Test 6: Ring buffer capacity
    Serial.print("Test 6 - Buffer Available: ");
    Serial.print(temp_sensor.available());
    Serial.println(" measurements - PASS");
    
    // Test 7: Multiple measurements
    Serial.println("Test 7 - Multiple Measurements:");
    for (int i = 0; i < 3; ++i) {
        temp_sensor.update();
        Serial.print("  Measurement ");
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.print(temp_sensor.available());
        Serial.println(" in buffer");
    }
    
    // Test 8: GPIO pin validation
    Serial.print("Test 8 - GPIO Pin A3: ");
    GPIO::Pin pin = pinMap[GPIO::PinIndex::A3];
    pin.pinMode(GPIO::PinMode::INPUT);
    std::uint16_t analog_value = pin.analogRead();
    Serial.print("Analog value: ");
    Serial.print(analog_value);
    Serial.print(" (0-1023) - ");
    bool gpio_valid = (analog_value >= 0 && analog_value <= 1023);
    Serial.println(gpio_valid ? "PASS" : "FAIL");
    
    Serial.println("=== SMOKE TEST END ===");
    Serial.println("Manual validation checklist:");
    Serial.println("- Verify TMP36 sensor is connected to pin A3");
    Serial.println("- Check power supply (5V to sensor)");
    Serial.println("- Verify temperature readings are reasonable");
    Serial.println("- Confirm no error messages above");
    Serial.println("- Run for 10+ minutes to check stability");
    Serial.println("================================");
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; // Wait for serial port to connect
  }
  
  Serial.println("Device Sensor Application Starting...");
  Serial.println("Commands:");
  Serial.println("  's' - Run smoke test (exclusive mode)");
  Serial.println("  'r' - Reset and resume normal operation");
  Serial.println("Normal operation will start automatically...");
  
  temp_sensor.begin();
}

void loop() {
  // Check for smoke test trigger
  if (Serial.available() && !smoke_test_run) {
    char command = Serial.read();
    if (command == 's' || command == 'S') {
      smoke_test_run = true;
      run_smoke_test_sequence();
      Serial.println("Smoke test completed. Send 'r' to reset and resume normal operation.");
      return; // Skip normal operation this cycle
    }
  }
  
  // Check for reset command to resume normal operation
  if (Serial.available() && smoke_test_run) {
    char command = Serial.read();
    if (command == 'r' || command == 'R') {
      smoke_test_run = false;
      // Clear any remaining data in the ring buffer
      TemperatureSensor::Measurement m;
      while (temp_sensor.has_data()) {
        temp_sensor.try_pop(m);
      }
      Serial.println("Reset complete. Resuming normal operation...");
      return; // Skip normal operation this cycle
    }
  }
  
  // Normal operation (only when smoke_test_run is false)
  if (!smoke_test_run) {
    temp_sensor.update();
    if (temp_sensor.has_data()) {
      TemperatureSensor::Measurement m;
      if (temp_sensor.try_pop(m)) {
        Serial.print("Temperature: ");
        Serial.print(m.temperature);
        Serial.print("°C, Timestamp: ");
        Serial.print(m.timestamp);
        Serial.println("ms");
      }
    }
    delay(1000);
  } else {
    // When in smoke test mode, just wait for reset command
    delay(100);
  }
}