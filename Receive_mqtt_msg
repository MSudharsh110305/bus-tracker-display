#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// WiFi Credentials
const char* ssid = "ABCD";  // Replace with your WiFi SSID
const char* password = "123456789";  // Replace with your WiFi Password

// MQTT Broker Details
const char* mqtt_server = "34.100.196.132";  // Replace with your broker's IP
const int mqtt_port = 1883;  // Standard MQTT port

// MQTT Topic
const char* topic_nmea = "display/nmea";

WiFiClient espClient;
PubSubClient client(espClient);

// Callback function to handle incoming MQTT messages
void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message received on topic: ");
    Serial.println(topic);
    
    String message;
    for (unsigned int i = 0; i < length; i++) {
        message += (char)payload[i];
    }

    Serial.println("NMEA Data: " + message);
}

void setup() {
    Serial.begin(115200);
    
    // Connect to WiFi
    Serial.print("Connecting to WiFi...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected!");

    // Configure MQTT
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(callback);
    
    connectToMQTT();
}

void loop() {
    if (!client.connected()) {
        connectToMQTT();
    }
    client.loop();
}

// Connect to MQTT Broker
void connectToMQTT() {
    Serial.print("Connecting to MQTT...");
    while (!client.connected()) {
        if (client.connect("ESP8266_Client")) {
            Serial.println("\nMQTT connected!");
            
            // Subscribe to NMEA topic
            client.subscribe(topic_nmea);
            Serial.println("Subscribed to topic: display/nmea");
        } else {
            Serial.print(".");
            delay(2000);
        }
    }
}
