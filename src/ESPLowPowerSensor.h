#ifndef ESP_LOW_POWER_SENSOR_H
#define ESP_LOW_POWER_SENSOR_H

#include <Arduino.h>
#include <functional>
#include <vector>

#if defined(ESP32)
#include <esp_sleep.h>
#include <esp_wifi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#else
#error "This library only supports ESP32 and ESP8266 boards"
#endif

class ESPLowPowerSensor {
public:
    enum class Mode {
        PER_SENSOR,
        SINGLE_INTERVAL
    };

    enum class LowPowerMode {
        LIGHT_SLEEP,
        DEEP_SLEEP
    };

    struct Sensor {
        std::function<void()> wakeFunction;
        std::function<void()> sleepFunction;
        unsigned long interval;
        unsigned long lastExecutionTime;
    };

    ESPLowPowerSensor();
    bool init(Mode mode, bool wifiRequired, LowPowerMode lowPowerMode);
    bool addSensor(std::function<void()> wakeFunction, std::function<void()> sleepFunction = nullptr, unsigned long interval = 0);

private:
    Mode _mode;
    bool _wifiRequired;
    LowPowerMode _lowPowerMode;
    std::vector<Sensor> _sensors;
    unsigned long _singleInterval;

    void goToSleep(unsigned long sleepTime);
    void wifiOff();
    void wifiOn();
};

#endif // ESP_LOW_POWER_SENSOR_H
