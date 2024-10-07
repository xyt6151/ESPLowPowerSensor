#include <AUnit.h>
#include <ESPLowPowerSensor.h>

void setup() {
  Serial.begin(115200);
  while (!Serial); // Wait for Serial to be ready - especially for Leonardo/Micro
}

void loop() {
  aunit::TestRunner::run();
}

test(constructor) {
  ESPLowPowerSensor sensor;
  // We can't directly test private members, so we'll test the behavior indirectly
  // by calling setMode() and checking if it returns true for SINGLE_INTERVAL mode
  assertTrue(sensor.setMode(ESPLowPowerSensor::Mode::SINGLE_INTERVAL));
}

test(init_per_sensor_mode) {
  ESPLowPowerSensor sensor;
  assertTrue(sensor.init(ESPLowPowerSensor::Mode::PER_SENSOR, false, ESPLowPowerSensor::LowPowerMode::LIGHT_SLEEP));
  // After initialization, adding a sensor with interval should succeed
  assertTrue(sensor.addSensor([](){}, nullptr, 1000));
}

test(init_single_interval_mode) {
  ESPLowPowerSensor sensor;
  assertTrue(sensor.init(ESPLowPowerSensor::Mode::SINGLE_INTERVAL, true, ESPLowPowerSensor::LowPowerMode::DEEP_SLEEP));
  // After initialization in SINGLE_INTERVAL mode, adding a sensor without interval should succeed
  assertTrue(sensor.addSensor([](){}));
}

test(init_invalid_mode) {
  ESPLowPowerSensor sensor;
  // Cast an invalid value to Mode to test error handling
  assertFalse(sensor.init(static_cast<ESPLowPowerSensor::Mode>(99), false, ESPLowPowerSensor::LowPowerMode::LIGHT_SLEEP));
}

// More test cases will be added in subsequent tasks
