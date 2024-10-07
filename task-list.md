# Task List
Use this document to keep track of tasks that need to be done.

## Unfinished tasks
1. Update library.properties with new keywords related to trigger modes
2. Add new test cases for different trigger modes in test files
3. Modify existing tests to accommodate the changes
4. Review and update any other relevant documentation or configuration files
5. Update keywords.txt with new keywords related to trigger modes
6. Review and update .github/workflows/ci.yml if necessary
7. Update include/README and lib/README if they contain relevant documentation
8. Review and update .vscode/c_cpp_properties.json if necessary
9. Review and update platformio.ini if necessary

## Finished tasks
63. Update README.md with information about new trigger modes and usage examples
62. Create new examples demonstrating digital and analog trigger usage
61. Update existing example files to showcase new trigger modes
60. Implement methods for checking digital and analog triggers in ESPLowPowerSensor.cpp
59. Update run, runPerSensorMode, and runSingleIntervalMode methods in ESPLowPowerSensor.cpp
58. Modify addSensor method in ESPLowPowerSensor.h and ESPLowPowerSensor.cpp
57. Update ESPLowPowerSensor.h to add TriggerMode enum and modify Sensor struct
56. Implement WiFi credential storage and management
2. Create the basic library structure (header and source files)
3. Implement the 'init' function with mode selection and configuration options
4. Implement the sensor addition function
5. Develop the Per-sensor Mode functionality
6. Develop the Single-interval Mode functionality
7. Implement ESP32/ESP8266 compatibility checks and adjustments
8. Create multiple examples demonstrating library usage
9. Write comprehensive documentation (README.md)
10. Create library.properties file
11. Perform final code review and testing
12. Add thorough comments throughout both the .cpp and .h files, ensuring comprehensive understanding for AI code readers
13. Add const qualifiers to methods that don't modify the object state in ESPLowPowerSensor.h
14. Add input validation for the interval parameter in addSensor()
15. Fix const correctness issues in runPerSensorMode() and runSingleIntervalMode()
16. In runPerSensorMode(), use std::min_element to find the next wake time instead of iterating through the sensors twice
17. In goToSleep(), add a check to ensure sleepTime is not zero or negative
18. Add error handling for WiFi operations in wifiOff() and wifiOn()
19. Set up unit testing framework (e.g., ArduinoUnit or AUnit)
20. Write test cases for ESPLowPowerSensor class initialization
21. Implement tests for addSensor() method with various inputs
22. Create tests for run() method in both Per-Sensor and Single-Interval modes
23. Develop mock functions for WiFi and sleep operations to test goToSleep(), wifiOn(), and wifiOff() methods
24. Write tests to verify correct behavior of runPerSensorMode() and runSingleIntervalMode()
25. Implement edge case tests (e.g., adding many sensors, very short/long intervals)
26. Set up Continuous Integration to run tests automatically on code changes
27. Research and implement a method to set up timer interrupts on both ESP32 and ESP8266
28. Create a new interrupt service routine (ISR) to handle sensor wake and sleep functions
29. Modify the addSensor() method to associate wake and sleep functions with specific interrupts
30. Update runPerSensorMode() and runSingleIntervalMode() to trigger interrupts instead of directly calling functions
31. Implement a queue system to handle multiple sensor interrupts that may occur close together
32. Add safeguards to ensure interrupt handlers complete before entering sleep mode
33. Update the documentation to reflect the new interrupt-driven approach
34. Create example sketches demonstrating the use of interrupt-driven sensor functions
35. Implement error handling for cases where interrupt setup fails
36. Add a method to disable interrupts if the user wants to revert to the original behavior
37. Use static allocation instead of dynamic allocation where possible to reduce memory fragmentation and improve performance.
38. Implement power-efficient delay using ESP32's and ESP8266's low-power timer peripherals instead of using delay() function.
39. Optimize the interrupt handling routine to minimize the time spent in the ISR.
40. Use inline functions for small, frequently called functions to reduce function call overhead.
41. Implement a more efficient queue system for interrupt handling, possibly using a circular buffer.
42. Use constexpr for compile-time computations where applicable.
43. Implement lazy initialization for WiFi to reduce startup time and power consumption.
44. Define MAX_SENSORS constant
45. Implement empty() method for CircularBuffer class
46. Fix array subscript errors for _buffer in CircularBuffer class
47. Add proper ESP32 WiFi header includes
48. Replace esp_wifi_start and esp_wifi_stop with appropriate WiFi functions
49. Fix CIRCULAR_BUFFER_SIZE and MAX_SENSORS declaration order
50. Fix array subscript errors for _sensors in ESPLowPowerSensor class
51. Replace esp_wifi_start with appropriate WiFi function in init method
52. Remove duplicate definition of CIRCULAR_BUFFER_SIZE
53. Update README.md to mention the interrupt-driven approach
54. Create a new example demonstrating the interrupt-driven approach
55. Add "interrupt" keyword to library.properties