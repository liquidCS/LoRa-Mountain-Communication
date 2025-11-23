#ifndef DEVICE_H
#define DEVICE_H

#include <Arduino.h>
#include "config.hpp"

#define DEVICE_ID_MAX_LENGTH 16


enum DeviceStatus {
    DEVICE_STATUS_UNKNOWN,
    DEVICE_STATUS_ACTIVE,
    DEVICE_STATUS_INACTIVE
};

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
protected:
    uint64_t       UID;                         // Unique ID of the device should be 48-bit from ESP and cannot be changed

public:
    char            ID[DEVICE_ID_MAX_LENGTH];   // Name of the device can be changed
    gps_location_t  location;                   // Last known GPS location with timestamp


    uint64_t GetUID() { return UID; } 
    char *GetID() { return ID; }
    void SetID(const char* newID) { strncpy(ID, newID, DEVICE_ID_MAX_LENGTH); ID[DEVICE_ID_MAX_LENGTH - 1] = '\0'; }
};


class OtherDevice : public Device
{
protected:
    DeviceStatus status;     // Status of the other device (e.g., active, inactive)

public:
    OtherDevice(uint64_t _UID) : status(DEVICE_STATUS_UNKNOWN) { UID = _UID; }                  // Constructor
};


class MyDevice: public Device
{
private:

public:
    gps_time_t      time;                       // Current time from GPS 

    MyDevice();                                 // Constructor
};


extern MyDevice myDevice; // Info of this device 

#endif