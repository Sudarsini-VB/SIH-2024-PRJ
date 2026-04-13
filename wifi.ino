#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "Toilet";
const char* password = "12345678";

const int motorPin1 = D0; // IN1 on L298N
const int motorPin2 = D1; // IN2 on L298N

ESP8266WebServer server(80);

void setup() {
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);

  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, LOW);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }

  server.on("/", HTTP_GET, handleRoot);
  server.on("/forward", HTTP_GET, moveForward);
  server.on("/backward", HTTP_GET, moveBackward);
  server.on("/stop", HTTP_GET, stopMotor);

  server.begin();
}

void loop() {
  server.handleClient();
}

void handleRoot() {
  String html = "<html><body><h1>Motor Control</h1>";
  html += "<a href=\"/forward\"><button>Move Forward</button></a><br><br>";
  html += "<a href=\"/backward\"><button>Move Backward</button></a><br><br>";
  html += "<a href=\"/stop\"><button>Stop</button></a>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void moveForward() {
  digitalWrite(motorPin1, HIGH);
  digitalWrite(motorPin2, LOW);
  server.send(200, "text/html", "<html><body><h1>Motor is Moving Forward</h1><a href=\"/\">Go Back</a></body></html>");
}

void moveBackward() {
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, HIGH);
  server.send(200, "text/html", "<html><body><h1>Motor is Moving Backward</h1><a href=\"/\">Go Back</a></body></html>");
}

void stopMotor() {
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, LOW);
  server.send(200, "text/html", "<html><body><h1>Motor is Stopped</h1><a href=\"/\">Go Back</a></body></html>");
}