#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char *ssid = "wifi@lsong.one";
const char *password = "song940@163.com";
const char *mqttServer = "broker.emqx.io";
const char *clientID = "esp8266-relay-220v";
const char *relayTopic = "esp8266-relay-220v";

const int RY1 = 4;
const int BTN1 = 12;
const int LED1 = LED_BUILTIN;

WiFiClient wlan;
PubSubClient mqtt(wlan);

void reconnect()
{
  while (!mqtt.connected())
  {
    Serial.println("Attempting to connect to the MQTT server...");
    if (mqtt.connect(clientID))
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

void onMessage(char *topic, byte *payload, unsigned int length)
{
  // Handle received messages
  String payloadStr = "";
  for (unsigned int i = 0; i < length; i++)
  {
    payloadStr += (char)payload[i];
  }
  Serial.println("Received message: [" + String(topic) + "] " + payloadStr);

  if (strcmp(topic, relayTopic) == 0)
  { // Compare with the control topic
    if (payloadStr == "relay1on")
    {
      digitalWrite(RY1, HIGH); // Open the first relay
    }
    else if (payloadStr == "relay1off")
    {
      digitalWrite(RY1, LOW); // Close the first relay
    }
    else if (payloadStr == "led1on")
    {
      digitalWrite(LED1, LOW); // Turn on the LED
    }
    else if (payloadStr == "led1off")
    {
      digitalWrite(LED1, HIGH); // Turn off the LED
    }
  }
}

void setup()
{

  Serial.begin(115200);
  pinMode(RY1, OUTPUT);
  pinMode(LED1, OUTPUT);

  // Connect to WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Set the MQTT server and port
  mqtt.setServer(mqttServer, 1883);
  mqtt.setCallback(onMessage);
  mqtt.setClient(wlan);
}

void loop()
{
  if (!mqtt.connected())
  {
    reconnect();
  }
  mqtt.loop();
}