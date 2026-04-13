#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "RedmiNote11";       
const char* password = "sudarsini"; 

ESP8266WebServer server(80);

const int motorPin1 = D3; 
const int motorPin2 = D4; 
#define CW D7             
#define CCW D8            

volatile int pulseCount = 0;
const int flowSensorPin = D2;
const float calibrationFactor = 4.5;

float flowRate = 0.0;
float flowLiters = 0.0;
unsigned long previousMillis = 0;
const unsigned long interval = 1000;

bool systemActive = false; 

void IRAM_ATTR pulseCounter() {
    pulseCount++;
}

void handleRoot() {
    Serial.println("Handling root page request...");
    String html = "<html>\
        <head>\
            <title>iFlush</title>\
            <script>\
                setInterval(function(){\
                    fetch('/flowrate')\
                    .then(response => response.json())\
                    .then(data => {\
                        document.getElementById('flowRate').innerText = 'Flow Rate (L/min): ' + data.flowRate;\
                        document.getElementById('flowLiters').innerText = 'Total Flow (L): ' + data.flowLiters;\
                    });\
                }, 5000);\
            </script>\
        </head>\
        <body>\
            <h1>iFlush System</h1>\
            <p id='flowRate'>Flow Rate (L/min): " + String(flowRate) + "</p>\
            <p id='flowLiters'>Total Flow (L): " + String(flowLiters) + "</p>\
            <form action='/activate' method='GET'>\
                <button type='submit'>Activate System</button>\
            </form>\
            <form action='/deactivate' method='GET'>\
                <button type='submit'>Deactivate System</button>\
            </form>\
            <h2>Test Motor and Pump</h2>\
            <form action='/testMotor' method='GET'>\
                <button type='submit'>Test Motor</button>\
            </form>\
            <form action='/testPump' method='GET'>\
                <button type='submit'>Test Pump</button>\
            </form>\
        </body>\
    </html>";
    server.send(200, "text/html", html);
}

void handleActivate() {
    Serial.println("System activated");
    systemActive = true;
    server.send(200, "text/html", "<h1>System Activated</h1><a href='/'>Go Back</a>");
}

void handleDeactivate() {
    Serial.println("System deactivated");
    systemActive = false;
    stopAllMotors();
    server.send(200, "text/html", "<h1>System Deactivated</h1><a href='/'>Go Back</a>");
}

void handleTestMotor() {
    Serial.println("Testing motor...");
    stopAllMotors();
    digitalWrite(motorPin1, HIGH);  
    digitalWrite(motorPin2, LOW);
    delay(2000); 
    stopAllMotors();
    server.send(200, "text/html", "<h1>Motor Tested (Forward)</h1><a href='/'>Go Back</a>");
}

void handleTestPump() {
    Serial.println("Testing pump...");
    stopAllMotors();
    digitalWrite(CW, HIGH); 
    delay(2000); 
    digitalWrite(CW, LOW);  
    digitalWrite(CCW, HIGH); 
    delay(2000); 
    digitalWrite(CCW, LOW); 
    server.send(200, "text/html", "<h1>Pump Tested (CW/CCW)</h1><a href='/'>Go Back</a>");
}

void stopAllMotors() {
    Serial.println("Stopping all motors...");
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, LOW);
    digitalWrite(CW, LOW);
    digitalWrite(CCW, LOW);
}

void handleFlowRate() {
    flowRate = (pulseCount / calibrationFactor); 
    flowLiters += (flowRate / 60);               
    pulseCount = 0;

    String jsonResponse = "{\"flowRate\": " + String(flowRate, 2) + ", \"flowLiters\": " + String(flowLiters, 2) + "}";
    Serial.print("Flow Rate: ");
    Serial.print(flowRate);
    Serial.print(" L/min, Total Flow: ");
    Serial.print(flowLiters);
    Serial.println(" L");

    server.send(200, "application/json", jsonResponse);
}

void setup() {
    Serial.begin(115200);
    Serial.println("Starting system...");

    pinMode(flowSensorPin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(flowSensorPin), pulseCounter, RISING);

    pinMode(CW, OUTPUT);
    pinMode(CCW, OUTPUT);
    digitalWrite(CW, LOW);
    digitalWrite(CCW, LOW);

    pinMode(motorPin1, OUTPUT);
    pinMode(motorPin2, OUTPUT);
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, LOW);

    WiFi.mode(WIFI_STA);
    int n = WiFi.scanNetworks();
    for (int i = 0; i < n; i++) {
        Serial.println(WiFi.SSID(i));
    }

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
    }
    Serial.println("\nConnected to Wi-Fi");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    server.on("/", handleRoot);
    server.on("/activate", handleActivate);
    server.on("/deactivate", handleDeactivate);
    server.on("/testMotor", handleTestMotor);
    server.on("/testPump", handleTestPump);
    server.on("/flowrate", handleFlowRate);  // Endpoint for flow rate data
    server.begin();
}

void loop() {
    server.handleClient();

    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;

        flowRate = (pulseCount / calibrationFactor); 
        flowLiters += (flowRate / 60);               
        pulseCount = 0;

        Serial.print("Flow Rate: ");
        Serial.print(flowRate);
        Serial.print(" L/min, Total Flow: ");
        Serial.print(flowLiters);
        Serial.println(" L - test");

        if (systemActive) {
            if (flowRate < 0.5) { 
                Serial.println("Flow rate is low, starting motor forward...");
                digitalWrite(CW, HIGH);
                digitalWrite(CCW, LOW);
                digitalWrite(motorPin1, HIGH);
                digitalWrite(motorPin2, LOW);
                delay(2000); 
            } else if (flowRate > 0.5 && flowRate <= 1.0) {

              for (int i = 0; i < 10; i++) {
                stopAllMotors();
                delay(500);                              
                digitalWrite(CW, HIGH);
                digitalWrite(CCW, LOW);
                digitalWrite(motorPin1, HIGH);
                digitalWrite(motorPin2, LOW);
                delay(500); 
              }
                Serial.println("Flow rate is moderate, running motor moderately...");
                stopAllMotors();
                delay(10000); 
            } else {   
                Serial.println("Flow rate is high, stopping motor...");
                stopAllMotors();
                delay(10000); 
            }
        }
    }

    delay(100); 
}