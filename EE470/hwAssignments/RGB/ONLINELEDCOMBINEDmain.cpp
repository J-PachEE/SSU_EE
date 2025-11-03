//-------------------------------
// Title: Unified ESP8266 Control for LED, RGB, and Webhook
//-------------------------------
// Program Details:
//-------------------------------
// Purpose: This program integrates three functions into a single ESP8266 sketch:
// 1. Reads LED status from a remote JSON file and updates a physical LED
// 2. Reads RGB intensity from a remote file and updates an RGB LED
// 3. Sends a webhook message to IFTTT when a button is pressed
// Inputs: Two physical buttons (D1 for LED/RGB, D0 for webhook trigger)
// Outputs: LED ON/OFF, RGB LED, IFTTT message
// Date: 11/3/2025
// Compiler: PlatformIO
// Author: Jair Pacheco
// Versions:
// V1: Initial integration of LED, RGB, and webhook functions
//---------------------------------
// File Dependencies: Arduino.h, ESP8266WiFi.h, ESP8266HTTPClient.h, WiFiClientSecure.h, ArduinoJson.h
//---------------------------------

//---------------------------------
// Main Program
//---------------------------------
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

const char* ssid = "iPhone";
const char* password = "Jpach6075";

// Remote URLs for LED, RGB, and webhook
const char* ledUrl = "https://jairpacheco.com/results.txt";
const char* rgbUrl = "https://jairpacheco.com/rgb.txt";
const char* webhookUrl = "https://maker.ifttt.com/trigger/Button_pressed/with/key/lnsgEOIZYpEjYtlc2KFSe9ZxtNinxfgHt1LSm3pWPo2";


const int ledPin = D2;
const int redPin = D7;
const int greenPin = D6;
const int bluePin = D5;
const int buttonStatus = D1;
const int buttonWebhook = D0;

// Reads LED status and RGB value from server and updates hardware
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

    // Fetch RGB value from server
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

// Sends a webhook message to IFTTT when button is pressed
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

// Forward declarations
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
 // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  Serial.println("WiFi connected");
}

void loop() {
  static bool lastStatus = HIGH;
  static bool lastWebhook = HIGH;
  bool currentStatus = digitalRead(buttonStatus);
  bool currentWebhook = digitalRead(buttonWebhook);

  // LED/RGB 
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
