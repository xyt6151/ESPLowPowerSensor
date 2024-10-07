#include <Arduino.h>
#include "ESPLowPowerSensor.h"

// put function declarations here:
int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:
  int result = myFunction(2, 3);
}

void loop() {
  // put your main code here, to run repeatedly:
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}

ESPLowPowerSensor::ESPLowPowerSensor() : _mode(Mode::SINGLE_INTERVAL), _wifiRequired(false), _lowPowerMode(LowPowerMode::DEEP_SLEEP), _singleInterval(0) {
    // Constructor initializes default values for member variables
}

bool ESPLowPowerSensor::init(Mode mode, bool wifiRequired, LowPowerMode lowPowerMode) {
    _mode = mode;
    _wifiRequired = wifiRequired;
    _lowPowerMode = lowPowerMode;

    // Perform any necessary setup based on the selected mode and options
    switch (_mode) {
        case Mode::PER_SENSOR:
            // Setup for per-sensor mode
            break;
        case Mode::SINGLE_INTERVAL:
            // Setup for single-interval mode
            break;
    }

    // Configure low-power mode
    if (_lowPowerMode == LowPowerMode::LIGHT_SLEEP) {
        #if defined(ESP32)
        // ESP32 supports light sleep, no changes needed
        #elif defined(ESP8266)
        // ESP8266 doesn't support light sleep, fallback to deep sleep
        _lowPowerMode = LowPowerMode::DEEP_SLEEP;
        #endif
    }

    // Configure WiFi if required
    if (_wifiRequired) {
        #if defined(ESP32)
        esp_wifi_start();
        #elif defined(ESP8266)
        WiFi.mode(WIFI_STA);
        WiFi.begin();
        #endif
    }

    return true; // Return false if any initialization fails
}

bool ESPLowPowerSensor::addSensor(std::function<void()> wakeFunction, std::function<void()> sleepFunction, unsigned long interval) {
    // Input validation for interval
    if (_mode == Mode::PER_SENSOR && interval == 0) {
        return false; // Interval is required for per-sensor mode
    }
    
    if (_mode == Mode::SINGLE_INTERVAL && interval > 0 && _singleInterval > 0 && interval != _singleInterval) {
        return false; // In single-interval mode, all intervals must be the same (if set)
    }

    // Ensure wakeFunction is provided
    if (!wakeFunction) {
        return false; // Wake function is required
    }

    Sensor newSensor = {
        .wakeFunction = wakeFunction,
        .sleepFunction = sleepFunction,
        .interval = interval,
        .lastExecutionTime = 0
    };

    _sensors.push_back(newSensor);

    if (_mode == Mode::SINGLE_INTERVAL) {
        if (_singleInterval == 0 || interval < _singleInterval) {
            _singleInterval = interval;
        }
    }

    return true;
}

void ESPLowPowerSensor::run() {
    if (_mode == Mode::PER_SENSOR) {
        runPerSensorMode();
    } else {
        runSingleIntervalMode();
    }
}

void ESPLowPowerSensor::runPerSensorMode() const {
    unsigned long currentTime = millis();
    unsigned long nextWakeTime = ULONG_MAX;

    for (auto& sensor : _sensors) {
        if (currentTime - sensor.lastExecutionTime >= sensor.interval) {
            // Execute wake function
            if (sensor.wakeFunction) {
                sensor.wakeFunction();
            }

            // Execute sleep function
            if (sensor.sleepFunction) {
                sensor.sleepFunction();
            }

            sensor.lastExecutionTime = currentTime;
        }

        // Calculate next wake time
        unsigned long sensorNextWake = sensor.lastExecutionTime + sensor.interval;
        if (sensorNextWake < nextWakeTime) {
            nextWakeTime = sensorNextWake;
        }
    }

    // Calculate sleep duration
    unsigned long sleepDuration = nextWakeTime - currentTime;
    goToSleep(sleepDuration);
}

void ESPLowPowerSensor::runSingleIntervalMode() const {
    unsigned long currentTime = millis();

    // Check if it's time to execute sensor functions
    if (currentTime - _sensors[0].lastExecutionTime >= _singleInterval) {
        // Execute wake functions for all sensors
        for (auto& sensor : _sensors) {
            if (sensor.wakeFunction) {
                sensor.wakeFunction();
            }
        }

        // Allow some time for sensor operations
        delay(100);  // Adjust this delay as needed

        // Execute sleep functions for all sensors
        for (auto& sensor : _sensors) {
            if (sensor.sleepFunction) {
                sensor.sleepFunction();
            }
            sensor.lastExecutionTime = currentTime;
        }

        // Go to sleep for the single interval duration
        goToSleep(_singleInterval);
    }
}

void ESPLowPowerSensor::goToSleep(unsigned long sleepTime) const {
    if (_wifiRequired) {
        wifiOff();
    }

    if (_lowPowerMode == LowPowerMode::DEEP_SLEEP) {
        #if defined(ESP32)
        esp_sleep_enable_timer_wakeup(sleepTime * 1000ULL); // Convert to microseconds
        esp_deep_sleep_start();
        #elif defined(ESP8266)
        ESP.deepSleep(sleepTime * 1000ULL); // Convert to microseconds
        #endif
    } else { // LIGHT_SLEEP
        #if defined(ESP32)
        esp_sleep_enable_timer_wakeup(sleepTime * 1000ULL); // Convert to microseconds
        esp_light_sleep_start();
        #elif defined(ESP8266)
        // ESP8266 doesn't support light sleep, so we use delay instead
        delay(sleepTime);
        #endif
    }

    if (_wifiRequired) {
        wifiOn();
    }
}

void ESPLowPowerSensor::wifiOff() const {
    #if defined(ESP32)
    esp_wifi_stop();
    #elif defined(ESP8266)
    WiFi.mode(WIFI_OFF);
    WiFi.forceSleepBegin();
    #endif
}

void ESPLowPowerSensor::wifiOn() const {
    #if defined(ESP32)
    esp_wifi_start();
    #elif defined(ESP8266)
    WiFi.forceSleepWake();
    delay(1); // Needed to ensure WiFi is fully awake
    #endif
}