#ifndef ESP_LOW_POWER_SENSOR_H
#define ESP_LOW_POWER_SENSOR_H

#include <Arduino.h>
#include <functional>
#include <vector>
#include <queue>  // Add this line
#include <atomic>  // Add this line at the top of the file
#include <array>  // Add this line
#define MAX_SENSORS 10 // Define the maximum number of sensors

// Include appropriate headers based on the ESP board being used
#if defined(ESP32)
#include <esp_sleep.h>
#include <esp_wifi.h>
#include "esp32-hal-timer.h"
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <Ticker.h>
#else
#error "This library only supports ESP32 and ESP8266 boards"
#endif

// Add this near the top of the file, after the includes
#define CIRCULAR_BUFFER_SIZE 32 // Adjust this size as needed

// Add this new class definition before the ESPLowPowerSensor class
class CircularBuffer {
private:
    size_t _buffer[CIRCULAR_BUFFER_SIZE];
    size_t _head;
    size_t _tail;
    bool _full;

public:
    CircularBuffer() : _head(0), _tail(0), _full(false) {}

    void push(size_t value) {
        _buffer[_head] = value;
        if (_full) {
            _tail = (_tail + 1) % CIRCULAR_BUFFER_SIZE;
        }
        _head = (_head + 1) % CIRCULAR_BUFFER_SIZE;
        _full = _head == _tail;
    }

    bool pop(size_t& value) {
        if (empty()) {
            return false;
        }
        value = _buffer[_tail];
        _full = false;
        _tail = (_tail + 1) % CIRCULAR_BUFFER_SIZE;
        return true;
    }

    bool empty() const {
        return (!_full && (_head == _tail));
    }

    bool full() const {
        return _full;
    }
};

/**
 * @class ESPLowPowerSensor
 * @brief A class to manage low-power sensor operations on ESP32 and ESP8266 boards.
 *
 * This class provides functionality to manage multiple sensors with different
 * sampling intervals while optimizing power consumption through sleep modes.
 * It uses an interrupt-driven approach for efficient sensor management.
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

    /**
     * @brief Sets up a timer interrupt for the specified interval.
     * @param interval The interval in milliseconds for the timer interrupt.
     * @return True if the timer was successfully set up, false otherwise.
     */
    bool setupTimerInterrupt(unsigned long interval);

    /**
     * @brief Disables interrupts and reverts to the original behavior.
     * @return True if interrupts were successfully disabled, false otherwise.
     */
    bool disableInterrupts();

    /**
     * @brief Returns the number of sensors currently managed.
     * @return The number of sensors.
     */
    size_t getSensorCount() const;

    /**
     * @brief Gets the current operational mode.
     * @return The current Mode.
     */
    inline Mode getMode() const { return _mode; }

    /**
     * @brief Checks if WiFi is required for sensor operations.
     * @return True if WiFi is required, false otherwise.
     */
    inline bool isWifiRequired() const { return _wifiRequired; }

    /**
     * @brief Gets the current low power mode.
     * @return The current LowPowerMode.
     */
    inline LowPowerMode getLowPowerMode() const { return _lowPowerMode; }

    /**
     * @brief Gets the single interval used in SINGLE_INTERVAL mode.
     * @return The single interval in milliseconds.
     */
    inline unsigned long getSingleInterval() const { return _singleInterval; }

    /**
     * @brief Checks if interrupts are currently enabled.
     * @return True if interrupts are enabled, false otherwise.
     */
    inline bool areInterruptsEnabled() const { return _interruptsEnabled; }

    /**
     * @brief Gets the number of sensors currently managed.
     * @return The number of sensors.
     */
    inline size_t getSensorCount() const { return _sensorCount; }

private:
    Mode _mode;                      ///< Current operational mode
    bool _wifiRequired;              ///< Whether WiFi is required during sensor operations
    LowPowerMode _lowPowerMode;      ///< Current low-power mode
    std::array<Sensor, MAX_SENSORS> _sensors;  ///< Collection of managed sensors
    unsigned long _singleInterval;   ///< Interval used in SINGLE_INTERVAL mode

    #if defined(ESP32)
    hw_timer_t* _timer = nullptr;
    #elif defined(ESP8266)
    Ticker _ticker;
    #endif

    static ESPLowPowerSensor* instance;
    static void IRAM_ATTR onTimerInterrupt();
    void handleInterrupt();

    // Replace this line:
    // std::queue<size_t> _interruptQueue;
    // With this:
    CircularBuffer _interruptQueue;  ///< Queue to store sensor indices for interrupt processing
    void processInterruptQueue();  ///< Process the queue of sensor interrupts

    std::atomic<bool> _interruptInProgress;  ///< Flag to indicate if an interrupt is being processed

    bool _interruptsEnabled;  ///< Flag to indicate if interrupts are enabled

    std::atomic<bool> _interruptOccurred;  ///< Flag to indicate if an interrupt has occurred

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