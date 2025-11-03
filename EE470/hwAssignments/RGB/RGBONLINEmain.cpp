//-------------------------------
// Title: ESP8266 RGB LED online sync
//-------------------------------
// Program Details:
//-------------------------------
// Purpose: This program reads a remote file containing an RGB intensity value (red channel only)
// and updates the RGB LED accordingly. It triggers either every 30 seconds or when a button is pressed.
// Input: Button press (D1)
// Output: RGB LED intensity (red channel)
// Date: 11/3/2025
// Compiler: PlatformIO
// Author: Jair Pacheco
// Versions:
// V1: Initial implementation with timed and manual sync
// V2: Added HTTPS support and serial debug output
// V3: RGB control simplified to red channel only
//---------------------------------
// File Dependencies: Arduino.h, ESP8266WiFi.h, ESP8266HTTPClient.h, WiFiClientSecure.h
//---------------------------------


//---------------------------------
// Main Program
//---------------------------------
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>

const char* ssid = "iPhone";
const char* password = "Jpach6075";

const char* ledUrl = "https://jairpacheco.com/results.txt";
const char* rgbUrl = "https://jairpacheco.com/rgb.txt";

const int ledPin = D2;
const int redPin = D7;
const int greenPin = D6;
const int bluePin = D5;
const int buttonPin = D1;

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(buttonPin, INPUT);
   // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  Serial.println("WiFi connected");
}

void loop() {
   // Track time and button state for sync triggers
  static unsigned long lastCheck = 0;
  static bool lastButton = LOW;
  bool currentButton = digitalRead(buttonPin);

  // Trigger sync every 30 seconds or on button press
  if ((millis() - lastCheck > 30000) || (lastButton == LOW && currentButton == HIGH)) {
    lastCheck = millis();
    Serial.println("Checking LED and RGB status...");

    if (WiFi.status() == WL_CONNECTED) {
      WiFiClientSecure client;
      client.setInsecure();
      HTTPClient http;

      // LED status
      http.begin(client, ledUrl);
      int code1 = http.GET();
      if (code1 == 200) {
        String ledStatus = http.getString();
        ledStatus.trim();
        digitalWrite(ledPin, ledStatus == "ON" ? HIGH : LOW);
        Serial.println("LED: " + ledStatus);
      }
      http.end();

      // RGB value (red channel only)
      http.begin(client, rgbUrl);
      int code2 = http.GET();
      if (code2 == 200) {
        String rgbValue = http.getString();
        int red = rgbValue.toInt();
        analogWrite(redPin, red);
        analogWrite(greenPin, 0);
        analogWrite(bluePin, 0);
        Serial.println("RGB Red: " + String(red));
      }
      http.end();
    }
    delay(1000); // debounce
  }

  lastButton = currentButton;
}
