#include "gps.hpp"

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

        // Sometimes location and time are updated separately 
        // If signal is weak, only time may be updated 

        if(gpsplus.location.isUpdated())
        {
            // DEBUG_PRINTLN(F("Waiting for location update..."));
            // xSemaphoreTake(myDevice.location.xMutex, portMAX_DELAY); // protect location update
            myDevice.location.valid = true;
            myDevice.location.latitude = gpsplus.location.lat(); // Update location
            myDevice.location.longitude = gpsplus.location.lng();
            myDevice.location.attitude = gpsplus.altitude.meters();

            myDevice.location.updateTime.valid = true; // Update time of last location update
            myDevice.location.updateTime.hour = gpsplus.time.hour();
            myDevice.location.updateTime.minute = gpsplus.time.minute();
            myDevice.location.updateTime.second = gpsplus.time.second();
            // xSemaphoreGive(myDevice.location.xMutex);

            DEBUG_PRINTF("Update location: Lat: %.6f, Lon: %.6f Att: %.2f\n", myDevice.location.latitude, myDevice.location.longitude, myDevice.location.attitude);
        }

        if(gpsplus.time.isUpdated())
        {
            if (xSemaphoreTake(myDevice.time.xMutex, portMAX_DELAY) == pdTRUE) { // protect time update
                myDevice.time.valid = true;
                myDevice.time.hour = gpsplus.time.hour();
                myDevice.time.minute = gpsplus.time.minute();
                myDevice.time.second = gpsplus.time.second(); 
                xSemaphoreGive(myDevice.time.xMutex);
            } 

            DEBUG_PRINTF("Update Time: %02d:%02d\n", myDevice.time.hour, myDevice.time.minute);
            return false;
        }

        return true;
    }
    else {
        DEBUG_PRINTLN(F("Serial2 for GPS not available"));
        return false;
    }
}

void UpdateGPSLoop(void *parameter)
{
    GPS *gps = (GPS *)parameter;
    for(;;) 
    {
        while(gps->GPSerialUpdate()) {}
        vTaskDelay(pdMS_TO_TICKS(GPS_UPDATE_INTERVAL_MS)); // Update config MS 
    }
}

void TaskGPSUpdate()
{
    xTaskCreate(
        UpdateGPSLoop,
        "GPSUpdateTask",
        8192,
        &gps,
        1,
        NULL
    );
}
