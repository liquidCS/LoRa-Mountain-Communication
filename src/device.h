#ifndef DEVICE_H
#define DEVICE_H

#include <Arduino.h>
#include "config.h"

#define DEVICE_ID_MAX_LENGTH 16

typedef struct gps_time_t
{
    bool valid = false;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
} gps_time_t;

typedef struct gps_location_t
{
    bool valid = false;
    double latitude;
    double longitude;
    double attitude;
    gps_time_t updateTime; // Time of last location update
} gps_location_t;

class Device
{
private:
    uint64_t       UID;                         // Unique ID of the device should be 48-bit from ESP and cannot be changed

public:
    char            ID[DEVICE_ID_MAX_LENGTH];   // Name of the device can be changed
    gps_location_t  location;                   // Last known GPS location with timestamp
    gps_time_t      time;                       // Last time update from GPS 

    Device();                                   // Constructor

    uint64_t GetUID() { return UID; } 
};


extern Device myDevice; // Info of this device {

#endif