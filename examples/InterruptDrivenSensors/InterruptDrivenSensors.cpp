#include <ESPLowPowerSensor.h>

ESPLowPowerSensor lowPowerSensor;

// Simulated sensor functions
void readTemperature() {
  Serial.println("Reading temperature...");
  // Simulate temperature reading
  delay(100);
  Serial.println("Temperature: 25°C");
}

void readHumidity() {
  Serial.println("Reading humidity...");
  // Simulate humidity reading
  delay(100);
  Serial.println("Humidity: 50%");
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect
  }

  // Set WiFi credentials
  lowPowerSensor.setWiFiCredentials("YourSSID", "YourPassword");

  // Initialize the ESPLowPowerSensor in PER_SENSOR mode with interrupts enabled
  if (!lowPowerSensor.init(ESPLowPowerSensor::Mode::PER_SENSOR, false, ESPLowPowerSensor::LowPowerMode::LIGHT_SLEEP)) {
    Serial.println("Failed to initialize ESPLowPowerSensor");
    return;
  }

  // Add temperature sensor (every 5 seconds)
  if (!lowPowerSensor.addSensor(readTemperature, nullptr, 5000)) {
    Serial.println("Failed to add temperature sensor");
    return;
  }

  // Add humidity sensor (every 10 seconds)
  if (!lowPowerSensor.addSensor(readHumidity, nullptr, 10000)) {
    Serial.println("Failed to add humidity sensor");
    return;
  }

  Serial.println("ESPLowPowerSensor initialized with interrupt-driven sensors");
}

void loop() {
  lowPowerSensor.run();
}
