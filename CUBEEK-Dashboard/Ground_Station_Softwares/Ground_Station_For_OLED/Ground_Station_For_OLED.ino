#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <LoRa.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const int maxPacketSize = 256; // Maximum expected packet size, adjust as needed

void setup() {
  Serial.begin(115200);
  while (!Serial);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(5, 20);
  // Display static text
  display.println("GROUND STATION");
  display.display(); 
  delay(1000);
  
  LoRa.setPins(8, 9, 2);
  Serial.println("LoRa Receiver");

  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}

void loop() {
  int packetSize = LoRa.parsePacket();
display.clearDisplay();

//  display.setTextSize(1);
//  display.setTextColor(WHITE);
//  display.setCursor(0, 10);
//  // Display static text
//  display.println("Packet size");
//  display.println(packetSize);
//  display.display(); 
  if (packetSize>0) {
    Serial.print("Received Data From Satellite:\n");

    char receivedData[maxPacketSize];
    int bytesRead = LoRa.readBytes(receivedData, maxPacketSize);

    receivedData[bytesRead] = '\0';
//    display.clearDisplay();
//
//  display.setTextSize(1);
//  display.setTextColor(WHITE);
//  display.setCursor(0, 10);
//  // Display static text
//  display.println("Receiving data");
////  display.println(receivedData);
//  display.display(); 
    Serial.print(receivedData);
//
//    Serial.println();
  }

//  else{
    display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  // Display static text
  display.println("No data received!!");
  display.display(); 

//  }
      delay(200);
}
