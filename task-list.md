# Task List
Use this document to keep track of tasks that need to be done.

## Unfinished tasks
No unfinished tasks remaining.

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
29. Update runPerSensorMode() and runSingleIntervalMode() to trigger interrupts instead of directly calling functions
30. Implement a queue system to handle multiple sensor interrupts that may occur close together
31. Add safeguards to ensure interrupt handlers complete before entering sleep mode
32. Update the documentation to reflect the new interrupt-driven approach
33. Create example sketches demonstrating the use of interrupt-driven sensor functions
34. Implement error handling for cases where interrupt setup fails
35. Add a method to disable interrupts if the user wants to revert to the original behavior
36. Use static allocation instead of dynamic allocation where possible to reduce memory fragmentation and improve performance.
37. Implement power-efficient delay using ESP32's and ESP8266's low-power timer peripherals instead of using delay() function.
38. Optimize the interrupt handling routine to minimize the time spent in the ISR.
39. Use inline functions for small, frequently called functions to reduce function call overhead.
40. Implement a more efficient queue system for interrupt handling, possibly using a circular buffer.
41. Use constexpr for compile-time computations where applicable.
42. Implement lazy initialization for WiFi to reduce startup time and power consumption.
43. Define MAX_SENSORS constant
44. Implement empty() method for CircularBuffer class
45. Fix array subscript errors for _buffer in CircularBuffer class
46. Add proper ESP32 WiFi header includes
47. Replace esp_wifi_start and esp_wifi_stop with appropriate WiFi functions
48. Fix CIRCULAR_BUFFER_SIZE and MAX_SENSORS declaration order
49. Fix array subscript errors for _sensors in ESPLowPowerSensor class
50. Replace esp_wifi_start with appropriate WiFi function in init method
52. Remove duplicate definition of CIRCULAR_BUFFER_SIZE