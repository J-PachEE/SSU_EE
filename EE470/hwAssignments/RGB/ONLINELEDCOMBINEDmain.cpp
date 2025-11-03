#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

const char* ssid = "iPhone";
const char* password = "Jpach6075";

// URLs
const char* ledUrl = "https://jairpacheco.com/results.txt";
const char* rgbUrl = "https://jairpacheco.com/rgb.txt";
const char* webhookUrl = "https://maker.ifttt.com/trigger/Button_pressed/with/key/lnsgEOIZYpEjYtlc2KFSe9ZxtNinxfgHt1LSm3pWPo2";

// Pins
const int ledPin = D2;
const int redPin = D7;
const int greenPin = D6;
const int bluePin = D5;
const int buttonStatus = D1;
const int buttonWebhook = D0;


void syncLEDandRGB() {
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;

  // LED status
  http.begin(client, ledUrl);
  if (http.GET() == 200) {
    String payload = http.getString();
StaticJsonDocument<200> doc;
DeserializationError error = deserializeJson(doc, payload);

if (!error) {
  String ledStatus = doc["led"];
  digitalWrite(ledPin, ledStatus == "ON" ? HIGH : LOW);
  Serial.println("LED status: " + ledStatus);
} else {
  Serial.println("Failed to parse LED JSON");
}
  }
  http.end();

  // RGB value
  http.begin(client, rgbUrl);
  if (http.GET() == 200) {
    int red = http.getString().toInt();
    analogWrite(redPin, red);
    analogWrite(greenPin, 0);
    analogWrite(bluePin, 0);
    Serial.println("RGB Red: " + String(red));
  }
  http.end();
}


void sendWebhookMessage() {
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;

  http.begin(client, webhookUrl);
  http.addHeader("Content-Type", "application/json");
  String payload = "{\"value1\":\"ESP Triggered\"}";
  int code = http.POST(payload);
  Serial.println("Webhook sent, code: " + String(code));
  http.end();
}

void syncLEDandRGB();
void sendWebhookMessage();


void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(buttonStatus, INPUT);
  pinMode(buttonWebhook, INPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  Serial.println("WiFi connected");
}

void loop() {
  static bool lastStatus = HIGH;
  static bool lastWebhook = HIGH;
  bool currentStatus = digitalRead(buttonStatus);
  bool currentWebhook = digitalRead(buttonWebhook);

  // LED/RGB sync
  if (lastStatus == HIGH && currentStatus == LOW) {
    syncLEDandRGB();
  }

  // Webhook trigger
  if (lastWebhook == HIGH && currentWebhook == LOW) {
    sendWebhookMessage();
  }

  lastStatus = currentStatus;
  lastWebhook = currentWebhook;
  delay(50); // debounce
}
