# Task List
Use this document to keep track of tasks that need to be done. Determine 

## Unfinished tasks
1. Modify the library to use interrupts for executing user-created functions:
    a. Update runPerSensorMode() and runSingleIntervalMode() to trigger interrupts instead of directly calling functions
    b. Implement a queue system to handle multiple sensor interrupts that may occur close together
    c. Add safeguards to ensure interrupt handlers complete before entering sleep mode
    d. Update the documentation to reflect the new interrupt-driven approach
    e. Create example sketches demonstrating the use of interrupt-driven sensor functions
    f. Implement error handling for cases where interrupt setup fails
    g. Add a method to disable interrupts if the user wants to revert to the original behavior

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
15. In runPerSensorMode(), use std::min_element to find the next wake time instead of iterating through the sensors twice
16. In goToSleep(), add a check to ensure sleepTime is not zero or negative
17. Add error handling for WiFi operations in wifiOff() and wifiOn()
18. Set up unit testing framework (e.g., ArduinoUnit or AUnit)
19. Write test cases for ESPLowPowerSensor class initialization
20. Implement tests for addSensor() method with various inputs
21. Create tests for run() method in both Per-Sensor and Single-Interval modes
22. Develop mock functions for WiFi and sleep operations to test goToSleep(), wifiOn(), and wifiOff() methods
23. Write tests to verify correct behavior of runPerSensorMode() and runSingleIntervalMode()
24. Implement edge case tests (e.g., adding many sensors, very short/long intervals)
25. Set up Continuous Integration to run tests automatically on code changes
26. Research and implement a method to set up timer interrupts on both ESP32 and ESP8266
27. Create a new interrupt service routine (ISR) to handle sensor wake and sleep functions
28. Modify the addSensor() method to associate wake and sleep functions with specific interrupts