
#define BLYNK_PRINT Serial


#include <SoftwareSerial.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include<Wire.h>
SoftwareSerial SwSerial(8,9); // RX, TX
    
#include <BlynkSimpleSerialBLE.h>

OneWire oneWire(7);
DallasTemperature sensors(&oneWire);
// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "1f9648e3ec9a4faea4864d54401c9d0d";
float  temp;

SoftwareSerial SerialBLE(8, 9); // RX, TX

const int MPU_addr=0x68;  // I2C address of the MPU-6050
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;

  int stepCounter =0;
  int16_t sm = 0 ;
  int16_t x,y,z;
  int16_t s;
  int count = 0;
  float smMax =0;
  int countAVM = 0;
  float avm = 0;
  float averSm =0;
void setup()
{
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  Serial.begin(9600);
  SerialBLE.begin(9600);
  Blynk.begin(SerialBLE, auth);
  sensors.begin();
}

void loop()
{
  Blynk.run();
  sensors.requestTemperatures();
  temp = sensors.getTempCByIndex(0);
  Serial.println(temp);
  SwSerial.println(temp);
  Blynk.virtualWrite(V1,temp);

  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);  // request a total of 14 registers
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  x = abs(AcX);
  y = abs(AcY);
  z = abs(AcZ);
  s = x+y+z;
  
  if(smMax < s){
  smMax = s;
  }
   // калибровка происходит в двух местах (1) и (2)
  if(count<2){ // (1)
    averSm = averSm + smMax;count++;
    Serial.print("averSM: ");Serial.print(averSm);Serial.print("\n");
  }
  else {
    Serial.print("count: ");Serial.print(count);Serial.print("\n");
    avm = averSm/count;
    averSm =0;
    count = 0;
  }
   Serial.print("avm: ");Serial.print(avm);Serial.print("\n");
  if(avm>s+10000)stepCounter++; // (2)
  
   Serial.print("s: ");Serial.print(s);Serial.print("\n");
  Serial.print("counter step: ");Serial.print(stepCounter);Serial.print("\n");
 // Serial.print("-----------------------------\n");
  
 
  Blynk.virtualWrite(V2,stepCounter);
  delay(333);
}
