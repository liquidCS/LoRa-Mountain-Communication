#ifndef DEVICE_H
#define DEVICE_H

#include <Arduino.h>
#include "config.h"

#define DEVICE_ID_MAX_LENGTH 16

typedef struct gps_location_t
{
    bool valid = false;
    double latitude;
    double longitude;
    double attitude;
} gps_location_t;

typedef struct gps_time_t
{
    bool valid = false;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
} gps_time_t;

typedef struct device_info_t
{
    char deviceID[DEVICE_ID_MAX_LENGTH];  // Name of the device 
    uint64_t deviceUID; // Unique ID of the device

    gps_location_t lastKnownLocation; // Last known GPS location 
    gps_time_t lastKnownTime;         // Last known GPS time
    float lastKnowSignalStrength; // Last known signal strength

} device_info_t;



class Device
{
public:
    device_info_t info; // Device information

    Device(); // Constructor
};


extern device_info_t myDevice; // Info of this device 

#endif