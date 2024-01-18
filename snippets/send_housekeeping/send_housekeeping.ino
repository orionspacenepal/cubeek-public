#include <Adafruit_INA219.h>
#include <LoRa.h>

Adafruit_INA219 ina219;

void setup()
{
    Serial.begin(115200); 
    while (!Serial);
    Serial.println("CUBEEK Initializing...");

    // housekeeping sensor init:
    ina219.begin();

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
    sendHouseKeeping();
    delay(500);      // Adjust the delay according to your data sending rate
}

void sendHouseKeeping()
{
  float shuntvoltage = ina219.getShuntVoltage_mV();
  float busvoltage = ina219.getBusVoltage_V();
  float current_mA = ina219.getCurrent_mA();
  float power_mW = ina219.getPower_mW();
  float loadvoltage = busvoltage + (shuntvoltage / 1000);

  String data = "Bus & shunt voltage: ";
  data += busvoltage;
  data += " V &";
  data += shuntvoltage;
  data += " mV and";
  data += " Current: ";
  data += current_mA;
  data += " mA";
  
  Serial.println(data);
  
  LoRa.beginPacket();
  LoRa.print(data);
  LoRa.endPacket();
}
