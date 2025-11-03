//-------------------------------
//Title: LED Status Sync via Web
//-------------------------------
//Program details: 
//-------------------------------
//Purpose: The program checks a remote server for LED status stored in JSON format
// and updates the onboard LED accordingly when a switch is pressed.
//Input: Physical switch press
//Outputs: LED ON/OFF based on server response
//Date: 11/3/2025
//Compiler: Platform IO
//Author: Jair Pacheco
//Versions:
// V1: First Attempt
// V2: Added JSON parsing for remote LED status
// V3: Added switch debounce and HTTPS support
//---------------------------------
//File Dependencies: Arduino.h, ESP8266WiFi.h, ESP8266HTTPClient.h, ArduinoJson.h
//---------------------------------


//---------------------------------
//Main Program
//---------------------------------

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

// Wi-Fi credentials and server URL
const char* ssid = "iPhone";
const char* password = "Jpach6075";
const char* url = "https://jairpacheco.com/results.txt"; 

const int ledPin = D2;
const int switchPin = D1;

void setup() {

  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  pinMode(switchPin, INPUT); 

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  Serial.println("WiFi connected");
}

void loop() {
   // Monitor switch press and debounce
  static bool lastState = LOW;
  bool currentState = digitalRead(switchPin);

  if (lastState == LOW && currentState == HIGH) {
    Serial.println("Switch pressed! Checking LED status...");

    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      WiFiClientSecure client;
      client.setInsecure(); // Skip SSL cert validation
      http.begin(client, url);

      int httpCode = http.GET();
      Serial.println("HTTP code: " + String(httpCode));

      // If response is OK, parse JSON and update LED
      if (httpCode == 200) {
        String payload = http.getString();
        Serial.println("Payload: " + payload);

        StaticJsonDocument<200> doc;
        DeserializationError error = deserializeJson(doc, payload);
        if (!error) {
          const char* status = doc["led"];
          digitalWrite(ledPin, strcmp(status, "ON") == 0 ? HIGH : LOW);
        } else {
          Serial.println("JSON parse error");
        }
      }

      http.end();
    }

    delay(1000); // Debounce delay
  }

  lastState = currentState;
}
