


#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "iPhone";
const char* password = "Jpach6075";
const char* url = "https://jairpacheco.com/results.txt"; 

const int ledPin = D2;
const int switchPin = D1;

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  pinMode(switchPin, INPUT); 
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  Serial.println("WiFi connected");
}

void loop() {
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

    delay(1000); // Debounce
  }

  lastState = currentState;
}
