#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_BMP085.h>
#include <SPI.h>
#include <RH_RF24.h>

// --- RFM24 Setup ---
RH_RF24 rf24(10, 2); // CS=10, INT=2

// --- Temperature Sensor (DS18B20) Setup ---
#define ONE_WIRE_BUS 6
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature tempSensor(&oneWire);

// --- Pressure Sensor (BMP180) Setup ---
Adafruit_BMP085 bmp;

// --- MPU6050 Setup ---
const int MPU = 0x69; // AD0 High
float AccX, AccY, AccZ;
float GyroX, GyroY, GyroZ;
float accAngleX, accAngleY, gyroAngleX, gyroAngleY, gyroAngleZ;
float roll, pitch, yaw;
float AccErrorX, AccErrorY, GyroErrorX, GyroErrorY, GyroErrorZ;
float elapsedTime, currentTime, previousTime;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  // Initialize RFM24
  if (!rf24.init()) {
    Serial.println("RFM24 init failed");
    while (1);
  }
  
  if (!rf24.setFrequency(434.0)) {
    Serial.println("Failed to set frequency");
    while (1);
  }

  // Set modulation to GFSK 500 bps, 1 kHz deviation
if (!rf24.setModemConfig(RH_RF24::GFSK_Rb0_5Fd1)) {
  Serial.println("Modem config failed");
  while (1);
}

  // Initialize DS18B20
  tempSensor.begin();

  // Initialize BMP180
  if (!bmp.begin()) {
    Serial.println("BMP180 sensor not found!");
    while (1);
  }

  // Initialize MPU6050
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(0x00);
  Wire.endTransmission(true);

  calculate_IMU_error();

  currentTime = millis();
}

void loop() {
  // --- Read DS18B20 Temperature ---
  tempSensor.requestTemperatures();
  float ds_temp = tempSensor.getTempCByIndex(0);

  // --- Read BMP180 ---
  float pressure = bmp.readPressure();
  float bmp_temp = bmp.readTemperature();
  float altitude = bmp.readAltitude();

  // --- Read MPU6050 ---
  readIMU();

  // --- Prepare message to send ---
  String message = "TempDS:" + String(ds_temp, 1) + "C,"
                   "TempBMP:" + String(bmp_temp, 1) + "C,"
                   "Pres:" + String(pressure, 1) + "Pa,"
                   "Alt:" + String(altitude, 1) + "m,"
                   "Roll:" + String(roll, 1) + ","
                   "Pitch:" + String(pitch, 1) + ","
                   "Yaw:" + String(yaw, 1);

  Serial.println("Sending: " + message);

  // Send via RF24
  rf24.send((uint8_t *)message.c_str(), message.length());
  rf24.waitPacketSent();

  delay(2000);
}

void calculate_IMU_error() {
  int c = 0;
  while (c < 200) {
    Wire.beginTransmission(MPU);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    AccX = (Wire.read() << 8 | Wire.read()) / 16384.0;
    AccY = (Wire.read() << 8 | Wire.read()) / 16384.0;
    AccZ = (Wire.read() << 8 | Wire.read()) / 16384.0;
    AccErrorX += atan(AccY / sqrt(pow(AccX, 2) + pow(AccZ, 2))) * 180 / PI;
    AccErrorY += atan(-AccX / sqrt(pow(AccY, 2) + pow(AccZ, 2))) * 180 / PI;
    c++;
  }
  AccErrorX /= 200;
  AccErrorY /= 200;

  c = 0;
  while (c < 200) {
    Wire.beginTransmission(MPU);
    Wire.write(0x43);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    GyroX = (Wire.read() << 8 | Wire.read()) / 131.0;
    GyroY = (Wire.read() << 8 | Wire.read()) / 131.0;
    GyroZ = (Wire.read() << 8 | Wire.read()) / 131.0;
    GyroErrorX += GyroX;
    GyroErrorY += GyroY;
    GyroErrorZ += GyroZ;
    c++;
  }
  GyroErrorX /= 200;
  GyroErrorY /= 200;
  GyroErrorZ /= 200;
}

void readIMU() {
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true);
  AccX = (Wire.read() << 8 | Wire.read()) / 16384.0;
  AccY = (Wire.read() << 8 | Wire.read()) / 16384.0;
  AccZ = (Wire.read() << 8 | Wire.read()) / 16384.0;

  accAngleX = (atan(AccY / sqrt(pow(AccX, 2) + pow(AccZ, 2))) * 180 / PI) - AccErrorX;
  accAngleY = (atan(-AccX / sqrt(pow(AccY, 2) + pow(AccZ, 2))) * 180 / PI) + AccErrorY;

  previousTime = currentTime;
  currentTime = millis();
  elapsedTime = (currentTime - previousTime) / 1000.0;

  Wire.beginTransmission(MPU);
  Wire.write(0x43);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true);
  GyroX = (Wire.read() << 8 | Wire.read()) / 131.0;
  GyroY = (Wire.read() << 8 | Wire.read()) / 131.0;
  GyroZ = (Wire.read() << 8 | Wire.read()) / 131.0;

  GyroX -= GyroErrorX;
  GyroY -= GyroErrorY;
  GyroZ -= GyroErrorZ;

  gyroAngleX += GyroX * elapsedTime;
  gyroAngleY += GyroY * elapsedTime;
  yaw += GyroZ * elapsedTime;

  roll = 0.96 * gyroAngleX + 0.04 * accAngleX;
  pitch = 0.96 * gyroAngleY + 0.04 * accAngleY;
}
