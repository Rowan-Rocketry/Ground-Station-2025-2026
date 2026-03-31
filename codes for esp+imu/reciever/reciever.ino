#include <esp_now.h>
#include <WiFi.h>

// 1. MATCH THE SENDER'S DATA STRUCTURE EXACTLY
typedef struct struct_message {
  float r; float i; float j; float k;
} struct_message;

struct_message incomingData;

// 2. UNIVERSAL CALLBACK (Works with ESP32 Core 2.x and 3.x)
// This function runs every time a packet arrives
#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
void OnDataRecv(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len) {
#else
void OnDataRecv(const uint8_t *mac, const uint8_t *data, int len) {
#endif
  // Copy the raw data into our struct
  memcpy(&incomingData, data, sizeof(incomingData));

  // Print the results to Serial Monitor
  Serial.println("--- New Data Received ---");
  Serial.print("Rotation Vector [R I J K]: ");
  Serial.print(incomingData.r, 4); Serial.print(" ");
  Serial.print(incomingData.i, 4); Serial.print(" ");
  Serial.print(incomingData.j, 4); Serial.print(" ");
  Serial.println(incomingData.k, 4);
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  Serial.print("Receiver MAC Address: ");
  Serial.println(WiFi.macAddress());

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Register the callback function
  esp_now_register_recv_cb(OnDataRecv);
  
  Serial.println("Waiting for data from BNO08x sender...");
}

void loop() {
  // Loop stays empty; the callback handles the printing
}
