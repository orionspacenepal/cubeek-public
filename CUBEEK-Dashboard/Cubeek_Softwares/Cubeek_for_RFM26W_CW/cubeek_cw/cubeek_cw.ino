#include <SPI.h>
#include <Wire.h>
#include <RH_RF24.h>
#include <Adafruit_INA219.h>
#include <MPU6050.h>
#include <Adafruit_BMP085.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <OneWire.h>
#include <DallasTemperature.h>

/* ================= CONFIG ================= */
#define UNIT_LENGTH  60
#define BEACON_GAP   5000

/* ================= PINS ================= */
#define GPIO0_PIN 7
#define SDN_PIN   9
#define TEMT_PIN  A1
#define GPS_RX    4
#define GPS_TX    3
#define ONE_WIRE_BUS 6

/* ================= OBJECTS ================= */
RH_RF24 rf24;
Adafruit_INA219 ina219(0x40);
MPU6050 mpu;
Adafruit_BMP085 bmp;
TinyGPSPlus gps;
SoftwareSerial gpsSerial(GPS_RX, GPS_TX);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature dallas(&oneWire);

/* ================= FLAGS ================= */
bool ina_ok=false, mpu_ok=false, bmp_ok=false, gps_ok=false, ds_ok=false;

/* ================= DATA ================= */
int vbatt=0, temp_ext=0, pressure_mb=0, light_lvl=0;
int spin=0, shock=0;
long peak_rot=0, peak_acc=0;

/* ================= RADIO CONFIG ================= */
RH_RF24::ModemConfig OOKAsync = {
  0x89, 0x00, 0xc3, 0x50, 0x01, 0x00, 0x00, 0x00, 0x00, 0x34,
  0x10, 0x00, 0x3f, 0x08, 0x31, 0x27, 0x04, 0x10, 0x02, 0x12,
  0x00, 0x2c, 0x03, 0xf9, 0x62, 0x11, 0x0e, 0x0e, 0x00, 0x02,
  0xff, 0xff, 0x00, 0x27, 0x00, 0x00, 0x07, 0xff, 0x40, 0xcc,
  0xa1, 0x30, 0xa0, 0x21, 0xd1, 0xb9, 0xc9, 0xea, 0x05, 0x12,
  0x11, 0x0a, 0x04, 0x15, 0xfc, 0x03, 0x00, 0xcc, 0xa1, 0x30,
  0xa0, 0x21, 0xd1, 0xb9, 0xc9, 0xea, 0x05, 0x12, 0x11, 0x0a,
  0x04, 0x15, 0xfc, 0x03, 0x00, 0x3f, 0x2c, 0x0e, 0x04, 0x0c,
  0x73
};

/* ================= MORSE ================= */
const char* morse(char c){
  switch(toupper(c)){
    case 'A':return ".-"; case 'B':return "-...";
    case 'C':return "-.-."; case 'D':return "-..";
    case 'E':return "."; case 'F':return "..-.";
    case 'G':return "--."; case 'H':return "....";
    case 'I':return ".."; case 'J':return ".---";
    case 'K':return "-.-"; case 'L':return ".-..";
    case 'M':return "--"; case 'N':return "-.";
    case 'O':return "---"; case 'P':return ".--.";
    case 'Q':return "--.-"; case 'R':return ".-.";
    case 'S':return "..."; case 'T':return "-";
    case 'U':return "..-"; case 'V':return "...-";
    case 'W':return ".--"; case 'X':return "-..-";
    case 'Y':return "-.--"; case 'Z':return "--..";
    case '0':return "-----"; case '1':return ".----";
    case '2':return "..---"; case '3':return "...--";
    case '4':return "....-"; case '5':return ".....";
    case '6':return "-...."; case '7':return "--...";
    case '8':return "---.."; case '9':return "----.";
    default:return "";
  }
}

/* ================= CRC-8 ================= */
uint8_t crc8(const String &s){
  uint8_t crc=0;
  for(uint16_t i=0;i<s.length();i++){
    crc ^= s[i];
    for(uint8_t j=0;j<8;j++)
      crc = (crc & 0x80) ? (crc<<1)^0x07 : (crc<<1);
  }
  return crc;
}

/* ================= MPU AUTO-DETECT ================= */
bool initMPU(){
  mpu = MPU6050(0x68);
  mpu.initialize();
  if(mpu.testConnection()){
    Serial.println(F("MPU @0x68"));
    return true;
  }
  mpu = MPU6050(0x69);
  mpu.initialize();
  if(mpu.testConnection()){
    Serial.println(F("MPU @0x69"));
    return true;
  }
  Serial.println(F("MPU NOT FOUND"));
  return false;
}

/* ================= CW SEND ================= */
void sendChar(char c){
  if(c==' '){ delay(UNIT_LENGTH*4); return; }
  const char* code=morse(c);
  Serial.print(code); Serial.print(" ");
  for(uint8_t i=0;code[i];i++){
    digitalWrite(GPIO0_PIN,HIGH);
    delay(code[i]=='.'?UNIT_LENGTH:UNIT_LENGTH*3);
    digitalWrite(GPIO0_PIN,LOW);
    delay(UNIT_LENGTH);
  }
  delay(UNIT_LENGTH*2);
}

/* ================= SETUP ================= */
void setup(){
  Serial.begin(9600);
  Wire.begin();
  Wire.setClock(50000);
  Wire.setWireTimeout(3000,true);

  pinMode(GPIO0_PIN,OUTPUT);
  pinMode(SDN_PIN,OUTPUT);
  digitalWrite(SDN_PIN,LOW);

  gpsSerial.begin(9600);

  ina_ok = ina219.begin();
  bmp_ok = bmp.begin();
  mpu_ok = initMPU();

  dallas.begin();
  dallas.requestTemperatures();
  ds_ok = dallas.getTempCByIndex(0)!=DEVICE_DISCONNECTED_C;

  rf24.init();
  rf24.setFrequency(436.0);
  rf24.setModemRegisters(&OOKAsync);
  rf24.setModeTx();

  Serial.println(F("CUBEEK CW READY"));
}

/* ================= LOOP ================= */
void loop(){
  while(gpsSerial.available())
    gps.encode(gpsSerial.read());

  if(mpu_ok){
    int16_t ax,ay,az,gx,gy,gz;
    mpu.getMotion6(&ax,&ay,&az,&gx,&gy,&gz);
    peak_rot=max(peak_rot,(long)abs(gx)+abs(gy)+abs(gz));
    peak_acc=max(peak_acc,(long)abs(ax)+abs(ay)+abs(az));
  }

  static unsigned long last=0;
  if(millis()-last<BEACON_GAP) return;
  last=millis();

  vbatt=(int)(ina219.getBusVoltage_V()*10);
  dallas.requestTemperatures();
  temp_ext=(int)dallas.getTempCByIndex(0);
  pressure_mb=bmp.readPressure()/100;
  light_lvl=analogRead(TEMT_PIN)/100;

  spin=map(constrain(peak_rot,0,15000),0,15000,0,9);
  shock=map(constrain(peak_acc,0,45000),0,45000,0,9);
  peak_rot=peak_acc=0;

  gps_ok=gps.location.isValid();

  String msg="CUBEEK V"+String(vbatt)+" E"+String(temp_ext)+" P"+String(pressure_mb);
  msg+=" L"+String(light_lvl)+" R"+String(spin)+" A"+String(shock);
  if(gps_ok){
    msg+=" LT"+String(abs((int)(gps.location.lat()*10)));
    msg+=" LN"+String(abs((int)(gps.location.lng()*10)));
  } else msg+=" NOGPS";

  uint8_t crc=crc8(msg);
  msg+=" C"+String(crc,HEX);

  Serial.println("\nCW:");
  for(char c:msg) sendChar(c);

  Serial.println("\nTEXT:");
  Serial.println(msg);
}