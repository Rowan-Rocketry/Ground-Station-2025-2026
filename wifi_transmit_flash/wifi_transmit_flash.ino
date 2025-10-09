#include <WiFi.h>
#include <WiFiUdp.h>

const char* ssid = "ESP32-Network";
const char* password = "12345678";

WiFiUDP udp;
IPAddress receiverIP(192, 168, 4, 1);  // Receiver ESP32 AP IP
#define DEST_PORT 4210

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to AP");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to AP!");
  Serial.print("IP: "); Serial.println(WiFi.localIP());
}

void loop() {
  int n = WiFi.scanNetworks();
  for (int i = 0; i < n; i++) {
    String msg = "SSID: " + WiFi.SSID(i) + " | RSSI: " + String(WiFi.RSSI(i)) + " dBm";

    udp.beginPacket(receiverIP, DEST_PORT);
    udp.print(msg);
    udp.endPacket();

    Serial.println("Sent: " + msg);
    delay(500);  // wait 0.5s between sending networks
  }

  WiFi.scanDelete();  // clean up
  delay(500);         // small delay before next scan
}

