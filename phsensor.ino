#define pH7Voltage 5
#define pHScaleFactor 3.5

const int sensorPin = A0;

void setup() {
  Serial.begin(115200);
  pinMode(sensorPin, INPUT);
} 

void loop() {
  int rawValue = analogRead(sensorPin);
  float voltage = rawValue * (3.3 / 1024.0);
  float pH = 7.0 + ((pH7Voltage - voltage) * pHScaleFactor);

  Serial.print("Voltage: ");
  Serial.print(voltage, 3);
  Serial.print(" V, pH: ");
  Serial.println(pH, 2);

  delay(1000);
}