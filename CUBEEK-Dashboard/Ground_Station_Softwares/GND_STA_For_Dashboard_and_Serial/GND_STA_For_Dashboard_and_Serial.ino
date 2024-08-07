#include <Wire.h>
#include <SPI.h>
#include <LoRa.h>
const int maxPacketSize = 256; // Maximum expected packet size, adjust as needed
void setup() {
 Serial.begin(9600);
 LoRa.setPins(8, 9, 2);
//  Serial.println("LoRa Receiver");

  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

}

void loop() {
  int packetSize = LoRa.parsePacket();
if (packetSize>0) {
    char receivedData[maxPacketSize];
    int bytesRead = LoRa.readBytes(receivedData, maxPacketSize);
    receivedData[bytesRead] = '\0';
    Serial.println(receivedData);
}}
