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


class Device
{
private:
    uint64_t       UID; // Unique ID of the device

public:
    char            ID[DEVICE_ID_MAX_LENGTH];  // Name of the device 
    gps_location_t  location; // Last known GPS location 
    gps_time_t      time;         // Last known GPS time

    Device(); // Constructor

    uint64_t GetUID() { return UID; } 
};


extern Device myDevice; // Info of this device 

#endif