#include <esp_now.h>
#include <WiFi.h>
#include <Adafruit_BNO08x.h>
#include <Wire.h>

#define BNO08X_RESET 18 
#define I2C_SDA 21
#define I2C_SCL 22

uint8_t broadcastAddress[] = {0x14, 0x08, 0x08, 0xA6, 0x40, 0xF4}; // 14:08:08:a6:40:f4

Adafruit_BNO08x bno08x;
sh2_SensorValue_t sensorValue;

typedef struct struct_message {
  float r; float i; float j; float k;
} struct_message;

struct_message myData;
esp_now_peer_info_t peerInfo;

void setup() {
  Serial.begin(115200);
  delay(1000); // Give serial monitor time to open
  Serial.println("\n--- BNO08x + ESP-NOW Debug Start ---");

  WiFi.mode(WIFI_STA);

  // 1. MANUALLY RESET THE SENSOR
  Serial.println("Action: Toggling Reset Pin (GPIO 18)...");
  pinMode(BNO08X_RESET, OUTPUT);
  digitalWrite(BNO08X_RESET, LOW);
  delay(100);
  digitalWrite(BNO08X_RESET, HIGH);
  delay(500); // Give sensor time to boot after reset

  // 2. CONFIGURE I2C
  Serial.println("Action: Setting up I2C at 100kHz...");
  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.setClock(100000); 
  Wire.setTimeOut(1000);

  // 3. TRY INITIALIZATION
  Serial.println("Action: Attempting bno08x.begin_I2C(0x4A)...");
  if (!bno08x.begin_I2C(0x4A, &Wire, BNO08X_RESET)) { 
    Serial.println("CRITICAL FAILURE: Sensor found by scanner but rejected by library.");
    Serial.println("Check: Is the DI pin grounded? Is the power stable?");
    while (1) {
      Serial.print("."); // Blink dots so you know it's stuck here
      delay(500);
    }
  }
  
  Serial.println("Success: BNO08x Initialized!");
  
  if (!bno08x.enableReport(SH2_ROTATION_VECTOR)) {
    Serial.println("Failure: Could not enable rotation vector report");
  }

  // 4. ESP-NOW
  Serial.println("Action: Initializing ESP-NOW...");
  if (esp_now_init() != ESP_OK) {
    Serial.println("Failure: ESP-NOW Init Error");
    return;
  }

  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failure: Failed to add peer");
    return;
  }
  
  Serial.println("--- Setup Complete: System Running! ---");
}

void loop() {
  if (bno08x.getSensorEvent(&sensorValue)) {
    if (sensorValue.sensorId == SH2_ROTATION_VECTOR) {
      Serial.println("Data: Got Rotation Vector, sending via ESP-NOW...");
      myData.r = sensorValue.un.rotationVector.real;
      myData.i = sensorValue.un.rotationVector.i;
      myData.j = sensorValue.un.rotationVector.j;
      myData.k = sensorValue.un.rotationVector.k;

      esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
    }
  }
  delay(10); 
}

