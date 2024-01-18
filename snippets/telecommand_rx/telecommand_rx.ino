// cubeek side
#include <SPI.h>
#include <LoRa.h>

#define Led_Pin 13
const int maxPacketSize = 10;
char SAT[] = "CUBE1";
char receivedData[maxPacketSize];

void setup() {
    Serial.begin(115200);
    while (!Serial);

    LoRa.setPins(8, 9, 2);
    Serial.println("CUBESAT Telecommand TEST");
    Serial.println("CUBEEK Side");

    if (!LoRa.begin(433E6)) {
        Serial.println("Starting LoRa failed!");
        while (1);
    }

    pinMode(Led_Pin,OUTPUT);
}
void loop()
{
    while(!LoRa.parsePacket()){}

    Serial.print("Received packet: ");
    int bytesRead = LoRa.readBytes(receivedData, maxPacketSize);
    receivedData[bytesRead] = '\0';
    Serial.println(receivedData);

    if(strcmp(receivedData, SAT)== 0){
        
        digitalWrite(Led_Pin,HIGH);
        delay(500);
        digitalWrite(Led_Pin,LOW);
        delay(500);

        String data = "CUBEEK";
        LoRa.beginPacket();
        LoRa.print(data);
        LoRa.endPacket();
    }
}