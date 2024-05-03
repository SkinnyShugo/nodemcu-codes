#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>

// Motor pins
const int PWM_A = D1;
const int PWM_B = D2;
const int DIR_A = D3;
const int DIR_B = D4;

// Buzzer and LED pins
const int buzPin = D5;
const int ledPin = D8;
const int wifiLedPin = D0;

String command;
int SPEED = 1023;
int speed_Coeff = 3;

ESP8266WebServer server(80);

const char *ssid = "your_ssid";
const char *password = "your_password";

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("*WiFi Robot Remote Control Mode*");
  Serial.println("--------------------------------------");

  pinMode(buzPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(wifiLedPin, OUTPUT);
  digitalWrite(buzPin, LOW);
  digitalWrite(ledPin, LOW);
  digitalWrite(wifiLedPin, HIGH);

  pinMode(PWM_A, OUTPUT);
  pinMode(PWM_B, OUTPUT);
  pinMode(DIR_A, OUTPUT);
  pinMode(DIR_B, OUTPUT);
  digitalWrite(DIR_A, LOW);
  digitalWrite(DIR_B, LOW);
  analogWrite(PWM_A, 0);
  analogWrite(PWM_B, 0);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.print("Connecting to: ");
  Serial.println(ssid);
  Serial.print("Password: ");
  Serial.println(password);

  unsigned long previousMillis = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - previousMillis <= 10000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("*WiFi-STA-Mode*");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    digitalWrite(wifiLedPin, LOW);
    delay(3000);
  } else {
    WiFi.mode(WIFI_AP);
    IPAddress IP = WiFi.softAPIP();
    Serial.println("");
    Serial.println("WiFi failed connected to " + String(ssid));
    Serial.println("");
    Serial.println("*WiFi-AP-Mode*");
    Serial.print("AP IP address: ");
    Serial.println(IP);
    digitalWrite(wifiLedPin, HIGH);
    delay(3000);
  }

  server.on("/", handleRoot);
  server.on("/command", handleCommand);
  server.begin();
  ArduinoOTA.begin();
}

void loop() {
  ArduinoOTA.handle();
  server.handleClient();
}

void handleRoot() {
  String html = "<!DOCTYPE html><html><head><title>WiFi Robot Control</title></head><body>";
  html += "<h1>WiFi Robot Control</h1>";
  html += "<form action=\"/command\">";
  html += "<button name=\"State\" value=\"F\">Forward</button>";
  html += "<button name=\"State\" value=\"B\">Backward</button>";
  html += "<button name=\"State\" value=\"R\">Turn Right</button>";
  html += "<button name=\"State\" value=\"L\">Turn Left</button>";
  html += "<button name=\"State\" value=\"G\">Forward Left</button>";
  html += "<button name=\"State\" value=\"H\">Backward Left</button>";
  html += "<button name=\"State\" value=\"I\">Forward Right</button>";
  html += "<button name=\"State\" value=\"J\">Backward Right</button>";
  html += "<button name=\"State\" value=\"S\">Stop</button>";
  html += "<button name=\"State\" value=\"V\">Beep Horn</button>";
  html += "<button name=\"State\" value=\"W\">Turn Light On</button>";
  html += "<button name=\"State\" value=\"w\">Turn Light Off</button>";
  html += "<label for=\"speed\">Speed:</label>";
  html += "<input type=\"range\" id=\"speed\" name=\"State\" min=\"0\" max=\"9\">";
  html += "<input type=\"submit\" value=\"Set Speed\">";
  html += "</form></body></html>";

  server.send(200, "text/html", html);
}

void handleCommand() {
  command = server.arg("State");

  if (command == "F") Forward();
  else if (command == "B") Backward();
  else if (command == "R") TurnRight();
  else if (command == "L") TurnLeft();
  else if (command == "G") ForwardLeft();
  else if (command == "H") BackwardLeft();
  else if (command == "I") ForwardRight();
  else if (command == "J") BackwardRight();
  else if (command == "S") Stop();
  else if (command == "V") BeepHorn();
  else if (command == "W") TurnLightOn();
  else if (command == "w") TurnLightOff();
  else if (command >= "0" && command <= "9") SPEED = 330 + (command.toInt() * 70);

  handleRoot(); // Refresh the page after command execution
}

void Forward() {
  digitalWrite(DIR_A, HIGH);
  digitalWrite(DIR_B, HIGH);
  analogWrite(PWM_A, SPEED);
  analogWrite(PWM_B, SPEED);
}

void Backward() {
  digitalWrite(DIR_A, LOW);
  digitalWrite(DIR_B, LOW);
  analogWrite(PWM_A, SPEED);
  analogWrite(PWM_B, SPEED);
}

void TurnRight() {
  digitalWrite(DIR_A, LOW);
  digitalWrite(DIR_B, HIGH);
  analogWrite(PWM_A, SPEED);
  analogWrite(PWM_B, SPEED);
}

void TurnLeft() {
  digitalWrite(DIR_A, HIGH);
  digitalWrite(DIR_B, LOW);
  analogWrite(PWM_A, SPEED);
  analogWrite(PWM_B, SPEED);
}

void ForwardLeft() {
  digitalWrite(DIR_A, HIGH);
  digitalWrite(DIR_B, HIGH);
  analogWrite(PWM_A, SPEED);
  analogWrite(PWM_B, SPEED / speed_Coeff);
}

void BackwardLeft() {
  digitalWrite(DIR_A, LOW);
  digitalWrite(DIR_B, LOW);
  analogWrite(PWM_A, SPEED);
  analogWrite(PWM_B, SPEED / speed_Coeff);
}

void ForwardRight() {
  digitalWrite(DIR_A, HIGH);
  digitalWrite(DIR_B, HIGH);
  analogWrite(PWM_A, SPEED / speed_Coeff);
  analogWrite(PWM_B, SPEED);
}

void BackwardRight() {
  digitalWrite(DIR_A, LOW);
  digitalWrite(DIR_B, LOW);
  analogWrite(PWM_A, SPEED / speed_Coeff);
  analogWrite(PWM_B, SPEED);
}

void Stop() {
  digitalWrite(DIR_A, LOW);
  digitalWrite(DIR_B, LOW);
  analogWrite(PWM_A, 0);
  analogWrite(PWM_B, 0);
}

void BeepHorn() {
  digitalWrite(buzPin, HIGH);
  delay(150);
  digitalWrite(buzPin, LOW);
  delay(80);
}

void TurnLightOn() {
  digitalWrite(ledPin, HIGH);
}

void TurnLightOff() {
  digitalWrite(ledPin, LOW);
}
