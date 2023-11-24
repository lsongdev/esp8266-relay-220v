#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <PubSubClient.h>
#include <EEPROM.h>

const char* apName = "ESP8266 Relay";
const char* clientID = "esp8266-relay-220v";
const char *relayTopic = "esp8266-relay-220v";

const int RY1 = 4;
const int LED = LED_BUILTIN;

WiFiClient wlan;
PubSubClient mqtt(wlan);
WiFiManager wifiManager(apName);

// Add custom parameters for configuring MQTT server information
WiFiManagerParameter custom_mqtt_server("mqtt_server", "MQTT Server", "192.168.8.160", 40);
WiFiManagerParameter custom_mqtt_port("mqtt_port", "MQTT Port", "1883", 6);
WiFiManagerParameter custom_mqtt_user("mqtt_user", "MQTT User", "mqtt", 40);
WiFiManagerParameter custom_mqtt_password("mqtt_password", "MQTT Password", "mqtt123", 40);

void reconnect()
{
  String user = custom_mqtt_user.getValue();
  String pass = custom_mqtt_password.getValue();
  while (!mqtt.connected())
  {
    Serial.println("Attempting to connect to the MQTT server...");
    if (mqtt.connect(clientID, user.c_str(), pass.c_str()))
    {
      Serial.println("MQTT connected");
      mqtt.subscribe(relayTopic);
    }
    else
    {
      Serial.println("MQTT connect failed, retrying...");
      delay(2000);
    }
  }
}

void onMessage(char* topic, byte* payload, unsigned int length) {
  // Handle received messages
  String payloadStr = "";
  for (unsigned int i = 0; i < length; i++) {
    payloadStr += (char)payload[i];
  }
  Serial.println("Received message: [" + String(topic) + "] " + payloadStr);

  if (strcmp(topic, relayTopic) == 0) { // Compare with the control topic
    if (payloadStr == "relay1on") {
      digitalWrite(RY1, HIGH); // Open the first relay
    } else if (payloadStr == "relay1off") {
      digitalWrite(RY1, LOW); // Close the first relay
    } else if (payloadStr == "led1on") {
      digitalWrite(LED, LOW); // Turn on the LED
    } else if (payloadStr == "led1off") {
      digitalWrite(LED, HIGH); // Turn off the LED
    }
  }
}

void setup() {

  Serial.begin(115200);
  pinMode(RY1, OUTPUT);
  pinMode(LED, OUTPUT);
  
  wifiManager.addParameter(&custom_mqtt_server);
  wifiManager.addParameter(&custom_mqtt_port);
  wifiManager.addParameter(&custom_mqtt_user);
  wifiManager.addParameter(&custom_mqtt_password);
  wifiManager.autoConnect();

  // Set the MQTT server and port
  mqtt.setServer(custom_mqtt_server.getValue(), atoi(custom_mqtt_port.getValue()));
  mqtt.setCallback(onMessage);
  mqtt.setClient(wlan);
}

void loop() {
  if (!mqtt.connected()) {
    reconnect();
  }
  mqtt.loop();
}