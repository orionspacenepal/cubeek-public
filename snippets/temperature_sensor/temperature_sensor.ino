#include <OneWire.h>
#include <DallasTemperature.h>
// Data wire is connected to the Arduino digital pin 4
#define ONE_WIRE_BUS 6

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    sensors.begin();
}

void loop() {
    // put your main code here, to run repeatedly:
    sensors.requestTemperatures();
    float  sensor_temp = sensors.getTempCByIndex(0);
    Serial.print("Temperature Value:");
    Serial.print(sensor_temp);
    Serial.println("*C");
}
