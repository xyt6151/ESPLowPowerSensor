#include <Arduino.h>
#include "ESPLowPowerSensor.h"
#include <algorithm>

void setup(){}
void loop(){}

ESPLowPowerSensor::ESPLowPowerSensor() 
    : _mode(Mode::SINGLE_INTERVAL), 
      _wifiRequired(false), 
      _lowPowerMode(LowPowerMode::DEEP_SLEEP), 
      _singleInterval(0), 
      _interruptInProgress(false),
      _interruptsEnabled(true),
      _sensorCount(0),
      _wifiInitialized(false),
      _lastExecutionTime(0) {
    instance = this;
}

bool ESPLowPowerSensor::initialize(Mode mode, bool wifiRequired, LowPowerMode lowPowerMode) {
    _mode = mode;
    _wifiRequired = wifiRequired;
    _lowPowerMode = lowPowerMode;

    // Perform any necessary setup based on the selected mode and options
    switch(_mode) {
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
        if (!initializeWifi()) {
            return false;
        }
    }

    return true; // Return false if any initialization fails
}

bool ESPLowPowerSensor::addSensor(std::function<void()> wakeFunction, 
                                  std::function<void()> sleepFunction, 
                                  TriggerMode triggerMode, 
                                  unsigned long intervalOrThreshold,
                                  uint8_t pin) {
    if (_sensorCount >= MAX_SENSORS) {
        Serial.println("Maximum number of sensors reached");
        return false;
    }

    if (!wakeFunction) {
        Serial.println("Wake function is required");
        return false;
    }

    if (_mode == Mode::PER_SENSOR && triggerMode == TriggerMode::TIME_INTERVAL && intervalOrThreshold == 0) {
        Serial.println("Invalid interval for PER_SENSOR mode");
        return false;
    }

    if (_mode == Mode::SINGLE_INTERVAL) {
        if (_sensorCount == 0) {
            _singleInterval = intervalOrThreshold;
        } else if (intervalOrThreshold != _singleInterval) {
            Serial.println("All sensors must have the same interval in SINGLE_INTERVAL mode");
            return false;
        }
    }

    Sensor newSensor;
    newSensor.wakeFunction = wakeFunction;
    newSensor.sleepFunction = sleepFunction;
    newSensor.triggerMode = triggerMode;
    newSensor.lastExecutionTime = 0;
    newSensor.pin = pin;

    switch (triggerMode) {
        case TriggerMode::TIME_INTERVAL:
            newSensor.triggerValue.interval = intervalOrThreshold;
            break;
        case TriggerMode::DIGITAL:
            newSensor.triggerValue.digitalValue = intervalOrThreshold != 0;
            pinMode(pin, INPUT);
            break;
        case TriggerMode::ANALOG_TRIGGER:
            newSensor.triggerValue.analogValue = intervalOrThreshold;
            pinMode(pin, INPUT);
            break;
    }

    _sensors[_sensorCount++] = newSensor;
    return true;
}

void ESPLowPowerSensor::run() {
    if (_mode == Mode::PER_SENSOR) {
        runPerSensorMode();
    } else {
        runSingleIntervalMode();
    }
}

void ESPLowPowerSensor::runPerSensorMode() {
    unsigned long currentTime = millis();
    for (size_t i = 0; i < _sensorCount; ++i) {
        auto& sensor = _sensors[i];
        bool shouldExecute = false;

        switch (sensor.triggerMode) {
            case TriggerMode::TIME_INTERVAL:
                shouldExecute = (currentTime - sensor.lastExecutionTime >= sensor.triggerValue.interval);
                break;
            case TriggerMode::DIGITAL:
                shouldExecute = checkDigitalTrigger(sensor);
                break;
            case TriggerMode::ANALOG_TRIGGER:
                shouldExecute = checkAnalogTrigger(sensor);
                break;
        }

        if (shouldExecute) {
            executeSensor(i);
        }
    }
}

void ESPLowPowerSensor::runSingleIntervalMode() {
    unsigned long currentTime = millis();
    if (currentTime - _lastExecutionTime >= _singleInterval || currentTime < _lastExecutionTime) {
        for (size_t i = 0; i < _sensorCount; ++i) {
            executeSensor(i);
        }
        _lastExecutionTime = currentTime;
    }
    goToSleep(_singleInterval - (currentTime - _lastExecutionTime));
}

void ESPLowPowerSensor::executeSensor(size_t index) {
    if (index >= _sensorCount) {
        return;
    }

    auto& sensor = _sensors[index];
    
    if (sensor.wakeFunction) {
        sensor.wakeFunction();
    }
    
    // Perform any necessary operations here
    
    if (sensor.sleepFunction) {
        sensor.sleepFunction();
    }
    
    sensor.lastExecutionTime = millis();
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
        // ESP8266 doesn't support light sleep, so we use a power-efficient delay
        uint32_t start = micros();
        while (micros() - start < sleepTime * 1000ULL) {
            ESP.wdtFeed(); // Feed the watchdog timer
            yield(); // Allow background tasks to run
        }
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
    if (!_wifiRequired) {
        return true;
    }

    #if defined(ESP32)
    return WiFi.disconnect(true);
    #elif defined(ESP8266)
    return WiFi.forceSleepBegin();
    #endif
}

bool ESPLowPowerSensor::wifiOn() const {
    if (!_wifiRequired) {
        return true;
    }

    if (!_wifiInitialized) {
        return initializeWifi();
    }

    #if defined(ESP32)
    return WiFi.begin();
    #elif defined(ESP8266)
    return WiFi.forceSleepWake() == WIFI_FORCE_SLEEP_WAKEUP;
    #endif
}

bool ESPLowPowerSensor::setMode(Mode newMode) {
    if (_mode == newMode) {
        return true; // Mode is already set, no change needed
    }

    // Check if there are any sensors added
    if (_sensorCount > 0) {
        // If changing to SINGLE_INTERVAL mode, ensure all sensors have the same interval
        if (newMode == Mode::SINGLE_INTERVAL) {
            unsigned long firstInterval = _sensors[0].triggerValue.interval;
            for (size_t i = 0; i < _sensorCount; ++i) {
                if (_sensors[i].triggerValue.interval != firstInterval) {
                    return false; // Cannot change to SINGLE_INTERVAL mode with different intervals
                }
            }
            _singleInterval = firstInterval;
        }
        // If changing to PER_SENSOR mode, ensure all sensors have non-zero intervals
        else if (newMode == Mode::PER_SENSOR) {
            for (size_t i = 0; i < _sensorCount; ++i) {
                if (_sensors[i].triggerValue.interval == 0) {
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
    timerAttachInterrupt(_timer, &ESPLowPowerSensor::onTimerInterrupt, true);
    timerAlarmWrite(_timer, interval * 1000, true);
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
        instance->_interruptOccurred = true;
    }
}

// Modify the handleInterrupt method
void ESPLowPowerSensor::handleInterrupt() {
    _interruptInProgress = true;
    _interruptOccurred = true;

    unsigned long currentTime = millis();
    for (size_t i = 0; i < _sensorCount; ++i) {
        if (currentTime - _sensors[i].lastExecutionTime >= _sensors[i].triggerValue.interval) {
            if (!_interruptQueue.full()) {
                _interruptQueue.push(i);
            } else {
                // Handle queue overflow (e.g., log an error)
                Serial.println("Interrupt queue overflow");
            }
        }
    }

    _interruptOccurred = false;
    _interruptInProgress = false;
}

void ESPLowPowerSensor::processInterruptQueue() {
    size_t sensorIndex;
    while (_interruptQueue.pop(sensorIndex)) {
        if (sensorIndex < _sensorCount) {
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

bool ESPLowPowerSensor::disableInterrupts() {
    if (!_interruptsEnabled) {
        return true;  // Interrupts are already disabled
    }

    #if defined(ESP32)
    if (_timer != nullptr) {
        timerAlarmDisable(_timer);
        timerDetachInterrupt(_timer);
        timerEnd(_timer);
        _timer = nullptr;
    }
    #elif defined(ESP8266)
    _ticker.detach();
    #endif

    _interruptsEnabled = false;
    return true;
}

void ESPLowPowerSensor::setWiFiCredentials(const char* ssid, const char* password) {
    _wifiSSID = ssid;
    _wifiPassword = password;
}

bool ESPLowPowerSensor::initializeWifi() const {
    if (!_wifiRequired) {
        return true;
    }

    if (_wifiSSID == nullptr || _wifiPassword == nullptr) {
        Serial.println("WiFi credentials not set. Call setWiFiCredentials before init.");
        return false;
    }

    #if defined(ESP32) || defined(ESP8266)
    WiFi.mode(WIFI_STA);
    WiFi.begin(_wifiSSID, _wifiPassword);
    // Wait for connection
    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED) {
        if (millis() - startTime > 10000) { // 10 second timeout
            Serial.println("Failed to connect to WiFi");
            return false;
        }
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected");
    #endif

    return true;
}

bool ESPLowPowerSensor::checkDigitalTrigger(const Sensor& sensor) {
    return digitalRead(sensor.pin) == sensor.triggerValue.digitalValue;
}

bool ESPLowPowerSensor::checkAnalogTrigger(const Sensor& sensor) {
    return analogRead(sensor.pin) >= sensor.triggerValue.analogValue;
}