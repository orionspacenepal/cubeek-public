#include <TinyGPS++.h>
#include <SoftwareSerial.h>

SoftwareSerial GPS_SoftSerial(3, 4);
double lat_val, lng_val;
TinyGPSPlus gps;

void setup()
{
    Serial.begin(115200);
    GPS_SoftSerial.begin(9600); 
    while (!Serial);
    Serial.println("Reading Data from GPS!");
}
void loop()
{
    readgps(); 
    
    Serial.print("Latitude: ");
    Serial.print(lat_val);
    Serial.print("\tLongitude: ");
    Serial.println(lng_val);

    delay(100);
}

void readgps(){
    smartDelay(1000);
    bool loc_valid;
    loc_valid = gps.location.isValid();
    if(loc_valid)
    {
        lat_val = gps.location.lat();
        lng_val = gps.location.lng();
    }
    else{
        lat_val = -1.00;
        lng_val = -1.00;
    }
}

void smartDelay(unsigned long ms)
{
    unsigned long start = millis();
    do 
    {
      while (GPS_SoftSerial.available()) 
          gps.encode(GPS_SoftSerial.read());

    } while (millis() - start < ms);
}