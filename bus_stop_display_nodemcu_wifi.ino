#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>

const char* ssid = "ABCD";
const char* password = "123456789";
const String firebase_host = "bustracker-6c517-default-rtdb.firebaseio.com";
const String firebase_secret = "KJW4RssVia2RMg4l2DpwpYLVe96EJhlsuHyTR0fm";


WiFiClientSecure client;

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi");
    client.setInsecure();
}

void loop() {
    Serial.println("\nFetching bus1 data from Firebase...");
    String jsonData = fetchBusData("bus1");
    
    if (jsonData.length() > 0) {
        Serial.println("Received Data: " + jsonData);
        parseBusData(jsonData);
    } else {
        Serial.println("No valid data received!");
    }
    
    delay(30000); // Fetch data every 30 seconds
}

String fetchBusData(String busId) {
    String url = String("https://") + firebase_host + "/busData/" + busId + ".json?auth=" + firebase_secret;
    Serial.println("Requesting: " + url);
    
    client.connect(firebase_host, 443);
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + firebase_host + "\r\n" +
                 "Connection: close\r\n\r\n");
    
    String response;
    while (client.connected() || client.available()) {
        if (client.available()) {
            char c = client.read();
            response += c;
        }
    }
    client.stop();
    return extractJSON(response);
}

String extractJSON(String response) {
    int jsonStart = response.indexOf('{');
    int jsonEnd = response.lastIndexOf('}');
    if (jsonStart != -1 && jsonEnd != -1) {
        return response.substring(jsonStart, jsonEnd + 1);
    }
    return "";
}

void parseBusData(String jsonData) {
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, jsonData);
    
    if (error) {
        Serial.print("JSON Parsing failed: ");
        Serial.println(error.f_str());
        return;
    }
    
    Serial.println("\nBus Data:");
    Serial.print("ID: "); Serial.println(doc["id"].as<String>());
    Serial.print("Latitude: "); Serial.println(doc["latitude"].as<float>());
    Serial.print("Longitude: "); Serial.println(doc["longitude"].as<float>());
    Serial.print("Route: "); Serial.println(doc["route"].as<String>());
    Serial.print("Speed: "); Serial.println(doc["speed"].as<int>());
    Serial.print("Status: "); Serial.println(doc["status"].as<String>());
}
