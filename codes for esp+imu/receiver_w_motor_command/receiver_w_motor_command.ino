#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

// --- MOTOR CONFIGURATION ---
const int pulPin = 17; 
const int dirPin = 16; 
const int stepsPerRev = 400; // Match your CL57Y DIP switches
const int pulseDelay = 312; 
const int statusLed = 2;     

// --- DATA STRUCTURE ---
typedef struct struct_message {
  float r; float i; float j; float k;
} struct_message;

struct_message incomingData;

// --- TRACKING ---
long currentStepPos = 0; 
unsigned long lastPacketTime = 0;

// Convert Quaternion to Yaw (Degrees)
float getYaw(float r, float i, float j, float k) {
  float siny_cosp = 2 * (r * k + i * j);
  float cosy_cosp = 1 - 2 * (j * j + k * k);
  float yaw = atan2(siny_cosp, cosy_cosp);
  return yaw * (180.0 / PI); 
}

// Motor Movement Logic
void moveToStep(long targetStep) {
  // Determine Direction
  if (targetStep > currentStepPos) digitalWrite(dirPin, HIGH);
  else digitalWrite(dirPin, LOW);

  long stepsToMove = abs(targetStep - currentStepPos);
  
  Serial.print("  [Motor] Moving ");
  Serial.print(stepsToMove);
  Serial.println(" steps.");

  for(int i = 0; i < stepsToMove; i++) {
    digitalWrite(pulPin, HIGH);
    delayMicroseconds(pulseDelay);
    digitalWrite(pulPin, LOW);
    delayMicroseconds(pulseDelay);
  }
  
  currentStepPos = targetStep;
}

// ESP-NOW Callback
#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
#else
void OnDataRecv(const uint8_t *mac, const uint8_t *data, int len) {
#endif
  lastPacketTime = millis();
  memcpy(&incomingData, data, sizeof(incomingData));
  
  float targetYaw = getYaw(incomingData.r, incomingData.i, incomingData.j, incomingData.k);
  
  // Normalize -180...180 to 0...360
  float normalizedYaw = targetYaw + 180.0; 
  long targetStep = (normalizedYaw / 360.0) * stepsPerRev;

  // --- ANTI-JITTER DEADZONE ---
  // Only command a move if the change is greater than 2 steps.
  // This prevents the motor from "buzzing" and overheating due to sensor noise.
  if (abs(targetStep - currentStepPos) > 2) {
    Serial.print(">>> TARGET CHANGE | Yaw: ");
    Serial.print(targetYaw, 1);
    Serial.print("° | New Step: ");
    Serial.println(targetStep);
    
    moveToStep(targetStep);
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10); 
  
  Serial.println("\n--- ESP32 MOTOR RECEIVER (V2 Deadzone) ---");

  pinMode(pulPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(statusLed, OUTPUT);
  digitalWrite(pulPin, LOW);
  digitalWrite(dirPin, LOW);

  WiFi.mode(WIFI_STA);
  ESP_ERROR_CHECK(esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE));

  Serial.print("Local MAC: ");
  Serial.println(WiFi.macAddress());

  if (esp_now_init() != ESP_OK) return;
  
  esp_now_register_recv_cb(OnDataRecv);
  Serial.println("System Ready. Listening for IMU...");
}

void loop() {
  // Keep-alive heartbeat
  static unsigned long lastHeartbeat = 0;
  if (millis() - lastHeartbeat > 2000) {
    lastHeartbeat = millis();
    if (millis() - lastPacketTime > 5000) {
      Serial.println("Waiting for data...");
      digitalWrite(statusLed, !digitalRead(statusLed)); 
    } else {
      digitalWrite(statusLed, HIGH); delay(20); digitalWrite(statusLed, LOW);
    }
  }
}

