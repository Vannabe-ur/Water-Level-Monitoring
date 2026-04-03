#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/* ================= OLED CONFIG ================= */
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDR 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

/* ================= SENSOR CONFIG ================= */
const int sensorPin = A0;

// Calibration values
const float Rdry = 20.0;
const float Rwet = 640.0;

// Tank and sensor parameters
const float tankHeightCM = 100.0;
const float sensorLength = 4.0; // Sensing area of sensor (cm)
const float H_offset = 94.0;   // tankHeight - TotalSensorLength = 100 - 6

/* WIFI CONFIG */
const char* ssid = "Shin Zo.";
const char* password = "08072022";
const char* serverURL = "http://10.194.208.217/water/update.php";

/* SETUP */
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

  /* WIFI */
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

/* LOOP */
void loop() {

  int rawValue = analogRead(sensorPin);

  /* STEP 1: NORMALIZATION */
  float N = (rawValue - Rdry) / (Rwet - Rdry);

  if (N < 0) N = 0;
  if (N > 1) N = 1;

  /* STEP 2: SENSOR REGION */
  float Lsensor = N * sensorLength;

  /* STEP 3: ACTUAL WATER HEIGHT */
  float waterCM = H_offset + Lsensor;

  /* STATUS LOGIC */
  String state;

  if (rawValue <= Rdry + 20) {
    state = "SAFE";        // no water touching sensor
  }
  else if (rawValue > Rdry + 20 && rawValue < Rwet - 20) {
    state = "CAUTION";     // water touching sensor
  }
  else if (rawValue >= Rwet - 20) {
    state = "FULL";        // near full
  }

  /* OLED DISPLAY */
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

  /* SERIAL LOG */
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

  /* SEND TO SERVER */
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
