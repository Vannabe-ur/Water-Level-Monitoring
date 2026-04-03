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
const int sensorPin = A0;      //  ESP8266 ONLY HAS A0

// 🔧 CALIBRATION (YOU MUST SET THESE FROM SERIAL MONITOR)
const int dryValue = 120;      // sensor value when DRY
const int wetValue = 760;      // sensor value when FULLY WET
const float tankHeightCM = 20.0; // max tank height (cm)

/* ================= LEVEL THRESHOLDS ================= */
const float SAFE_LEVEL = 6.0;
const float MID_LEVEL  = 12.0;

/* ================= WIFI CONFIG ================= */
const char* ssid = "Bun Hong";
const char* password = "67670749";
const char* serverURL = "http://172.17.255.25/water/update.php";

/* ================= SETUP ================= */
void setup() {
  Serial.begin(9600);

  // I2C (ESP8266 default: SDA=D2, SCL=D1)
  Wire.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println(" OLED NOT FOUND");
    while (1);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Starting...");
  display.display();

  // WiFi connect
  WiFi.begin(ssid, password);
  Serial.print("Connecting WiFi");

  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 15000) {
    delay(500);
    Serial.print(".");
  }

  display.clearDisplay();
  display.setCursor(0, 0);

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n WiFi connected");
    display.println("WiFi OK");
    display.println(WiFi.localIP());
  } else {
    Serial.println("\n WiFi FAILED");
    display.println("WiFi FAIL");
  }

  display.display();
}

/* ================= LOOP ================= */
void loop() {
  int rawValue = analogRead(sensorPin);

  // Convert raw → cm (1 decimal)
  float waterCM = map(rawValue, dryValue, wetValue,
                      0, tankHeightCM * 10) / 10.0;

  // Safety clamp
  waterCM = constrain(waterCM, 0, tankHeightCM);

  // Determine state
  String state;
  if (waterCM < SAFE_LEVEL) {
    state = "SAFE";
  } else if (waterCM < MID_LEVEL) {
    state = "MID";
  } else {
    state = "HIGH";
  }

  /* ---------- OLED DISPLAY ---------- */
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

  /* ---------- SERIAL DEBUG ---------- */
  Serial.print("Raw: ");
  Serial.print(rawValue);
  Serial.print(" | Water: ");
  Serial.print(waterCM, 1);
  Serial.print(" cm | ");
  Serial.println(state);

  /* ---------- SEND TO SERVER ---------- */
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    String url = String(serverURL) + "?cm=" + String(waterCM, 1);
    http.begin(client, url);
    http.GET();
    http.end();
  }

  delay(1000); // update every 1 second
}
