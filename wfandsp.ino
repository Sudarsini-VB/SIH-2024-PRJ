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

        // Pump Control Logic
        if (flowRate < 0.5) { // Start pump in clockwise direction if flow rate > 0.5 L/min
            Serial.println("Pump Activated (CW)");
            digitalWrite(CW, HIGH);
            digitalWrite(CCW, LOW); // Ensure counterclockwise pin is LOW
        } else { // Stop the pump if flow rate is <= 0.5 L/min
            Serial.println("Pump Stopped");
            digitalWrite(CW, LOW);
            digitalWrite(CCW, LOW);
        }
    }

    // Additional delay to stabilize readings
    delay(100);
}   
