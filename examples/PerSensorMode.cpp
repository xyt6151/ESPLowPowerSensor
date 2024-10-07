#include <ESPLowPowerSensor.h>

ESPLowPowerSensor lowPowerSensor;

void setup() {
  Serial.begin(115200);
  
  // Initialize the library in Per-Sensor mode, without WiFi, using deep sleep
  lowPowerSensor.init(ESPLowPowerSensor::Mode::PER_SENSOR, false, ESPLowPowerSensor::LowPowerMode::DEEP_SLEEP);
  
  // Add two sensors with different intervals
  lowPowerSensor.addSensor(readSensor1, nullptr, 5000);  // Sensor 1: 5 second interval
  lowPowerSensor.addSensor(readSensor2, nullptr, 10000); // Sensor 2: 10 second interval
}

void loop() {
  lowPowerSensor.run();
}

void readSensor1() {
  Serial.println("Reading Sensor 1");
  // Add your sensor 1 reading code here
}

void readSensor2() {
  Serial.println("Reading Sensor 2");
  // Add your sensor 2 reading code here
}
