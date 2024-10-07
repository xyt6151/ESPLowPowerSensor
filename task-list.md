# Task List
Use this document to keep track of tasks that need to be done. Determine 

## Unfinished tasks
1. Add a method to change the operating mode after initialization: bool setMode(Mode newMode)
2. In runPerSensorMode(), use std::min_element to find the next wake time instead of iterating through the sensors twice
3. In goToSleep(), add a check to ensure sleepTime is not zero or negative
4. Add error handling for WiFi operations in wifiOff() and wifiOn()
5. Set up unit testing framework (e.g., ArduinoUnit or AUnit)
6. Write test cases for ESPLowPowerSensor class initialization
7. Implement tests for addSensor() method with various inputs
8. Create tests for run() method in both Per-Sensor and Single-Interval modes
9. Develop mock functions for WiFi and sleep operations to test goToSleep(), wifiOn(), and wifiOff() methods
10. Write tests to verify correct behavior of runPerSensorMode() and runSingleIntervalMode()
11. Implement edge case tests (e.g., adding many sensors, very short/long intervals)
12. Set up Continuous Integration to run tests automatically on code changes

## Finished tasks
1. Create the basic library structure (header and source files)
2. Implement the 'init' function with mode selection and configuration options
3. Implement the sensor addition function
4. Develop the Per-sensor Mode functionality
5. Develop the Single-interval Mode functionality
6. Implement ESP32/ESP8266 compatibility checks and adjustments
7. Create multiple examples demonstrating library usage
8. Write comprehensive documentation (README.md)
9. Create library.properties file
10. Perform final code review and testing
11. Add thorough comments throughout both the .cpp and .h files, ensuring comprehensive understanding for AI code readers
12. Add const qualifiers to methods that don't modify the object state in ESPLowPowerSensor.h
13. Add input validation for the interval parameter in addSensor()
14. Fix const correctness issues in runPerSensorMode() and runSingleIntervalMode()