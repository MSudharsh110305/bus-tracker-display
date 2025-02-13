// Used ESP8266 and SIM800L

#include <SoftwareSerial.h>

#define RX 3  // SIM800 TX connected to D7 (GPIO13)
#define TX 1  // SIM800 RX connected to D8 (GPIO15)

SoftwareSerial sim800(RX, TX);  // Create a serial connection to SIM800L

const String FIREBASE_HOST = "bustracker-6c517-default-rtdb.firebaseio.com";
const String FIREBASE_SECRET = "KJW4RssVia2RMg4l2DpwpYLVe96EJhlsuHyTR0fm";
const String APN = "airtelgprs.com";  // Airtel SIM APN

void setup() {
    Serial.begin(115200);
    sim800.begin(9600);
    Serial.println("Initializing SIM800L...");

    connectToGSM();
}

void loop() {
    Serial.println("\nFetching bus1 data from Firebase...");
    String jsonData = fetchBusData();

    if (jsonData.length() > 0) {
        Serial.println("Received Data: " + jsonData);
        sendStatusUpdate(true);
    } else {
        Serial.println("No valid data received!");
        sendStatusUpdate(false);
    }

    delay(30000); // Fetch data every 30 seconds
}

// Connect to GSM Network
void connectToGSM() {
    sendATCommand("AT", "OK", 2000);
    sendATCommand("AT+CPIN?", "READY", 2000);
    sendATCommand("AT+CREG?", "0,1", 2000);
    sendATCommand("AT+CGATT=1", "OK", 2000);
    sendATCommand("AT+CSTT=\"" + APN + "\"", "OK", 3000);
    sendATCommand("AT+CIICR", "OK", 5000);
    sendATCommand("AT+CIFSR", ".", 3000);
    Serial.println("GPRS Connected!");
}

// Fetch Bus1 Data from Firebase
String fetchBusData() {
    String url = FIREBASE_HOST + "/busData/bus1.json?auth=" + FIREBASE_SECRET;
    return sendHTTPGET(url);
}

// Send Status Update to Firebase
void sendStatusUpdate(bool status) {
    String url = FIREBASE_HOST + "/status.json?auth=" + FIREBASE_SECRET;
    String payload = "{\"validDataReceived\": " + String(status ? "true" : "false") + "}";
    sendHTTPPOST(url, payload);
}

// Send HTTP GET request using SIM800L
String sendHTTPGET(String url) {
    Serial.println("Sending HTTP GET Request...");
    sendATCommand("AT+HTTPINIT", "OK", 2000);
    sendATCommand("AT+HTTPPARA=\"CID\",1", "OK", 2000);
    sendATCommand("AT+HTTPPARA=\"URL\",\"" + url + "\"", "OK", 3000);
    sendATCommand("AT+HTTPACTION=0", "200", 10000);  // 0 = GET
    String response = readHTTPResponse();
    sendATCommand("AT+HTTPTERM", "OK", 2000);
    return response;
}

// Send HTTP POST request using SIM800L
void sendHTTPPOST(String url, String payload) {
    Serial.println("Sending HTTP POST Request...");
    sendATCommand("AT+HTTPINIT", "OK", 2000);
    sendATCommand("AT+HTTPPARA=\"CID\",1", "OK", 2000);
    sendATCommand("AT+HTTPPARA=\"URL\",\"" + url + "\"", "OK", 3000);
    sendATCommand("AT+HTTPPARA=\"CONTENT\",\"application/json\"", "OK", 2000);
    sendATCommand("AT+HTTPDATA=" + String(payload.length()) + ",5000", "DOWNLOAD", 5000);
    sim800.println(payload);
    delay(3000);
    sendATCommand("AT+HTTPACTION=1", "200", 10000);  // 1 = POST
    sendATCommand("AT+HTTPTERM", "OK", 2000);
}

// Send AT Command to SIM800L
void sendATCommand(String command, String expectedResponse, int timeout) {
    Serial.println("AT Command: " + command);
    sim800.println(command);
    long int time = millis();
    while ((millis() - time) < timeout) {
        if (sim800.find(expectedResponse.c_str())) {
            Serial.println("Success: " + command);
            return;
        }
    }
    Serial.println("Failed: " + command);
}

// Read HTTP Response from SIM800L
String readHTTPResponse() {
    String response = "";
    while (sim800.available()) {
        char c = sim800.read();
        response += c;
        delay(1);
    }
    Serial.println("HTTP Response: " + response);
    return response;
}
