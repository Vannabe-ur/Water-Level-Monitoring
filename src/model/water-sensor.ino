#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/** 
 * OLED Configurations
 * - SCREEN_WIDTH: OLED display width, in pixels
 * - SCREEN_HEIGHT: OLED display height, in pixels
 * - OLED_RESET: Reset pin (or -1 if sharing Arduino reset pin)
 * - OLED_ADDR: I2C address of the OLED display
*/

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDR 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

/**
 * Sensor and Tank Configurations
 * - sensorPin: Analog pin connected to the water sensor
 * - Rdry: Resistance value when the sensor is dry
 * - Rwet: Resistance value when the sensor is wet
 * - tankHeightCM: Total height of the water tank in centimeters
 * - sensorLength: Length of the sensor's sensing area in centimeters
 */

const int sensorPin = A0;

// Calibration values
const float Rdry = 20.0;
const float Rwet = 640.0;

// Tank and sensor parameters
const float tankHeightCM = 100.0;
const float sensorLength = 4.0; // Sensing area of sensor (cm)
const float H_offset = 94.0;   // tankHeight - TotalSensorLength = 100 - 6

/**
 * WiFi and Server Configurations
 * - ssid: WiFi network name
 * - password: WiFi network password
 * - serverURL: URL of the server endpoint to send water level data
 */

const char* ssid = "F206";
const char* password = "11112222";
const char* serverURL = "http://192.168.20.1/water/update.php";

/**
 * Setup function ->
 */

void setup() {

  Serial.begin(9600);
  Wire.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("OLED not found");
    while (1);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Starting...");
  display.display();

  // Start WiFi connection
  WiFi.begin(ssid, password);
  Serial.print("Connecting WiFi");

  unsigned long startTime = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 15000) {
    delay(500);
    Serial.print(".");
  }

  display.clearDisplay();

  if (WiFi.status() == WL_CONNECTED) {
    display.println("WiFi OK");
    display.println(WiFi.localIP());
  } else {
    display.println("WiFi FAIL");
  }

  display.display();
}

/**
 * Main loop function ->
 * 1. Reads raw sensor value
 * 2. Normalizes the value to a 0-1 range
 * 3. Calculates the sensor region and actual water height
 * 4. Determines the water level status (SAFE, CAUTION, FULL)
 * 5. Updates the OLED display with water level and status
 * 6. Logs the data to the serial monitor
 * 7. Sends the data to the server if WiFi is connected
 * 8. Delays for 1 second before the next reading
 */

void loop() {

  int rawValue = analogRead(sensorPin);

  // Normalize
  float N = (rawValue - Rdry) / (Rwet - Rdry);

  if (N < 0) N = 0;
  if (N > 1) N = 1;

  // Init Sensor Region
  float Lsensor = N * sensorLength;

  // Get actual water height in cm
  float waterCM = H_offset + Lsensor;

  // Status logics
  String state;

  if (rawValue <= Rdry + 20) {
    state = "SAFE";        // No water touching sensor
  }
  else if (rawValue > Rdry + 20 && rawValue < Rwet - 20) {
    state = "CAUTION";     // Water touching sensor
  }
  else if (rawValue >= Rwet - 20) {
    state = "FULL";        // Near full
  }

  // Start OLED display update ->
  display.clearDisplay();

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Water Level");

  display.setTextSize(2);
  display.setCursor(0, 14);
  display.print(waterCM, 1);
  display.println(" cm");

  display.setTextSize(1);
  display.setCursor(0, 42);
  display.print("Status: ");
  display.println(state);

  display.display();

  // Logs ->
  Serial.print("Raw: ");
  Serial.print(rawValue);
  Serial.print(" | N: ");
  Serial.print(N);
  Serial.print(" | Lsensor: ");
  Serial.print(Lsensor);
  Serial.print(" | Water: ");
  Serial.print(waterCM);
  Serial.print(" cm | ");
  Serial.println(state);

  // Send data to server ->
  if (WiFi.status() == WL_CONNECTED) {

    WiFiClient client;
    HTTPClient http;

    String url = String(serverURL) +
                 "?cm=" + String(waterCM, 1) +
                 "&status=" + state;

    http.begin(client, url);
    http.GET();
    http.end();
  }

  delay(1000);
}