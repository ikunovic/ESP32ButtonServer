#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include "wifi_credentials.h"

// Pin Definitions
const int BUTTON_PIN = 4;  // GPIO4 for the button input
const int LED_PIN = 2;     // Built-in LED

// Server state
bool serverRunning = false;
unsigned long serverStartTime = 0;

// Web server instance
AsyncWebServer server(80);
AsyncEventSource events("/events");  // Server-Sent Events

// Button state variables
bool lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

// CORS headers
const char* CORS_HEADER = "Access-Control-Allow-Origin";
const char* CORS_VALUE = "*";  // Allow any origin for demo. In production, limit to your Netlify/GitHub Pages domain.

// Function declarations
void setupWiFi();
void setupServer();
void handleButtonPress();
void blinkLED(int times);
void startServerFunction();  // New function for remote start

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  
  // Configure pins
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  
  // Initial LED state - off
  digitalWrite(LED_PIN, LOW);
  
  // Initialize SPIFFS for static files
  if(!SPIFFS.begin(true)) {
    Serial.println("SPIFFS Mount Failed");
  }
  
  // Setup WiFi connection
  setupWiFi();
  
  // Auto-start the server on boot
  if (!serverRunning) {
    Serial.println("Auto-starting server on boot");
    startServerFunction();
  }
  
  Serial.println("ESP32 Button Server initialized. Press the button or use web interface to control server.");
}

void loop() {
  // Handle physical button press
  handleButtonPress();
  
  // If server is running, periodically send status updates (every 5 seconds)
  if (serverRunning && (millis() % 5000 < 50)) {
    String status = "{\"status\":\"running\",\"uptime\":" + String((millis() - serverStartTime) / 1000) + "}";
    events.send(status.c_str(), "status", millis());
  }
}

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
    blinkLED(2);  // Indicate WiFi connected
  } else {
    Serial.println();
    Serial.println("Failed to connect to WiFi. Check credentials.");
    blinkLED(5);  // Indicate WiFi failure
  }
}

void setupServer() {
  // Add CORS headers to all responses
  DefaultHeaders::Instance().addHeader(CORS_HEADER, CORS_VALUE);
  
  // Define server endpoints
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    String html = "<html><head>";
    html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
    html += "<style>body{font-family:Arial,sans-serif;margin:0;padding:20px;text-align:center;background:#f5f5f5}";
    html += "h1{color:#333}p{color:#666}.status{font-weight:bold;color:#4CAF50}</style>";
    html += "</head><body>";
    html += "<h1>ESP32 Button Server</h1>";
    html += "<p>Server is running!</p>";
    html += "<p>Current status: <span class='status'>ACTIVE</span></p>";
    html += "<p>Visit <a href='https://YOUR-GITHUB-USERNAME.github.io/ESP32ButtonServer/' target='_blank'>the control portal</a> to control this server remotely.</p>";
    html += "</body></html>";
    request->send(200, "text/html", html);
  });

  // API endpoint for status
  server.on("/api/status", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    DynamicJsonDocument doc(256);
    doc["status"] = "running";
    doc["uptime"] = (millis() - serverStartTime) / 1000;
    doc["ip"] = WiFi.localIP().toString();
    serializeJson(doc, *response);
    request->send(response);
  });
  
  // API endpoint to get current IP address (used by web portal)
  server.on("/api/ip", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    DynamicJsonDocument doc(128);
    doc["ip"] = WiFi.localIP().toString();
    serializeJson(doc, *response);
    request->send(response);
  });
  
  // Remote start endpoint
  server.on("/api/start", HTTP_POST, [](AsyncWebServerRequest *request) {
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    DynamicJsonDocument doc(128);
    
    if (!serverRunning) {
      startServerFunction();
      doc["success"] = true;
      doc["message"] = "Server started successfully";
    } else {
      doc["success"] = false;
      doc["message"] = "Server is already running";
    }
    
    serializeJson(doc, *response);
    request->send(response);
  });
  
  // CORS preflight for the start endpoint
  server.on("/api/start", HTTP_OPTIONS, [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "OK");
    response->addHeader("Access-Control-Allow-Methods", "POST, OPTIONS");
    response->addHeader("Access-Control-Allow-Headers", "Content-Type");
    request->send(response);
  });
  
  // Event source for real-time updates
  events.onConnect([](AsyncEventSourceClient *client) {
    if(client->lastId()) {
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    client->send("hello!", NULL, millis(), 10000);
  });
  server.addHandler(&events);
  
  // Start server
  server.begin();
  Serial.println("HTTP server started");
  Serial.print("Server running at: http://");
  Serial.println(WiFi.localIP());
  
  // Record server start time
  serverStartTime = millis();
}

// Common function to start the server (used by both physical button and remote API)
void startServerFunction() {
  Serial.println("Starting server...");
  serverRunning = true;
  setupServer();
  blinkLED(3);  // Indicate server started
  
  // Send event notification
  String notificationJson = "{\"event\":\"server_started\",\"timestamp\":" + String(millis()) + "}";
  events.send(notificationJson.c_str(), "notification", millis());
}

void handleButtonPress() {
  // Read button with debounce
  int reading = digitalRead(BUTTON_PIN);
  
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }
  
  // If button state has been stable for the debounce delay
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // If the button is pressed (LOW when using INPUT_PULLUP)
    if (reading == LOW && lastButtonState == HIGH) {
      // Button was just pressed
      if (!serverRunning) {
        Serial.println("Button pressed - starting server");
        startServerFunction();
      }
    }
  }
  
  lastButtonState = reading;
}

void blinkLED(int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
    delay(200);
  }
} 