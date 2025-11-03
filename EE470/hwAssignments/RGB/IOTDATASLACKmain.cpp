//-------------------------------
// Title: IFTTT Webhook Trigger via ESP8266
//-------------------------------
// Program Details:
//-------------------------------
// Purpose: This program monitors a physical button press and sends a PUT request
// to an IFTTT webhook URL to trigger an online event (e.g., Slack, SMS, Google Sheets).
// Input: Physical button press (D5)
// Output: PUT request to IFTTT webhook
// Date: 11/3/2025
// Compiler: PlatformIO
// Author: Jair Pacheco
// Versions:
// V1: Initial implementation using HTTPS and manual PUT request
// V2: Added debounce and serial debug output
//---------------------------------
// File Dependencies: ESP8266WiFi.h, WiFiClientSecure.h
//---------------------------------


//---------------------------------
// Main Program
//---------------------------------
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

// Wi-Fi credentials
const char* ssid = "iPhone";
const char* password = "Jpach6075";

// IFTTT Webhook details
const char* host = "maker.ifttt.com";
const int httpsPort = 443;
const char* requestPath = "/trigger/Button_pressed/with/key/lnsgEOIZYpEjYtlc2KFSe9ZxtNinxfgHt1LSm3pWPo2";


const int buttonPin = D5;

void setup() {
  // Initialize serial monitor and configure button pin
  Serial.begin(9600);
  pinMode(buttonPin, INPUT);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}

void loop() {
   // Monitor button press with edge detection
  static bool lastState = LOW;
  bool currentState = digitalRead(buttonPin);

  if (lastState == LOW && currentState == HIGH) {
    Serial.println("Button pressed! Sending PUT to IFTTT...");

    WiFiClientSecure client;
    client.setInsecure(); // Skip SSL cert validation

    if (client.connect(host, httpsPort)) {
      String url = String(requestPath) + "?value1=Button+Pressed";
        // Send raw HTTP PUT request
      client.print(String("PUT ") + url + " HTTP/1.1\r\n" +
                   "Host: " + host + "\r\n" +
                   "Connection: close\r\n\r\n");
      Serial.println("Request sent");
    } else {
      Serial.println("Connection failed");
    }

    delay(1000); // Debounce
  }

  lastState = currentState;
}
