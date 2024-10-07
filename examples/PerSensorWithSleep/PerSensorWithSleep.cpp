#include <ESPLowPowerSensor.h>

ESPLowPowerSensor lowPowerSensor;

void setup() {
  Serial.begin(115200);
  
  // Initialize the library in Per-Sensor mode, without WiFi, using deep sleep
  lowPowerSensor.init(ESPLowPowerSensor::Mode::PER_SENSOR, false, ESPLowPowerSensor::LowPowerMode::DEEP_SLEEP);
  
  // Add sensors with wake and sleep functions
  lowPowerSensor.addSensor(wakeAndReadSensor1, sleepSensor1, 15000);  // Sensor 1: 15 second interval
  lowPowerSensor.addSensor(wakeAndReadSensor2, sleepSensor2, 30000);  // Sensor 2: 30 second interval
}

void loop() {
  lowPowerSensor.run();
}

void wakeAndReadSensor1() {
  Serial.println("Waking and reading Sensor 1");
  // Add your sensor 1 wake-up and reading code here
}

void sleepSensor1() {
  Serial.println("Putting Sensor 1 to sleep");
  // Add your sensor 1 sleep code here
}

void wakeAndReadSensor2() {
  Serial.println("Waking and reading Sensor 2");
  // Add your sensor 2 wake-up and reading code here
}

void sleepSensor2() {
  Serial.println("Putting Sensor 2 to sleep");
  // Add your sensor 2 sleep code here
}
