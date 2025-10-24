#include "unity.h"
#include <Arduino.h>
#include <TinyGPS++.h>

#define GPS_BAUD 9600
#define TxP 1 
#define RxP 38
TinyGPSPlus gps;
// HardwareSerial gpsSerial(2);

void displayInfo()
{
  Serial.print(F("Location: ")); 
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.println();
}

void test_gps_functionality() {

  Serial2.begin(GPS_BAUD, SERIAL_8N1, RxP, TxP);

  while(true) {
    while(Serial2.available() > 0) {
      char gpsData = Serial2.read();
      Serial.print(gpsData);
      if(gps.encode(gpsData)){
        displayInfo();
      }
    }
  }
  Serial.println(F("End GPS module test."));
  return;
}