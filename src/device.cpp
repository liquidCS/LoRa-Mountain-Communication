#include "device.h"

MyDevice myDevice;


MyDevice::MyDevice()
{
    UID = ESP.getEfuseMac(); // 48-bit unique ID from ESP32

    if(1) {// Todo: Check if device info exists in persistent storage
        sniprintf(myDevice.ID, DEVICE_ID_MAX_LENGTH, "LoRaNode_%04X", (uint16_t)(myDevice.UID& 0xFFFF));
    }
    else {
        // Load device info from persistent storage (not implemented)
    }

    DEBUG_PRINTF("Device ID: %s\n", myDevice.ID);
    DEBUG_PRINTF("Device UID: %012llX\n", myDevice.UID);
}


