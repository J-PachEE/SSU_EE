/*
#include <Arduino.h>

// Part A: ESP8266 Hardware Test (Potentiometer, Switch, LED) /////////////////////////

#define LED_PIN    D5      // GPIO pin for LED
#define SWITCH_PIN D2      // GPIO pin for switch
#define POT_PIN    A0      // Analog input for potentiometer

int lastSwitchState = LOW; // track previous state

void setup() {
  Serial.begin(9600); // faster baud rate for ESP8266

  pinMode(LED_PIN, OUTPUT);
  pinMode(SWITCH_PIN, INPUT); // use INPUT_PULLUP if wired to GND

  Serial.println("ESP8266 Part A Test Started");
  Serial.println("Type '1' to turn LED ON, '0' to turn LED OFF");
}

void loop() {
  // --- Potentiometer ---
  int potValue = analogRead(POT_PIN);   // range 0–1023
  float voltage = (potValue / 1023.0) * 3.3; // convert to volts
  Serial.print("Potentiometer raw: ");
  Serial.print(potValue);
  Serial.print(" | Voltage: ");
  Serial.println(voltage, 2);


 // --- Switch ---
  int switchState = digitalRead(SWITCH_PIN);
  if (switchState == HIGH) { // pressed (with pull-down resistor)
    digitalWrite(LED_PIN, HIGH);
    Serial.println("Button pressed → LED ON");
  } else {
    digitalWrite(LED_PIN, LOW);
    Serial.println("Button released → LED OFF");
  } 

  // --- Serial LED Control ---
  if (Serial.available()) {
    char cmd = Serial.read();
    if (cmd == '1') {
      digitalWrite(LED_PIN, HIGH);
      Serial.println("LED ON (Serial)");
    } else if (cmd == '0') {
      digitalWrite(LED_PIN, LOW);
      Serial.println("LED OFF (Serial)");
    }
  }

  delay(1000); // shorter loop delay for responsiveness
}
*/

// PART B /////////////////////////////////////////////////////////////
/*
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// ----- WiFi credentials -----
const char* ssid     = "iPhone";
const char* password = "Jpach6075";

// ----- MQTT broker (HiveMQ public) -----
const char* mqtt_server = "broker.mqttdashboard.com"; // HiveMQ public broker
const uint16_t mqtt_port = 1883; // use 8883 only if you add TLS

// ----- Topic -----
const char* pub_topic = "testtopic/temp/room1";

// ----- Pins -----
#define POT_PIN A0

// ----- Globals -----
WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastPublish = 0;
const unsigned long publishIntervalMs = 15000; // 15 seconds

// ----- Helpers -----
void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nWiFi connected. IP: ");
  Serial.println(WiFi.localIP());
}

void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Connecting MQTT...");
    // Unique client ID
    String clientId = "ESP8266Client-";
    clientId += String(ESP.getChipId(), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // No subscriptions needed in Part B
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" retry in 2s");
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(100);

  connectWiFi();

  client.setServer(mqtt_server, mqtt_port);
  reconnectMQTT();

  Serial.println("Step B: Publishing potentiometer every 15s");
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }
  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastPublish >= publishIntervalMs) {
    lastPublish = now;

    int raw = analogRead(POT_PIN);               // 0–1023
    float volts = (raw / 1023.0f) * 3.3f;        // approx. 3.3V reference
    // Publish as JSON for easy parsing later
    char payload[64];
    snprintf(payload, sizeof(payload), "{\"raw\":%d,\"volts\":%.2f}", raw, volts);

    bool ok = client.publish(pub_topic, payload, false); // retain=false
    Serial.print("Publish to ");
    Serial.print(pub_topic);
    Serial.print(" -> ");
    Serial.print(payload);
    Serial.println(ok ? " [OK]" : " [FAIL]");
  }
}
*/ 

/*
//PART C /////////////////////////////////////////////////////////////////////
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// ----- WiFi -----
const char* ssid     = "iPhone";
const char* password = "Jpach6075";

// ----- MQTT -----
const char* mqtt_server = "broker.mqttdashboard.com";
const int mqtt_port = 1883;

const char* sub_topic = "testtopic/temp/inTopic";       // LED control
const char* pub_topic = "testtopic/temp/outTopic/room1"; // switch events

// ----- Pins -----
#define LED_PIN D5
#define SWITCH_PIN D2   // pull-down button

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastSwitchPress = 0;
bool switchPressed = false;

// ----- MQTT callback -----
void callback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (unsigned int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(msg);

  if (String(topic) == sub_topic) {
    if (msg == "1") {
      digitalWrite(LED_PIN, HIGH);
      Serial.println("LED ON (via MQTT)");
    } else if (msg == "0") {
      digitalWrite(LED_PIN, LOW);
      Serial.println("LED OFF (via MQTT)");
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-" + String(ESP.getChipId(), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe(sub_topic); // subscribe to LED control
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" retry in 2s");
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(SWITCH_PIN, INPUT); // external pull-down resistor

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  // --- Switch logic (pull-down: HIGH = pressed) ---
  int state = digitalRead(SWITCH_PIN);
  if (state == HIGH && !switchPressed) {
    // Button pressed
    switchPressed = true;
    lastSwitchPress = millis();
    client.publish(pub_topic, "1");
    Serial.println("Switch pressed → published 1");
  }

  // After 5 seconds, publish 0
  if (switchPressed && millis() - lastSwitchPress >= 5000) {
    client.publish(pub_topic, "0");
    Serial.println("Switch released → published 0");
    switchPressed = false;
  }
}
  */ 

// PART F1 /////////////////////////////////////////////////////////////////////////////////  
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// ----- WiFi -----
const char* ssid     = "iPhone";
const char* password = "Jpach6075";

// ----- MQTT -----
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;

const char* sub_topic = "testtopic/temp/inTopic";        // LED control
const char* pub_topic = "testtopic/temp/outTopic/room1"; // potentiometer data

// ----- Pins -----
#define LED_PIN D5
#define POT_PIN A0

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastPublish = 0;
const unsigned long publishIntervalMs = 15000; // 15 seconds

// ----- MQTT callback -----
void callback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (unsigned int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }
  msg.trim(); // remove whitespace/newlines

  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(msg);

  if (String(topic) == sub_topic) {
    if (msg == "1") {
      digitalWrite(LED_PIN, HIGH);
      Serial.println("LED ON (via MQTT)");
    } else if (msg == "0") {
      digitalWrite(LED_PIN, LOW);
      Serial.println("LED OFF (via MQTT)");
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-" + String(ESP.getChipId(), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe(sub_topic); // subscribe to LED control
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" retry in 2s");
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  // --- Publish potentiometer data every 15s ---
  unsigned long now = millis();
  if (now - lastPublish >= publishIntervalMs) {
    lastPublish = now;

    int raw = analogRead(POT_PIN);
    float volts = (raw / 1023.0f) * 3.3f;

    char payload[64];
    snprintf(payload, sizeof(payload), "{\"raw\":%d,\"volts\":%.2f}", raw, volts);

    client.publish(pub_topic, payload);
    Serial.print("Published to ");
    Serial.print(pub_topic);
    Serial.print(" -> ");
    Serial.println(payload);
  }
}
