#include <WiFi.h>
#include <WiFiUdp.h>

const char* ssid = "ESP32-Network";
const char* password = "12345678";

WiFiUDP udp;
#define LISTEN_PORT 4210

void setup() {
  Serial.begin(115200);

  // Start AP
  WiFi.softAP(ssid, password);
  Serial.println("AP started");
  Serial.print("AP IP: "); Serial.println(WiFi.softAPIP());

  udp.begin(LISTEN_PORT);
  Serial.println("Listening for UDP packets...");
}

void loop() {
  int packetSize = udp.parsePacket();
  if (packetSize) {
    char incoming[255];
    int len = udp.read(incoming, sizeof(incoming) - 1);
    if (len > 0) incoming[len] = '\0';
    Serial.println(incoming);
  }
}

