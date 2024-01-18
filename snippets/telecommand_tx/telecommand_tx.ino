// ground station
#include <SPI.h>
#include <LoRa.h>

#define Led_Pin 13
bool led_state;

const int maxPacketSize = 10;
char GST[] = "CUBEEK";
char receivedData[maxPacketSize];

void setup() {
    Serial.begin(115200);
    while (!Serial);

    LoRa.setPins(8, 9, 2);
    Serial.println("CUBESAT Telecommand TEST");
    Serial.println("Ground Station");

    if (!LoRa.begin(433E6)) {
        Serial.println("Starting LoRa failed!");
        while (1);
    }

    pinMode(Led_Pin,OUTPUT);
}
void loop()
{
    Serial.println("Press 's' to send command:");
    while(!Serial.available()){}

    char read = Serial.read();
    if(read == 'S' || read == 's')
    {
        String data = "CUBE1";
        LoRa.beginPacket();
        LoRa.print(data);
        LoRa.endPacket();

        while(!LoRa.parsePacket()){}

        Serial.print("Received packet: ");
        int bytesRead = LoRa.readBytes(receivedData, maxPacketSize);
        receivedData[bytesRead] = '\0';
        Serial.println(receivedData);

        if(strcmp(receivedData, GST) == 0) 
        {
            digitalWrite(Led_Pin,HIGH);
            delay(500);
            digitalWrite(Led_Pin,LOW);
            delay(500);
        }
    }
}