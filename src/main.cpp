#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiManager.h>

// WiFiManager settings
char mqttServer[40] = "192.168.8.160"; // Default MQTT server
char mqttPort[6] = "1883"; // Default MQTT port
char mqttUser[40] = ""; // Default MQTT user (can be an empty string)
char mqttPassword[40] = ""; // Default MQTT password (can be an empty string)

const char* apName = "ESP8266 Relay";
const char* clientID = "esp8266-relay";
const char* topic = "relay"; // Topic to control the relay

const int RY1 = 0;
const int LED = BUILTIN_LED;

WiFiClient wlan;
PubSubClient mqtt(wlan);

// WiFiManager callback function
void saveConfigCallback() {
  Serial.println("Configuration saved");
  strcpy(mqttServer, WiFiManagerParameter("mqtt_server").getValue());
  strcpy(mqttPort, WiFiManagerParameter("mqtt_port").getValue());
  strcpy(mqttUser, WiFiManagerParameter("mqtt_user").getValue());
  strcpy(mqttPassword, WiFiManagerParameter("mqtt_password").getValue());
}

void reconnect() {
  while (!mqtt.connected()) {
    Serial.println("Attempting to connect to the MQTT server...");
    if (mqtt.connect(clientID, mqttUser, mqttPassword)) {
      Serial.println("MQTT reconnected");
      mqtt.subscribe(topic); // Subscribe to the control topic
    } else {
      Serial.print("MQTT connect failed, retrying...");
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

  Serial.print("Received message: [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.println(payloadStr);

  if (strcmp(topic, topic) == 0) { // Compare with the control topic
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
  pinMode(RY1, OUTPUT);
  pinMode(LED, OUTPUT);

  WiFiManager wifiManager;
  // Add custom parameters for configuring MQTT server information
  WiFiManagerParameter custom_mqtt_server("mqtt_server", "MQTT Server", mqttServer, 40);
  WiFiManagerParameter custom_mqtt_port("mqtt_port", "MQTT Port", mqttPort, 6);
  WiFiManagerParameter custom_mqtt_user("mqtt_user", "MQTT User", mqttUser, 40);
  WiFiManagerParameter custom_mqtt_password("mqtt_password", "MQTT Password", mqttPassword, 40);

  wifiManager.setSaveConfigCallback(saveConfigCallback);
  wifiManager.addParameter(&custom_mqtt_server);
  wifiManager.addParameter(&custom_mqtt_port);
  wifiManager.addParameter(&custom_mqtt_user);
  wifiManager.addParameter(&custom_mqtt_password);

  // Initialize WiFi connection and connect to a saved WiFi network or start AP mode for configuration
  wifiManager.autoConnect(apName);

  // Set the MQTT server and port
  mqtt.setServer(mqttServer, atoi(mqttPort));
  mqtt.setCallback(onMessage);
  mqtt.setClient(wlan);
}

void loop() {
  if (!mqtt.connected()) {
    reconnect();
  }
  mqtt.loop();
}