#ifndef ESP_LOW_POWER_SENSOR_H
#define ESP_LOW_POWER_SENSOR_H

#include <Arduino.h>
#include <functional>
#include <vector>

// Include appropriate headers based on the ESP board being used
#if defined(ESP32)
#include <esp_sleep.h>
#include <esp_wifi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#else
#error "This library only supports ESP32 and ESP8266 boards"
#endif

/**
 * @class ESPLowPowerSensor
 * @brief A class to manage low-power sensor operations on ESP32 and ESP8266 boards.
 *
 * This class provides functionality to manage multiple sensors with different
 * sampling intervals while optimizing power consumption through sleep modes.
 */
class ESPLowPowerSensor {
public:
    /**
     * @enum Mode
     * @brief Defines the operational mode of the ESPLowPowerSensor.
     */
    enum class Mode {
        PER_SENSOR,      ///< Each sensor has its own sampling interval
        SINGLE_INTERVAL  ///< All sensors are sampled at a fixed interval
    };

    /**
     * @enum LowPowerMode
     * @brief Defines the low-power mode to be used.
     */
    enum class LowPowerMode {
        LIGHT_SLEEP,  ///< Light sleep mode (faster wake-up, slightly higher power consumption)
        DEEP_SLEEP    ///< Deep sleep mode (slower wake-up, lowest power consumption)
    };

    /**
     * @struct Sensor
     * @brief Represents a sensor with its associated functions and timing information.
     */
    struct Sensor {
        std::function<void()> wakeFunction;    ///< Function to be called when the sensor wakes up
        std::function<void()> sleepFunction;   ///< Function to be called before the sensor goes to sleep
        unsigned long interval;                ///< Sampling interval for the sensor
        unsigned long lastExecutionTime;       ///< Last time the sensor functions were executed
    };

    /**
     * @brief Default constructor.
     */
    ESPLowPowerSensor();

    /**
     * @brief Initializes the ESPLowPowerSensor with the specified parameters.
     * @param mode The operational mode (PER_SENSOR or SINGLE_INTERVAL).
     * @param wifiRequired Whether WiFi is required during sensor operations.
     * @param lowPowerMode The low-power mode to be used (LIGHT_SLEEP or DEEP_SLEEP).
     * @return True if initialization was successful, false otherwise.
     */
    bool init(Mode mode, bool wifiRequired, LowPowerMode lowPowerMode);

    /**
     * @brief Adds a sensor to be managed by the ESPLowPowerSensor.
     * @param wakeFunction Function to be called when the sensor wakes up.
     * @param sleepFunction Function to be called before the sensor goes to sleep (optional).
     * @param interval Sampling interval for the sensor (required for PER_SENSOR mode).
     * @return True if the sensor was successfully added, false otherwise.
     */
    bool addSensor(std::function<void()> wakeFunction, std::function<void()> sleepFunction = nullptr, unsigned long interval = 0);

    /**
     * @brief Runs the main loop of the ESPLowPowerSensor.
     *
     * This function should be called repeatedly in the Arduino loop() function.
     */
    void run();

    /**
     * @brief Changes the operating mode after initialization.
     * @param newMode The new operational mode (PER_SENSOR or SINGLE_INTERVAL).
     * @return True if the mode was successfully changed, false otherwise.
     */
    bool setMode(Mode newMode);

private:
    Mode _mode;                      ///< Current operational mode
    bool _wifiRequired;              ///< Whether WiFi is required during sensor operations
    LowPowerMode _lowPowerMode;      ///< Current low-power mode
    std::vector<Sensor> _sensors;    ///< Collection of managed sensors
    unsigned long _singleInterval;   ///< Interval used in SINGLE_INTERVAL mode

    /**
     * @brief Runs the ESPLowPowerSensor in PER_SENSOR mode.
     */
    void runPerSensorMode();

    /**
     * @brief Runs the ESPLowPowerSensor in SINGLE_INTERVAL mode.
     */
    void runSingleIntervalMode();

    /**
     * @brief Puts the ESP into sleep mode for the specified duration.
     * @param sleepTime Duration to sleep in milliseconds.
     */
    void goToSleep(unsigned long sleepTime) const;

    /**
     * @brief Turns off WiFi to conserve power.
     * @return True if WiFi was successfully turned off, false otherwise.
     */
    bool wifiOff() const;

    /**
     * @brief Turns on WiFi when needed for sensor operations.
     * @return True if WiFi was successfully turned on, false otherwise.
     */
    bool wifiOn() const;
};

#endif // ESP_LOW_POWER_SENSOR_H