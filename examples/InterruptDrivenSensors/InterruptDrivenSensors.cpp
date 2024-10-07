#include <ESPLowPowerSensor.h>

ESPLowPowerSensor lowPowerSensor;

const int TEMPERATURE_PIN = A0;
const int MOTION_PIN = 2;

// Simulated sensor functions
void readTemperature() {
  int temperature = analogRead(TEMPERATURE_PIN);
  Serial.print("Temperature: ");
  Serial.println(temperature);
}

void detectMotion() {
  Serial.println("Motion detected!");
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect
  }

  pinMode(MOTION_PIN, INPUT_PULLUP);

  // Initialize the ESPLowPowerSensor in PER_SENSOR mode
  if (!lowPowerSensor.init(ESPLowPowerSensor::Mode::PER_SENSOR, false, ESPLowPowerSensor::LowPowerMode::LIGHT_SLEEP)) {
    Serial.println("Failed to initialize ESPLowPowerSensor");
    return;
  }

  // Add temperature sensor with analog trigger (threshold: 500)
  if (!lowPowerSensor.addSensor(readTemperature, nullptr, ESPLowPowerSensor::TriggerMode::ANALOG, 500, TEMPERATURE_PIN)) {
    Serial.println("Failed to add temperature sensor");
    return;
  }

  // Add motion sensor with digital trigger (trigger on LOW)
  if (!lowPowerSensor.addSensor(detectMotion, nullptr, ESPLowPowerSensor::TriggerMode::DIGITAL, LOW, MOTION_PIN)) {
    Serial.println("Failed to add motion sensor");
    return;
  }

  Serial.println("ESPLowPowerSensor initialized with analog and digital triggers");
}

void loop() {
  lowPowerSensor.run();
}
