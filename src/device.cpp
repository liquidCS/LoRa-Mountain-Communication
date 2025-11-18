#include "device.h"

device_info_t myDevice;


Device::Device() 
{
    myDevice.deviceUID= ESP.getEfuseMac(); // 48-bit unique ID from ESP32

    if(1) {// Todo: Check if device info exists in persistent storage
        sniprintf(myDevice.deviceID, DEVICE_ID_MAX_LENGTH, "LoRaNode_%04X", (uint16_t)(myDevice.deviceUID & 0xFFFF));
    }
    else {
        // Load device info from persistent storage (not implemented)
    }

    DEBUG_PRINTF("Device ID: %s\n", myDevice.deviceID);
    DEBUG_PRINTF("Device UID: %012llX\n", myDevice.deviceUID);
}


