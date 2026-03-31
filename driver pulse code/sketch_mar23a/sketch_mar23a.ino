const int pulPin = 13; 
const int dirPin = 12; 

// UPDATE: Change this to match your new DIP switch setting (e.g., 1600)
const int stepsPerRev = 400; 

// New Math for 1 Rev/Sec at 1600 steps:
// 1,000,000us / 1600 steps / 2 = 312.5us
const int pulseDelay = 312; 

void setup() {
  pinMode(pulPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
}

void loop() {
  digitalWrite(dirPin, HIGH); 
  for(int i = 0; i < stepsPerRev; i++) {
    digitalWrite(pulPin, HIGH);
    delayMicroseconds(pulseDelay);
    digitalWrite(pulPin, LOW);
    delayMicroseconds(pulseDelay);
  }
  delay(1000);

  digitalWrite(dirPin, LOW); 
  for(int i = 0; i < stepsPerRev; i++) {
    digitalWrite(pulPin, HIGH);
    delayMicroseconds(pulseDelay);
    digitalWrite(pulPin, LOW);
    delayMicroseconds(pulseDelay);
  }
  delay(1000);
}