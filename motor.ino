// Motor Control Configuration
const int motorPin1 = D3; // Motor control pin 1
const int motorPin2 = D4; // Motor control pin 2

// Flow Sensor Configuration
volatile int pulseCount = 0;
const int flowSensorPin = D2; // Flow sensor pin
const float calibrationFactor = 4.5;

float flowRate = 0.0;
float flowLiters = 0.0;
unsigned long previousMillis = 0;
const unsigned long interval = 1000;

// Pump Configuration
#define CW D7 // Pump clockwise control pin
#define CCW D8 // Pump counterclockwise control pin

void IRAM_ATTR pulseCounter() {
    pulseCount++;
}

void setup() {
    // Initialize Serial Monitor
    Serial.begin(115200);

    // Flow Sensor Setup
    pinMode(flowSensorPin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(flowSensorPin), pulseCounter, RISING);

    // Pump Pins Setup
    pinMode(CW, OUTPUT);
    pinMode(CCW, OUTPUT);
    digitalWrite(CW, LOW);
    digitalWrite(CCW, LOW);

    // Motor Pins Setup
    pinMode(motorPin1, OUTPUT);
    pinMode(motorPin2, OUTPUT);
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, LOW);

    Serial.println("System Initialized");
}

void loop() {
    unsigned long currentMillis = millis();

    // Flow Sensor Calculation
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;

        flowRate = (pulseCount / calibrationFactor); // Calculate flow rate in L/min
        flowLiters += (flowRate / 60);               // Convert flow rate to total liters

        // Print Flow Sensor Data
        Serial.print("Flow Rate (L/min): ");
        Serial.print(flowRate);
        Serial.print("\tTotal Flow (L): ");
        Serial.println(flowLiters);

        pulseCount = 0; // Reset pulse count

        // Pump and Motor Control Logic
        if (flowRate < 0.5) { // Start pump and motor in CW direction if flow rate < 0.5 L/min
            Serial.println("Flow Rate Low: Activating Pump and Motor (CW)");
            digitalWrite(CW, HIGH);
            digitalWrite(CCW, LOW);
            digitalWrite(motorPin1, HIGH);
            digitalWrite(motorPin2, LOW);
            delay(2000); // Run motor for 2 seconds
        } else if (flowRate > 0.5 && flowRate <= 1.0) { // Stop pump and motor if flow rate is between 0.5 and 1.0
            Serial.println("Flow Rate Moderate: Stopping Pump and Motor");
            digitalWrite(CW, LOW);
            digitalWrite(CCW, LOW);
            digitalWrite(motorPin1, LOW);
            digitalWrite(motorPin2, LOW);
            delay(10000); // Delay for 10 seconds
        } else { // Reverse motor if flow rate > 1.0
            Serial.println("Flow Rate High: Activating Motor in Reverse (CCW)");
            digitalWrite(CW, LOW);
            digitalWrite(CCW, LOW);
            digitalWrite(motorPin1, LOW);
            digitalWrite(motorPin2, HIGH); 
            delay(2000); // Run motor in reverse for 2 seconds
            digitalWrite(motorPin1, LOW);
            digitalWrite(motorPin2, LOW);
            delay(10000); // Delay for 10 seconds
        }
    }

    // Additional delay to stabilize readings
    delay(100);
}
