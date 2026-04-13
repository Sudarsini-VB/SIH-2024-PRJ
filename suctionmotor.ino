#define CW D7
#define CCW D8

void setup() {
	pinMode(CW, OUTPUT);
	pinMode(CCW, OUTPUT);
}

void loop() { 
	digitalWrite(CW,HIGH);
	delay(1000); 
	digitalWrite(CW, LOW);
	digitalWrite(CCW, HIGH);
	delay(1000);
	digitalWrite(CCW, LOW); 
	delay(5000);
}