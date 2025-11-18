#include "gps.h"

#define GPS_BAUD 9600
#define GPS_TX 1 // PinTx for GPS module on ESP32 
#define GPS_RX 38 // PinRx for GPS module on EPS32
#define GPS_UPDATE_INTERVAL_MS 10000 // Update GPS data every 10 second

GPS gps;

GPS::GPS() 
{
    Serial2.begin(GPS_BAUD, SERIAL_8N1, GPS_RX, GPS_TX);
}

bool GPS::GPSerialUpdate()
{
    if(Serial2.available() > 0)
    {
        gpsplus.encode(Serial2.read());

        if(gpsplus.location.isUpdated())
        {
            myDevice.lastKnownLocation.valid = true;
            myDevice.lastKnownLocation.latitude = gpsplus.location.lat();
            myDevice.lastKnownLocation.longitude = gpsplus.location.lng();
            DEBUG_PRINTLN(F("Update location"));
        }

        if(gpsplus.time.isUpdated())
        {
            myDevice.lastKnownTime.valid = true;
            myDevice.lastKnownTime.hour = gpsplus.time.hour();
            myDevice.lastKnownTime.minute = gpsplus.time.minute();
            myDevice.lastKnownTime.second = gpsplus.time.second(); 
            DEBUG_PRINTF("Update Time: %02d:%02d\n", myDevice.lastKnownTime.hour, myDevice.lastKnownTime.minute);
            return false;
        }

        return true;
    }
    #ifdef DEBUG
    else {
        DEBUG_PRINTLN(F("Serial2 for GPS not available"));
        return false;
    }
    #endif
}

void UpdateGPSLoop(void *parameter)
{
    GPS *gps = (GPS *)parameter;
    for(;;) 
    {
        while(gps->GPSerialUpdate()) {
            vTaskDelay(pdMS_TO_TICKS(10)); // Small delay to avoid busy loop
        }
        vTaskDelay(pdMS_TO_TICKS(GPS_UPDATE_INTERVAL_MS)); // Update config MS 
    }
}

void TaskGPSUpdate()
{
    xTaskCreate(
        UpdateGPSLoop,
        "GPSUpdateTask",
        2048,
        &gps,
        1,
        NULL
    );
}
