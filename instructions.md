# ESPLowPowerSensor: Instructions

This project is an ESP32/ESP8266 library for use in sensor projects where low power consumption is required. The library should be designed in a way that it is usable with any sensor that transmits data via SPI, I2C, digital pins or analog pins. The compatibility of a sensor with this library should not be affected by whether the sensor simply sends data to the ESP, or if the ESP also needs to send data to the sensor. The user will create functions that will be called by the library at the appropriate times, and will provide function pointers to these functions when adding sensors to the library. These user functions will contain the code that is specific to the user's application, with none of the actual sensor communication/reading being handled by the library. This library simply makes it easier to configure the ESP to wake up and perform tasks associated with the sensors at the appropriate times.

The library should work for both ESP32 and ESP8266, and you must ensure that any differences between the two are properly handled.

The library should be initialized with a call to the 'init' function, which should include a value indicating which mode of operation the library should use, whether Wifi is required during the period where the ESP is awake, and which low-power mode the ESP should use after waking up and completing its tasks. The two modes of operation are:
    1. Per-sensor Mode: Each sensor has a different interval at which it needs to be sampled, at which point the ESP must be woken up. In this mode, the user would provide function pointers for one or both of the following user-created functions: 
        - A function pointer for the function that should be called after the interval elapses and the library has woken up the ESP.
        - A function pointer for the function that should be called before the library puts the ESP to sleep (optional).
    2. Single-interval Mode: All sensors are sampled at a fixed interval. The ESP wakes up, samples all sensors, and then goes back to the configured low-power mode. In this mode, the user would provide function pointers for one or both of the following user-created functions: 
        - A function pointer for a function that should be called after the interval elapses and the library has woken up the ESP. The user can add numerous of these functions to the library, to be called in order, to perform different tasks after the interval elapses.
        - A function pointer for a function that should be called after all after-wake functions have been completed, and before the library puts the ESP to sleep (optional).

The user should call a function to add a sensor to a collection within the library. This will allow any necessary details to be associated with individual sensors. In per-sensor mode, each sensor will have its own interval after which the library should wake the ESP. The libary must take care of any calculations required to ensure that tasks corresponding to sensors with overlapping intervals are performed with one sensor's tasks all being completed before the second sensor's tasks begin. In single-interval mode, all sensors will have the same interval after which the library should wake the ESP.  

The library will call the appropriate functions at the appropriate times, and the user can add as many sensors as needed. The user can also add as many after-wake functions as needed. 

The library should include multiple examples and comprehensive documentation (README.md). 
The library should include a library.properties file with the appropriate details along with any other files seen in the standard Arduino library format.