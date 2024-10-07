#include <AUnit.h>
#include <ESPLowPowerSensor.h>

// Mock functions for WiFi and sleep operations
unsigned long mockSleepDuration = 0;
bool mockWifiStatus = false;

void mockGoToSleep(unsigned long sleepTime) {
    mockSleepDuration = sleepTime;
}

bool mockWifiOff() {
    mockWifiStatus = false;
    return true;
}

bool mockWifiOn() {
    mockWifiStatus = true;
    return true;
}

// Override the original functions with mock functions
#define goToSleep(x) mockGoToSleep(x)
#define wifiOff() mockWifiOff()
#define wifiOn() mockWifiOn()

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

test(addSensor_per_sensor_mode) {
  ESPLowPowerSensor sensor;
  assertTrue(sensor.init(ESPLowPowerSensor::Mode::PER_SENSOR, false, ESPLowPowerSensor::LowPowerMode::LIGHT_SLEEP));
  
  // Valid case: add sensor with wake function and interval
  assertTrue(sensor.addSensor([](){}, nullptr, 1000));
  
  // Invalid case: add sensor without interval in PER_SENSOR mode
  assertFalse(sensor.addSensor([](){}));
  
  // Invalid case: add sensor with zero interval in PER_SENSOR mode
  assertFalse(sensor.addSensor([](){}, nullptr, 0));
}

test(addSensor_single_interval_mode) {
  ESPLowPowerSensor sensor;
  assertTrue(sensor.init(ESPLowPowerSensor::Mode::SINGLE_INTERVAL, false, ESPLowPowerSensor::LowPowerMode::DEEP_SLEEP));
  
  // Valid case: add sensor without interval
  assertTrue(sensor.addSensor([](){}));
  
  // Valid case: add sensor with interval (should set single interval)
  assertTrue(sensor.addSensor([](){}, nullptr, 2000));
  
  // Invalid case: add sensor with different interval
  assertFalse(sensor.addSensor([](){}, nullptr, 3000));
}

test(addSensor_invalid_input) {
  ESPLowPowerSensor sensor;
  assertTrue(sensor.init(ESPLowPowerSensor::Mode::PER_SENSOR, false, ESPLowPowerSensor::LowPowerMode::LIGHT_SLEEP));
  
  // Invalid case: add sensor without wake function
  assertFalse(sensor.addSensor(nullptr, [](){}, 1000));
}

// Helper function to simulate passage of time
void simulateDelay(ESPLowPowerSensor& sensor, unsigned long ms) {
  for (unsigned long i = 0; i < ms; i += 10) {
    sensor.run();
    delay(10);
  }
}

test(run_per_sensor_mode) {
  ESPLowPowerSensor sensor;
  assertTrue(sensor.init(ESPLowPowerSensor::Mode::PER_SENSOR, false, ESPLowPowerSensor::LowPowerMode::LIGHT_SLEEP));
  
  int sensor1Count = 0;
  int sensor2Count = 0;
  
  assertTrue(sensor.addSensor([&sensor1Count](){ sensor1Count++; }, nullptr, 100));
  assertTrue(sensor.addSensor([&sensor2Count](){ sensor2Count++; }, nullptr, 200));
  
  simulateDelay(sensor, 250);
  
  assertEqual(2, sensor1Count);
  assertEqual(1, sensor2Count);
}

test(run_single_interval_mode) {
  ESPLowPowerSensor sensor;
  assertTrue(sensor.init(ESPLowPowerSensor::Mode::SINGLE_INTERVAL, false, ESPLowPowerSensor::LowPowerMode::DEEP_SLEEP));
  
  int sensor1Count = 0;
  int sensor2Count = 0;
  
  assertTrue(sensor.addSensor([&sensor1Count](){ sensor1Count++; }));
  assertTrue(sensor.addSensor([&sensor2Count](){ sensor2Count++; }));
  assertTrue(sensor.setSingleInterval(150));
  
  simulateDelay(sensor, 350);
  
  assertEqual(2, sensor1Count);
  assertEqual(2, sensor2Count);
}

test(goToSleep) {
  ESPLowPowerSensor sensor;
  sensor.init(ESPLowPowerSensor::Mode::PER_SENSOR, false, ESPLowPowerSensor::LowPowerMode::LIGHT_SLEEP);
  
  mockSleepDuration = 0;
  sensor.goToSleep(1000);
  assertEqual(1000UL, mockSleepDuration);
}

test(wifiOperations) {
  ESPLowPowerSensor sensor;
  sensor.init(ESPLowPowerSensor::Mode::PER_SENSOR, true, ESPLowPowerSensor::LowPowerMode::LIGHT_SLEEP);
  
  mockWifiStatus = true;
  assertTrue(sensor.wifiOff());
  assertFalse(mockWifiStatus);
  
  assertTrue(sensor.wifiOn());
  assertTrue(mockWifiStatus);
}

// More test cases will be added in subsequent tasks

test(runPerSensorMode) {
  ESPLowPowerSensor sensor;
  assertTrue(sensor.init(ESPLowPowerSensor::Mode::PER_SENSOR, false, ESPLowPowerSensor::LowPowerMode::LIGHT_SLEEP));
  
  int sensor1Count = 0;
  int sensor2Count = 0;
  
  assertTrue(sensor.addSensor([&sensor1Count](){ sensor1Count++; }, nullptr, 100));
  assertTrue(sensor.addSensor([&sensor2Count](){ sensor2Count++; }, nullptr, 200));
  
  // Simulate running for 250ms
  for (int i = 0; i < 25; i++) {
    sensor.run();
    delay(10);
  }
  
  assertEqual(2, sensor1Count);
  assertEqual(1, sensor2Count);
  
  // Simulate running for another 250ms
  for (int i = 0; i < 25; i++) {
    sensor.run();
    delay(10);
  }
  
  assertEqual(4, sensor1Count);
  assertEqual(2, sensor2Count);
}

test(runSingleIntervalMode) {
  ESPLowPowerSensor sensor;
  assertTrue(sensor.init(ESPLowPowerSensor::Mode::SINGLE_INTERVAL, false, ESPLowPowerSensor::LowPowerMode::DEEP_SLEEP));
  
  int sensor1Count = 0;
  int sensor2Count = 0;
  
  assertTrue(sensor.addSensor([&sensor1Count](){ sensor1Count++; }));
  assertTrue(sensor.addSensor([&sensor2Count](){ sensor2Count++; }));
  assertTrue(sensor.setSingleInterval(150));
  
  // Simulate running for 350ms
  for (int i = 0; i < 35; i++) {
    sensor.run();
    delay(10);
  }
  
  assertEqual(2, sensor1Count);
  assertEqual(2, sensor2Count);
  
  // Simulate running for another 150ms
  for (int i = 0; i < 15; i++) {
    sensor.run();
    delay(10);
  }
  
  assertEqual(3, sensor1Count);
  assertEqual(3, sensor2Count);
}

// Add these new test cases to the ESPLowPowerSensorTest.ino file

test(addManySensors) {
  ESPLowPowerSensor sensor;
  assertTrue(sensor.init(ESPLowPowerSensor::Mode::PER_SENSOR, false, ESPLowPowerSensor::LowPowerMode::LIGHT_SLEEP));
  
  const int maxSensors = 100; // Adjust this value based on the expected maximum number of sensors
  for (int i = 0; i < maxSensors; i++) {
    assertTrue(sensor.addSensor([](){ /* Do nothing */ }, nullptr, (i + 1) * 100));
  }
  
  // Try to add one more sensor, which should fail
  assertFalse(sensor.addSensor([](){ /* Do nothing */ }, nullptr, (maxSensors + 1) * 100));
}

test(veryShortInterval) {
  ESPLowPowerSensor sensor;
  assertTrue(sensor.init(ESPLowPowerSensor::Mode::PER_SENSOR, false, ESPLowPowerSensor::LowPowerMode::LIGHT_SLEEP));
  
  int sensorCount = 0;
  assertTrue(sensor.addSensor([&sensorCount](){ sensorCount++; }, nullptr, 1)); // 1ms interval
  
  simulateDelay(sensor, 10);
  
  // The sensor should have been called multiple times, but not necessarily 10 times due to execution time
  assertTrue(sensorCount > 1);
  assertTrue(sensorCount <= 10);
}

test(veryLongInterval) {
  ESPLowPowerSensor sensor;
  assertTrue(sensor.init(ESPLowPowerSensor::Mode::PER_SENSOR, false, ESPLowPowerSensor::LowPowerMode::LIGHT_SLEEP));
  
  int sensorCount = 0;
  assertTrue(sensor.addSensor([&sensorCount](){ sensorCount++; }, nullptr, 3600000)); // 1 hour interval
  
  simulateDelay(sensor, 10000); // Run for 10 seconds
  
  assertEqual(0, sensorCount); // The sensor should not have been called
}

test(mixedIntervals) {
  ESPLowPowerSensor sensor;
  assertTrue(sensor.init(ESPLowPowerSensor::Mode::PER_SENSOR, false, ESPLowPowerSensor::LowPowerMode::LIGHT_SLEEP));
  
  int shortCount = 0, mediumCount = 0, longCount = 0;
  assertTrue(sensor.addSensor([&shortCount](){ shortCount++; }, nullptr, 100));    // 100ms interval
  assertTrue(sensor.addSensor([&mediumCount](){ mediumCount++; }, nullptr, 250));  // 250ms interval
  assertTrue(sensor.addSensor([&longCount](){ longCount++; }, nullptr, 500));      // 500ms interval
  
  simulateDelay(sensor, 1000);
  
  assertEqual(10, shortCount);
  assertEqual(4, mediumCount);
  assertEqual(2, longCount);
}
