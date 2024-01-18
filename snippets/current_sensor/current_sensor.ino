//CUBESAT EPS Housekeeping data
//Testing of EPS subsystem
//Important for proper functioning, Mission status and operation of 
//CUBESAT itslef.

#include <Adafruit_INA219.h>
Adafruit_INA219 ina219;
void setup() {
  Serial.begin(115200);

  Serial.println("Measuring voltage and current with INA219 ...");
  ina219.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  float shuntvoltage = ina219.getShuntVoltage_mV();
  float busvoltage = ina219.getBusVoltage_V();
  float current_mA = ina219.getCurrent_mA();
  float power_mW = ina219.getPower_mW();
  float loadvoltage = busvoltage + (shuntvoltage / 1000);

  Serial.print("Bus Voltage:   "); Serial.print(busvoltage); Serial.println(" V");
  Serial.print("Shunt Voltage: "); Serial.print(shuntvoltage); Serial.println(" mV");
  Serial.print("Load Voltage:  "); Serial.print(loadvoltage); Serial.println(" V");
  Serial.print("Current:       "); Serial.print(current_mA); Serial.println(" mA");
  Serial.print("Power:         "); Serial.print(power_mW); Serial.println(" mW");
  Serial.println("");
}
