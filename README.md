# ESPLowPowerSensor Library

## Overview
ESPLowPowerSensor is a library for managing low-power sensor operations on ESP32 and ESP8266 boards. It provides an easy-to-use interface for managing multiple sensors with different sampling intervals and trigger modes while optimizing power consumption through sleep modes. The library uses an interrupt-driven approach for efficient sensor management, ensuring precise timing and minimal power consumption.

## Features
- Support for both ESP32 and ESP8266 boards
- Two operational modes: Per-Sensor and Single-Interval
- Three trigger modes: Time Interval, Digital, and Analog
- Interrupt-driven approach for efficient and precise sensor management
- Queue system to handle multiple sensor interrupts
- WiFi credential management for easy configuration of wireless connectivity

## Installation
1. Download the library as a ZIP file
2. In the Arduino IDE, go to Sketch > Include Library > Add .ZIP Library
3. Select the downloaded ZIP file
4. The library is now installed and ready to use

## Usage
### Initialization
```cpp
#include <ESPLowPowerSensor.h>

ESPLowPowerSensor lowPowerSensor;

void setup() {
  // Initialize the library in Per-Sensor mode, without WiFi, using light sleep
  lowPowerSensor.init(ESPLowPowerSensor::Mode::PER_SENSOR, false, ESPLowPowerSensor::LowPowerMode::LIGHT_SLEEP);
}
```

### Adding Sensors
You can add sensors with different trigger modes:

1. Time Interval Trigger:
```cpp
lowPowerSensor.addSensor(readSensor, nullptr, ESPLowPowerSensor::TriggerMode::TIME_INTERVAL, 1000);
```

2. Digital Trigger:
```cpp
lowPowerSensor.addSensor(detectMotion, nullptr, ESPLowPowerSensor::TriggerMode::DIGITAL, LOW, MOTION_PIN);
```

3. Analog Trigger:
```cpp
lowPowerSensor.addSensor(readTemperature, nullptr, ESPLowPowerSensor::TriggerMode::ANALOG, 500, TEMPERATURE_PIN);
```

### Running the Sensor Manager
In your main loop, simply call the `run()` method:

```cpp
void loop() {
  lowPowerSensor.run();
}
```

## Example: Digital and Analog Triggers
Here's an example demonstrating the use of digital and analog triggers:

```cpp
#include <ESPLowPowerSensor.h>

ESPLowPowerSensor lowPowerSensor;

const int TEMPERATURE_PIN = A0;
const int MOTION_PIN = 2;

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
  
  lowPowerSensor.init(ESPLowPowerSensor::Mode::PER_SENSOR, false, ESPLowPowerSensor::LowPowerMode::LIGHT_SLEEP);
  
  // Add temperature sensor with analog trigger (threshold: 500)
  lowPowerSensor.addSensor(readTemperature, nullptr, ESPLowPowerSensor::TriggerMode::ANALOG, 500, TEMPERATURE_PIN);
  
  // Add motion sensor with digital trigger (trigger on LOW)
  lowPowerSensor.addSensor(detectMotion, nullptr, ESPLowPowerSensor::TriggerMode::DIGITAL, LOW, MOTION_PIN);
}

void loop() {
  lowPowerSensor.run();
}
```

This example sets up two sensors:
1. A temperature sensor that triggers when the analog reading exceeds 500.
2. A motion sensor that triggers when the digital pin reads LOW.

## Contributing
Contributions to the ESPLowPowerSensor library are welcome. Please submit pull requests or open issues on the GitHub repository.

## License
This library is released under the MIT License.