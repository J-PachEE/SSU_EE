
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

// Wi-Fi credentials
const char* ssid = "iPhone";
const char* password = "Jpach6075";

// IFTTT Webhook details
const char* host = "maker.ifttt.com";
const int httpsPort = 443;
const char* requestPath = "/trigger/Button_pressed/with/key/lnsgEOIZYpEjYtlc2KFSe9ZxtNinxfgHt1LSm3pWPo2";

// Button pin
const int buttonPin = D5;

void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT); // Pull-down resistor already wired
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}

void loop() {
  static bool lastState = LOW;
  bool currentState = digitalRead(buttonPin);

  if (lastState == LOW && currentState == HIGH) {
    Serial.println("Button pressed! Sending PUT to IFTTT...");

    WiFiClientSecure client;
    client.setInsecure(); // Skip SSL cert validation

    if (client.connect(host, httpsPort)) {
      String url = String(requestPath) + "?value1=Button+Pressed";
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
