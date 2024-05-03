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

const String sta_ssid = "your_ssid";
const String sta_password = "your_password";

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

  String chip_id = String(ESP.getChipId(), HEX).substring(8);
  String hostname = "wificar-" + chip_id;
  Serial.println("Hostname: " + hostname);

  WiFi.mode(WIFI_STA);
  WiFi.begin(sta_ssid.c_str(), sta_password.c_str());
  Serial.println("");
  Serial.print("Connecting to: ");
  Serial.println(sta_ssid);
  Serial.print("Password: ");
  Serial.println(sta_password);

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
    WiFi.softAP(hostname.c_str());
    IPAddress myIP = WiFi.softAPIP();
    Serial.println("");
    Serial.println("WiFi failed connected to " + sta_ssid);
    Serial.println("");
    Serial.println("*WiFi-AP-Mode*");
    Serial.print("AP IP address: ");
    Serial.println(myIP);
    digitalWrite(wifiLedPin, HIGH);
    delay(3000);
  }

  server.on("/", HTTP_handleRoot);
  server.onNotFound(HTTP_handleRoot);
  server.begin();
  ArduinoOTA.begin();
}

void loop() {
  ArduinoOTA.handle();
  server.handleClient();

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
}

void HTTP_handleRoot() {
  server.send(200, "text/html", "");

  if (server.hasArg("State")) {
    Serial.println(server.arg("State"));
  }
}

void handleNotFound() {
  server.send(404, "text/plain", "404: Not found");
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
