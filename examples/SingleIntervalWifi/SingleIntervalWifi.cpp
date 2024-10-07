#include <ESPLowPowerSensor.h>
#include <WiFi.h>

ESPLowPowerSensor lowPowerSensor;

const char* ssid = "YourWiFiSSID";
const char* password = "YourWiFiPassword";

void setup() {
  Serial.begin(115200);
  
  // Initialize the library in Single-Interval mode, with WiFi, using light sleep
  lowPowerSensor.init(ESPLowPowerSensor::Mode::SINGLE_INTERVAL, true, ESPLowPowerSensor::LowPowerMode::LIGHT_SLEEP);
  
  // Add sensors (all will use the same interval)
  lowPowerSensor.addSensor(readAndSendSensor1);
  lowPowerSensor.addSensor(readAndSendSensor2);
  
  // Set up WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  lowPowerSensor.run();
}

void readAndSendSensor1() {
  Serial.println("Reading and sending Sensor 1 data");
  // Add your sensor 1 reading and data sending code here
}

void readAndSendSensor2() {
  Serial.println("Reading and sending Sensor 2 data");
  // Add your sensor 2 reading and data sending code here
}
