#include "device.hpp"

MyDevice myDevice; // store current device info
OtherDevice deviceList[DEVICE_LIST_MAX_SIZE];  // store other devices


MyDevice::MyDevice()
{
    UID = ESP.getEfuseMac(); // 48-bit unique ID from ESP32

    if(1) {// Todo: Check if device info exists in persistent storage
        snprintf(myDevice.ID, DEVICE_ID_MAX_LENGTH, "LoRaNode_%08X", (uint32_t)(myDevice.UID& 0xFFFFFFFF));
    }
    else {
        // Load device info from persistent storage (not implemented)
    }

    DEBUG_PRINTF("Device ID: %s\n", myDevice.ID);
    DEBUG_PRINTF("Device UID: %08X\n", myDevice.UID);
}

uint16_t CreateNewDevice(const uint32_t UID)
{
    for (uint16_t i = 0; i < DEVICE_LIST_MAX_SIZE; i++) {
        if (deviceList[i].GetStatus() == DEVICE_STATUS_UNKNOWN) { 
            deviceList[i] = OtherDevice(UID);

            DEBUG_PRINTF("Created new device with ID: %08X\n", UID);
            return i;
        }
    }
    DEBUG_PRINTLN("Device list is full, cannot create new device.");
    return DEVICE_LIST_MAX_SIZE; // Indicate failure
}

uint16_t FindDeviceIndexByUID(const uint32_t UID)
{
    for (int i = 0; i < DEVICE_LIST_MAX_SIZE; i++) {
        if (deviceList[i].GetUID() == UID && deviceList[i].GetStatus() != DEVICE_STATUS_UNKNOWN) {
            return i;
        }
    }
    return DEVICE_LIST_MAX_SIZE; // Not found
}

bool CheckDeviceExists(const uint32_t UID)
{
    return FindDeviceIndexByUID(UID) < DEVICE_LIST_MAX_SIZE;
}

void UpdateDeviceLocation(const uint32_t UID, double lat, double lon, double att)
{
    uint16_t index = FindDeviceIndexByUID(UID);
    if (index < DEVICE_LIST_MAX_SIZE) {
        deviceList[index].location.valid = true;
        deviceList[index].location.latitude = lat;
        deviceList[index].location.longitude = lon;
        deviceList[index].location.attitude = att;

        DEBUG_PRINTF("Updated device (UID: %08X) location to Lat: %.6f, Lon: %.6f, Att: %.2f\n", 
                      UID, lat, lon, att);
    } else {
        DEBUG_PRINTF("Device with UID: %08X not found for location update.\n", UID);
    }
}