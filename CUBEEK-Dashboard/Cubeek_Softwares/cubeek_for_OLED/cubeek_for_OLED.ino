 #include <SPI.h>
#include <Wire.h>

#include <LoRa.h>
#include <Adafruit_BMP085.h>
#include <Adafruit_INA219.h>



// MPU6050 variables:
const int MPU = 0x69; // MPU6050 I2C address for AD0 High
float AccX, AccY, AccZ;
float GyroX, GyroY, GyroZ;
float accAngleX, accAngleY, gyroAngleX, gyroAngleY, gyroAngleZ;
float roll, pitch, yaw;
float AccErrorX, AccErrorY, GyroErrorX, GyroErrorY, GyroErrorZ;
float elapsedTime, currentTime, previousTime;
bool imu_enabled = false;

// BMP085 variables:
Adafruit_BMP085 bmp;
float altitude, pressure, tempr;
bool bmp_enabled = false;

Adafruit_INA219 ina219;
const int temt6000 = A1;

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

  // housekeeping sensor init:
  ina219.begin();
  pinMode(temt6000,INPUT);

  // payload init:
  imu_init();

  bmp_init();
}

void loop()
{
  if(imu_enabled) readIMU();

  if(bmp_enabled) readbmp();

  sendSensorData();
  delay(1000);      // Adjust the delay according to your data sending rate
}

/* Initialization Code Start!*/


void bmp_init()
{
  bmp_enabled = true;
  if (!bmp.begin())
  {
    Serial.println("Could not find a valid BMP085/BMP180 sensor, check wiring!");
    while (1);
  }
}

void imu_init()
{
  imu_enabled = true;
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(0x00);
  Wire.endTransmission(true);

  calculate_IMU_error();
}
/* Initialization Code End!*/

/* IMU Reading Functions Start!*/
void calculate_IMU_error()
{
  int c = 0;

  while (c < 200)
  {
    Wire.beginTransmission(MPU);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    AccX = (Wire.read() << 8 | Wire.read()) / 16384.0;
    AccY = (Wire.read() << 8 | Wire.read()) / 16384.0;
    AccZ = (Wire.read() << 8 | Wire.read()) / 16384.0;
    AccErrorX += atan((AccY) / sqrt(pow((AccX), 2) + pow((AccZ), 2))) * 180 / PI;
    AccErrorY += atan(-1 * (AccX) / sqrt(pow((AccY), 2) + pow((AccZ), 2))) * 180 / PI;
    c++;
  }
  AccErrorX /= 200;
  AccErrorY /= 200;

  c = 0;

  while (c < 200)
  {
    Wire.beginTransmission(MPU);
    Wire.write(0x43);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    GyroX = (Wire.read() << 8 | Wire.read()) / 131.0;
    GyroY = (Wire.read() << 8 | Wire.read()) / 131.0;
    GyroZ = (Wire.read() << 8 | Wire.read()) / 131.0;
    GyroErrorX += GyroX / 131.0;
    GyroErrorY += GyroY / 131.0;
    GyroErrorZ += GyroZ / 131.0;
    c++;
  }
  GyroErrorX /= 200;
  GyroErrorY /= 200;
  GyroErrorZ /= 200;
}

void readIMU() 
{
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true);
  AccX = (Wire.read() << 8 | Wire.read()) / 16384.0;
  AccY = (Wire.read() << 8 | Wire.read()) / 16384.0;
  AccZ = (Wire.read() << 8 | Wire.read()) / 16384.0;

  accAngleX = (atan(AccY / sqrt(pow(AccX, 2) + pow(AccZ, 2))) * 180 / PI) - AccErrorX;
  accAngleY = (atan(-1 * AccX / sqrt(pow(AccY, 2) + pow(AccZ, 2))) * 180 / PI) + AccErrorY;

  previousTime = currentTime;
  currentTime = millis();
  elapsedTime = (currentTime - previousTime) / 1000;

  Wire.beginTransmission(MPU);
  Wire.write(0x43);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true);
  GyroX = (Wire.read() << 8 | Wire.read()) / 131.0;
  GyroY = (Wire.read() << 8 | Wire.read()) / 131.0;
  GyroZ = (Wire.read() << 8 | Wire.read()) / 131.0;

  GyroX += GyroErrorX;
  GyroY += GyroErrorY;
  GyroZ += GyroErrorZ;

  gyroAngleX += GyroX * elapsedTime;
  gyroAngleY += GyroY * elapsedTime;
  yaw += GyroZ * elapsedTime;

  roll = 0.96 * gyroAngleX + 0.04 * accAngleX;
  pitch = 0.96 * gyroAngleY + 0.04 * accAngleY;
}
/* IMU Reading Functions End!*/


/* BMP085 Reading Functions Start!*/
void readbmp()
{
  altitude = bmp.readAltitude();
  pressure = bmp.readPressure() / 100.0F;
  tempr = bmp.readTemperature();
}
/* BMP085 Reading Functions End!*/

void sendSensorData()
{

  float busvoltage = ina219.getBusVoltage_V();

  int brightness = analogRead(temt6000);
  
  String packet = "";
  String data = "";

  if(imu_enabled)
  {
    packet += "YPR:";
    packet += yaw;
    packet += ",";
    packet += pitch;
    packet += ",";
    packet += roll;
    packet += "\n";
//    packet += "I";
  }

  if(bmp_enabled)
  {
    packet += "\nAltitude:";
    packet += altitude;
//    packet += ",";
    packet += " m, ";
    packet += pressure;
    packet += " hPa &";
//packet += ",";
    packet += tempr;
    packet += " C.\n";

  }

 
  data = "\nvoltage: ";
  packet += busvoltage;
  packet += " V &";
//packet += "\n";
//  data += shuntvoltage;
//  data += " mV and";
//  data += " Current: ";
//  data += current_mA;
//  data += " mA";
  packet += " Brightness: ";
  packet += brightness;
  packet += " lm\n";
  
  if(imu_enabled || bmp_enabled) Serial.println(packet);
//  Serial.println(data);
  
  LoRa.beginPacket();
  if(imu_enabled || bmp_enabled) LoRa.print(packet);
//  LoRa.print(data);
  LoRa.endPacket();
}
