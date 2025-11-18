#ifndef GPS_H
#define GPS_H

#include <TinyGPS++.h>
#include <Arduino.h>
#include "config.h"
#include "device.h"

#define GPS_BAUD 9600 // Baud rate for GPS module
#define GPS_TX 1 // PinTx for GPS module on ESP32 
#define GPS_RX 38 // PinRx for GPS module on EPS32




class GPS 
{
private:
    TinyGPSPlus gpsplus;

public:
    GPS(); // Constructor
    bool GPSerialUpdate();

};

void TaskGPSUpdate();

extern GPS gps;

#endif