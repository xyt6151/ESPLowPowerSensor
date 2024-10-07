# ESPLowPowerSensor Library

## Overview
ESPLowPowerSensor is an Arduino library for ESP32 and ESP8266 boards, designed to simplify the implementation of low-power sensor projects. It provides an easy-to-use interface for managing multiple sensors with different sampling intervals while optimizing power consumption through sleep modes.

## Features
- Support for both ESP32 and ESP8266 boards
- Two operational modes: Per-Sensor and Single-Interval
- Configurable low-power modes: Light Sleep and Deep Sleep
- WiFi support with power-saving features
- Easy sensor management with customizable wake and sleep functions

## Installation
1. Download the library as a ZIP file
2. In the Arduino IDE, go to Sketch > Include Library > Add .ZIP Library
3. Select the downloaded ZIP file
4. The library is now installed and ready to use

## Usage
### Initialization
```
#include <ESPLowPowerSensor.h>

ESPLowPowerSensor lowPowerSensor;

void setup() {
  // Initialize the library
  lowPowerSensor.init(ESPLowPowerSensor::Mode::PER_SENSOR, false, ESPLowPowerSensor::LowPowerMode::DEEP_SLEEP);
  
  // Add sensors
  lowPowerSensor.addSensor(readSensor1, nullptr, 5000);  // 5 second interval
  lowPowerSensor.addSensor(readSensor2, nullptr, 10000); // 10 second interval
}

void loop() {
  lowPowerSensor.run();
}
```

### Adding Sensors
```
// Add a sensor with only a wake function
lowPowerSensor.addSensor(wakeFunctionPtr, nullptr, interval);

// Add a sensor with both wake and sleep functions
lowPowerSensor.addSensor(wakeFunctionPtr, sleepFunctionPtr, interval);
```

## Operational Modes
### Per-Sensor Mode
In this mode, each sensor has its own sampling interval. The ESP will wake up to handle each sensor independently.

### Single-Interval Mode
All sensors are sampled at a fixed interval. The ESP wakes up, samples all sensors, and then goes back to sleep.

## Low-Power Modes
### Deep Sleep
Offers the lowest power consumption but requires a full reboot on wake-up.

### Light Sleep
Provides faster wake-up times but with slightly higher power consumption. Note: Not available on ESP8266.

## Examples
See the \`examples\` folder for detailed usage examples:
- PerSensorMode: Basic usage of Per-Sensor mode
- SingleIntervalWiFi: Single-Interval mode with WiFi enabled
- PerSensorWithSleep: Per-Sensor mode with custom sleep functions

## Compatibility
- ESP32 boards
- ESP8266 boards

## License
This library is released under the MIT License.

## Contributing
Contributions to the ESPLowPowerSensor library are welcome. Please submit pull requests or open issues on the GitHub repository.