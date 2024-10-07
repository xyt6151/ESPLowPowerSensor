# ESPLowPowerSensor Library

## Overview
ESPLowPowerSensor is a library for managing low-power sensor operations on ESP32 and ESP8266 boards. It provides an easy-to-use interface for managing multiple sensors with different sampling intervals while optimizing power consumption through sleep modes. The library uses an interrupt-driven approach for efficient sensor management, ensuring precise timing and minimal power consumption.

## Features
- Support for both ESP32 and ESP8266 boards
- Two operational modes: Per-Sensor and Single-Interval
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

## WiFi Management

To use WiFi with the ESPLowPowerSensor library, you need to set the WiFi credentials before initializing the library. Use the `setWiFiCredentials` method to do this:

```cpp
ESPLowPowerSensor lowPowerSensor;

void setup() {
  // Set WiFi credentials
  lowPowerSensor.setWiFiCredentials("YourSSID", "YourPassword");

  // Initialize the library with WiFi enabled
  lowPowerSensor.init(ESPLowPowerSensor::Mode::PER_SENSOR, true, ESPLowPowerSensor::LowPowerMode::LIGHT_SLEEP);

  // ... rest of your setup code ...
}
```

The library will automatically handle WiFi connections when needed, based on the `wifiRequired` parameter in the `init` method.