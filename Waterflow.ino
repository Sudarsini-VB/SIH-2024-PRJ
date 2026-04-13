volatile int pulseCount = 0;
const int flowSensorPin = D2;
const float calibrationFactor = 4.5;

#define CW D7
#define CCW D8

float flowRate = 0.0;
float flowLiters = 0.0;
unsigned long previousMillis = 0;
const unsigned long interval = 1000;

void IRAM_ATTR pulseCounter() {
    pulseCount++;
}

void setup() {
  pinMode(CW, OUTPUT);
	pinMode(CCW, OUTPUT);
    Serial.begin(115200);
    pinMode(flowSensorPin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(flowSensorPin), pulseCounter, RISING);
}

void loop() {
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;

        flowRate = (pulseCount / calibrationFactor);
        flowLiters += (flowRate / 60);

        Serial.print("Flow Rate (L/min): ");
        Serial.print(flowRate);
        Serial.print("\tTotal Flow (L): ");
        Serial.println(flowLiters);

        pulseCount = 0;
        digitalWrite(CW,HIGH);
	delay(1000); 
	digitalWrite(CW, LOW);
	digitalWrite(CCW, HIGH);
	delay(1000);
	digitalWrite(CCW, LOW); 
	delay(5000);
    }
}


