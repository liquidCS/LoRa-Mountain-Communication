#ifndef DEVICE_H
#define DEVICE_H

#include <Arduino.h>
#include "config.hpp"


enum DeviceStatus {
    DEVICE_STATUS_UNKNOWN,
    DEVICE_STATUS_ACTIVE,
    DEVICE_STATUS_INACTIVE
};

typedef struct gps_time_t
{
    SemaphoreHandle_t xMutex;
    bool valid = false;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
} gps_time_t;

typedef struct gps_location_t
{
    SemaphoreHandle_t xMutex;
    bool valid = false;
    double latitude;
    double longitude;
    double attitude;
    gps_time_t updateTime; // Time of last location update. Mutexed gps_time_t not used 
} gps_location_t;

class Device
{
protected:
    uint32_t       UID;                         // Unique ID of the device should be 48-bit from ESP and cannot be changed
    char           ID[DEVICE_ID_MAX_LENGTH];    // Name of the device can be changed
    DeviceStatus status;                        // Status of the device

public:
    gps_location_t  location;                   // Last known GPS location with timestamp

    Device() {
        location.xMutex = xSemaphoreCreateBinary();
        assert(location.xMutex != NULL); 
        xSemaphoreGive(location.xMutex);
    } 

    uint32_t GetUID() { return UID; } 
    const char *GetID() { return ID; }
    void SetID(const char* newID, uint8_t n) { strncpy(ID, newID, n); }
    DeviceStatus GetStatus() { return status; }
    void SetStatus(DeviceStatus newStatus) { status = newStatus; }
};


class OtherDevice : public Device
{
protected:
    DeviceStatus status;     // Status of the other device (e.g., active, inactive)

public:
    OtherDevice() : Device(), status(DEVICE_STATUS_UNKNOWN) { UID = 0; }                                                                  // Default constructor
    OtherDevice(uint32_t _UID, DeviceStatus _status = DEVICE_STATUS_UNKNOWN) : Device(), status(_status) { UID = _UID; }                  // Constructor for new device
};


class MyDevice: public Device
{
private:

public:
    gps_time_t      time;                       // Current time from GPS 

    MyDevice();                                 // Constructor
};

uint16_t CreateNewDevice(const uint32_t UID);
uint16_t FindDeviceIndexByUID(const uint32_t UID);
bool CheckDeviceExists(const uint32_t UID);
void UpdateDeviceLocation(const uint32_t UID, double lat, double lon, double att);
void UpdateDeviceID(const uint32_t UID, const char* newID);

extern MyDevice myDevice; // Info of this device 

#endif