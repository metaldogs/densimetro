#include "TRIGGER_WIFI.h"         /*Includes ESP8266WiFi.h and WiFiClientSecure.h, just have these two libraries downloaded before*/
#include "TRIGGER_GOOGLESHEETS.h" /*Library file for Google Sheets, has to be used after Wi-Fi Client Secure declaration, here everything is in Trigger_WIFI.h, so using it after Trigger_WIFI.h*/
#include <Wire.h>
/**********Google Sheets Definations***********/
char column_name_in_sheets[][7] = {
  "X",
  "Y",
  "Z",
  "T",
  "B",
  "N",
};                                                                                                   /*1. The Total no of column depends on how many value you have created in Script of Sheets;2. It has to be in order as per the rows decided in google sheets*/
String Sheets_GAS_ID = "AKfycbwfV8yY5Bj_j5HI88jEEWrQAPE4kb-1v7ef5VwVX-BT590yy09jtU_iAgy63rsnDGmZgg"; /*This is the Sheets GAS ID, you need to look for your sheets id*/
int No_of_Parameters = 6;                                                                            /*Here No_of_Parameters decides how many parameters you want to send it to Google Sheets at once, change it according to your needs*/
/*********************************************/

int16_t Acc_rawX, Acc_rawY, Acc_rawZ, Gyr_rawX, Gyr_rawY, Gyr_rawZ, temp_raw;
double accX, accY, accZ, temp, bateryRead;
int analogPin = A0;
char deviceName[10] = "D02";  // nome do densimetro

void setup() {

  delay(5000);
  Serial.begin(9600);
  while (!Serial)
    ;

  WIFI_Connect("tplink", "#tp-2023");                                         /*Provide you Wi-Fi SSID and password to connect to Wi-Fi*/
  Google_Sheets_Init(column_name_in_sheets, Sheets_GAS_ID, No_of_Parameters); /*Sets the column name for Google Sheets, the GAS ID, and the No of Parameter we want to send*/

  Wire.begin(0, 2);              //begin the wire comunication
  Wire.beginTransmission(0x68);  // 0x68 is the i2c address of MPU6050
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);

  Wire.beginTransmission(0x68);
  Wire.write(0x3B);  //Ask for the 0x3B register- correspond to AcX
  Wire.endTransmission(false);
  Wire.requestFrom(0x68, 14, true);

  Acc_rawX = Wire.read() << 8 | Wire.read();  //each value needs two registres
  Acc_rawY = Wire.read() << 8 | Wire.read();
  Acc_rawZ = Wire.read() << 8 | Wire.read();
  temp_raw = Wire.read() << 8 | Wire.read();
  Gyr_rawX = Wire.read() << 8 | Wire.read();  //Once again we shift and sum
  Gyr_rawY = Wire.read() << 8 | Wire.read();
  Gyr_rawZ = Wire.read() << 8 | Wire.read();


  accX = Acc_rawX / 16384.0;
  accY = Acc_rawY / 16384.0000;
  accZ = Acc_rawZ / 16384.0;

  temp = (temp_raw / 340.0) + 36.53;

  bateryRead = analogRead(analogPin);

  Data_to_Sheets(accX, accY, accZ, temp, bateryRead, deviceName); /*1. This function accepts multiple float parameter, here No_of_Parameters decides how many parameters you want to send to Google Sheets; 2. The values sent should be in order as per the column in Google Sheets*/

  Serial.println();
  //delay(3000);
  ESP.deepSleep(6e7); // MicroSEGUNDOS   2e9 = 33 MINUTOS  4e9 = 66 minutos   6e9 = 100 minutos
}

void loop() {
}