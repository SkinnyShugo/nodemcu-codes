#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

// Replace with your network credentials
const char* ssid = "YourSSID";
const char* password = "YourPassword";

// Define the GPIO pin connected to the relay
const int relayPin = 5; // D1

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);
  pinMode(relayPin, OUTPUT);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", "<html><body><h1>Control Light Bulb</h1><p><a href=\"/on\">Turn On</a></p><p><a href=\"/off\">Turn Off</a></p></body></html>");
  });

  // Route to turn on the light bulb
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request) {
    digitalWrite(relayPin, HIGH);
    request->send(200, "text/plain", "Light Bulb turned on");
  });

  // Route to turn off the light bulb
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request) {
    digitalWrite(relayPin, LOW);
    request->send(200, "text/plain", "Light Bulb turned off");
  });

  // Start server
  server.begin();
}

void loop() {
  // Nothing to do here
}
