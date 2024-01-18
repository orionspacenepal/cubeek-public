#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;

void setup()
{
    Serial.begin(115200);
    if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085/BMP180 sensor, check wiring!");
    while (1);
    }

}
void loop()
{
    double Altitude = bmp.readAltitude();
    double Pressure = bmp.readPressure();
    double Temperature = bmp.readTemperature();

    Serial.print("Altitude: ");
    Serial.print(Altitude);
    Serial.print("m");
    Serial.print("\tPressure: ");
    Serial.print(Pressure);
    Serial.print("hPa");
    Serial.print("\tTemperature: ");
    Serial.print(Temperature);
    Serial.println("*C");
}