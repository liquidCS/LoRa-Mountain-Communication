#include "gps.h"

#define GPS_BAUD 9600
#define GPS_TX 1 // PinTx for GPS module on ESP32 
#define GPS_RX 38 // PinRx for GPS module on EPS32
#define GPS_UPDATE_INTERVAL_MS 30000 // Update GPS data every 30 second

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
            myDevice.location.valid = true;
            myDevice.location.latitude = gpsplus.location.lat();
            myDevice.location.longitude = gpsplus.location.lng();
            DEBUG_PRINTF("Update location: Lat: %.6f, Lon: %.6f\n", myDevice.location.latitude, myDevice.location.longitude);
        }

        if(gpsplus.time.isUpdated())
        {
            myDevice.time.valid = true;
            myDevice.time.hour = gpsplus.time.hour();
            myDevice.time.minute = gpsplus.time.minute();
            myDevice.time.second = gpsplus.time.second(); 
            DEBUG_PRINTF("Update Time: %02d:%02d\n", myDevice.time.hour, myDevice.time.minute);
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
        4096,
        &gps,
        1,
        NULL
    );
}
