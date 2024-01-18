#include <LoRa.h>

String data = "Hello from CUBEEK!"; // data to be  send

void setup()
{
    Serial.begin(115200); 
    while (!Serial);
    Serial.println("CUBEEK Initializing...");

    // communication init:
    LoRa.setPins(8, 9, 2);
    if (!LoRa.begin(433E6))
    {
        Serial.println("Starting LoRa failed!");
        while (1);
    }
}

void loop()
{
    LoRa.beginPacket();
    LoRa.print(data);
    LoRa.endPacket();
    delay(1000);      // Adjust the delay according to your data sending rate
}