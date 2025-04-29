#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>

// Pin Definitions
const int LED_PIN = 2;  // Built-in LED

// WiFi credentials - Update these with your network information
const char* ssid = "Serenity";
const char* password = "12345678";

// LED state
bool ledState = false;

// Web server instance
AsyncWebServer server(80);

// CORS headers
const char* CORS_HEADER = "Access-Control-Allow-Origin";
const char* CORS_VALUE = "*";  // Allow any origin for demo. In production, limit to your domain.

void setupWiFi() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  
  // Wait for connection (with timeout)
  int timeout = 0;
  while (WiFi.status() != WL_CONNECTED && timeout < 20) {
    delay(500);
    Serial.print(".");
    timeout++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.print("Connected to WiFi. IP address: ");
    Serial.println(WiFi.localIP());
    
    // Blink LED twice to indicate WiFi connected
    for (int i = 0; i < 2; i++) {
      digitalWrite(LED_PIN, HIGH);
      delay(200);
      digitalWrite(LED_PIN, LOW);
      delay(200);
    }
  } else {
    Serial.println();
    Serial.println("Failed to connect to WiFi. Check credentials.");
    
    // Blink LED 5 times quickly to indicate WiFi failure
    for (int i = 0; i < 5; i++) {
      digitalWrite(LED_PIN, HIGH);
      delay(100);
      digitalWrite(LED_PIN, LOW);
      delay(100);
    }
  }
}

void setupServer() {
  // Add CORS headers to all responses
  DefaultHeaders::Instance().addHeader(CORS_HEADER, CORS_VALUE);
  
  // Define server endpoints
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "ESP32 LED Control Server Running. Connect to the web interface to control the LED.");
  });

  // API endpoint for status
  server.on("/api/status", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    DynamicJsonDocument doc(128);
    doc["status"] = "running";
    doc["ip"] = WiFi.localIP().toString();
    serializeJson(doc, *response);
    request->send(response);
  });
  
  // API endpoint to get LED status
  server.on("/api/led-status", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    DynamicJsonDocument doc(64);
    doc["ledState"] = ledState;
    serializeJson(doc, *response);
    request->send(response);
  });
  
  // API endpoint to toggle LED
  server.on("/api/toggle-led", HTTP_POST, [](AsyncWebServerRequest *request) {
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState ? HIGH : LOW);
    
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    DynamicJsonDocument doc(64);
    doc["ledState"] = ledState;
    doc["message"] = ledState ? "LED turned ON" : "LED turned OFF";
    serializeJson(doc, *response);
    request->send(response);
    
    Serial.println(ledState ? "LED turned ON" : "LED turned OFF");
  });
  
  // CORS preflight for the toggle-led endpoint
  server.on("/api/toggle-led", HTTP_OPTIONS, [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "OK");
    response->addHeader("Access-Control-Allow-Methods", "POST, OPTIONS");
    response->addHeader("Access-Control-Allow-Headers", "Content-Type");
    request->send(response);
  });
  
  // Start server
  server.begin();
  Serial.println("HTTP server started");
  Serial.print("Server running at: http://");
  Serial.println(WiFi.localIP());
}

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  
  // Configure LED pin
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);  // Initial LED state - off
  
  // Setup WiFi connection
  setupWiFi();
  
  // Setup and start the web server
  setupServer();
  
  Serial.println("ESP32 LED Control initialized. Use web interface to control the LED.");
}

void loop() {
  // Nothing to do here, the web server handles requests asynchronously
} 