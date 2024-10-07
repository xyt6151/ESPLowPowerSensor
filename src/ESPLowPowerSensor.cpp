#include <Arduino.h>
#include "ESPLowPowerSensor.h"
#include <algorithm> // Add this at the top of the file

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

ESPLowPowerSensor::ESPLowPowerSensor() : _mode(Mode::SINGLE_INTERVAL), _wifiRequired(false), _lowPowerMode(LowPowerMode::DEEP_SLEEP), _singleInterval(0), _interruptInProgress(false) {
    instance = this;
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
    if (_mode == Mode::PER_SENSOR && interval == 0) {
        Serial.println("Invalid interval for PER_SENSOR mode");
        return false;
    }

    if (_mode == Mode::SINGLE_INTERVAL) {
        if (_sensors.empty()) {
            _singleInterval = interval;
        } else if (interval != _singleInterval) {
            Serial.println("All sensors must have the same interval in SINGLE_INTERVAL mode");
            return false;
        }
    }

    Sensor newSensor = {wakeFunction, sleepFunction, interval, 0};
    _sensors.push_back(newSensor);

    // Set up timer interrupt for this sensor
    if (!setupTimerInterrupt(interval)) {
        Serial.println("Failed to set up timer interrupt for sensor");
        _sensors.pop_back(); // Remove the sensor if interrupt setup fails
        return false;
    }

    return true;
}

void ESPLowPowerSensor::run() {
    if (_mode == Mode::PER_SENSOR) {
        runPerSensorMode();
    } else {
        runSingleIntervalMode();
    }
    processInterruptQueue();  // Add this line
}

void ESPLowPowerSensor::runPerSensorMode() {
    unsigned long currentTime = millis();
    unsigned long nextWakeTime = ULONG_MAX;

    for (auto& sensor : _sensors) {
        unsigned long timeToWake = sensor.lastExecutionTime + sensor.interval - currentTime;
        if (timeToWake < nextWakeTime) {
            nextWakeTime = timeToWake;
        }
    }

    // Set up the timer interrupt for the next wake time
    setupTimerInterrupt(nextWakeTime);

    // Go to sleep
    goToSleep(nextWakeTime);
}

void ESPLowPowerSensor::runSingleIntervalMode() {
    // Set up the timer interrupt for the single interval
    setupTimerInterrupt(_singleInterval);

    // Go to sleep
    goToSleep(_singleInterval);
}

void ESPLowPowerSensor::goToSleep(unsigned long sleepTime) const {
    // Check if sleepTime is zero or negative
    if (sleepTime == 0) {
        return;
    }

    // Wait for any pending interrupt handlers to complete
    while (_interruptInProgress) {
        yield();  // Allow other tasks to run while waiting
    }

    if (_wifiRequired) {
        if (!wifiOff()) {
            // Handle WiFi turn off error (e.g., log it or set an error flag)
            // For now, we'll continue with sleep even if WiFi couldn't be turned off
        }
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
        if (!wifiOn()) {
            // Handle WiFi turn on error (e.g., log it or set an error flag)
            // For now, we'll continue even if WiFi couldn't be turned on
        }
    }
}

bool ESPLowPowerSensor::wifiOff() const {
    #if defined(ESP32)
    esp_err_t result = esp_wifi_stop();
    if (result != ESP_OK) {
        // Handle error (e.g., log it or set an error flag)
        return false;
    }
    #elif defined(ESP8266)
    WiFi.mode(WIFI_OFF);
    if (WiFi.forceSleepBegin() != WIFI_FORCE_SLEEP_BEGIN) {
        // Handle error (e.g., log it or set an error flag)
        return false;
    }
    #endif
    return true;
}

bool ESPLowPowerSensor::wifiOn() const {
    #if defined(ESP32)
    esp_err_t result = esp_wifi_start();
    if (result != ESP_OK) {
        // Handle error (e.g., log it or set an error flag)
        return false;
    }
    #elif defined(ESP8266)
    if (WiFi.forceSleepWake() != WIFI_FORCE_SLEEP_WAKEUP) {
        // Handle error (e.g., log it or set an error flag)
        return false;
    }
    delay(1); // Needed to ensure WiFi is fully awake
    #endif
    return true;
}

bool ESPLowPowerSensor::setMode(Mode newMode) {
    if (_mode == newMode) {
        return true; // Mode is already set, no change needed
    }

    // Check if there are any sensors added
    if (!_sensors.empty()) {
        // If changing to SINGLE_INTERVAL mode, ensure all sensors have the same interval
        if (newMode == Mode::SINGLE_INTERVAL) {
            unsigned long firstInterval = _sensors[0].interval;
            for (const auto& sensor : _sensors) {
                if (sensor.interval != firstInterval) {
                    return false; // Cannot change to SINGLE_INTERVAL mode with different intervals
                }
            }
            _singleInterval = firstInterval;
        }
        // If changing to PER_SENSOR mode, ensure all sensors have non-zero intervals
        else if (newMode == Mode::PER_SENSOR) {
            for (const auto& sensor : _sensors) {
                if (sensor.interval == 0) {
                    return false; // Cannot change to PER_SENSOR mode with zero intervals
                }
            }
        }
    }

    _mode = newMode;
    return true;
}

bool ESPLowPowerSensor::setupTimerInterrupt(unsigned long interval) {
    #if defined(ESP32)
    _timer = timerBegin(0, 80, true);  // Timer 0, prescaler 80, count up
    if (_timer == nullptr) {
        Serial.println("Failed to initialize timer");
        return false;
    }
    if (timerAttachInterrupt(_timer, &ESPLowPowerSensor::onTimerInterrupt, true) != ESP_OK) {
        Serial.println("Failed to attach timer interrupt");
        timerEnd(_timer);
        _timer = nullptr;
        return false;
    }
    if (timerAlarmWrite(_timer, interval * 1000, true) != ESP_OK) {
        Serial.println("Failed to set timer alarm");
        timerDetachInterrupt(_timer);
        timerEnd(_timer);
        _timer = nullptr;
        return false;
    }
    timerAlarmEnable(_timer);
    return true;
    #elif defined(ESP8266)
    try {
        _ticker.attach_ms(interval, ESPLowPowerSensor::onTimerInterrupt);
    } catch (const std::exception& e) {
        Serial.println("Failed to set up timer interrupt: " + String(e.what()));
        return false;
    }
    return true;
    #endif
}

// Implement the static ISR
void IRAM_ATTR ESPLowPowerSensor::onTimerInterrupt() {
    if (instance) {
        instance->handleInterrupt();
    }
}

// Implement the handleInterrupt method
void ESPLowPowerSensor::handleInterrupt() {
    _interruptInProgress = true;
    unsigned long currentTime = millis();

    // Queue sensor indices that need processing
    for (size_t i = 0; i < _sensors.size(); ++i) {
        if (currentTime - _sensors[i].lastExecutionTime >= _sensors[i].interval) {
            _interruptQueue.push(i);
        }
    }
    _interruptInProgress = false;
}

void ESPLowPowerSensor::processInterruptQueue() {
    // Process all queued sensor interrupts
    while (!_interruptQueue.empty()) {
        size_t sensorIndex = _interruptQueue.front();
        _interruptQueue.pop();

        if (sensorIndex < _sensors.size()) {
            auto& sensor = _sensors[sensorIndex];
            
            // Execute wake and sleep functions for the sensor
            if (sensor.wakeFunction) {
                sensor.wakeFunction();
            }
            if (sensor.sleepFunction) {
                sensor.sleepFunction();
            }

            sensor.lastExecutionTime = millis();
        }
    }
}