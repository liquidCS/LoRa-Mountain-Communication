#include "gps.h"

#define GPS_BAUD 9600
#define GPS_TX 1 // PinTx for GPS module on ESP32 
#define GPS_RX 38 // PinRx for GPS module on EPS32

GPS gps;

GPS::GPS() 
{
    Serial2.begin(GPS_BAUD, SERIAL_8N1, GPS_RX, GPS_TX);
}

void GPS::ReadFromGPS()
{
    if(Serial2.available() > 0)
    {
        gpsplus.encode(Serial2.read());

        if(gpsplus.location.isUpdated())
        {
            validLocation = true;
            location.latitude = gpsplus.location.lat();
            location.longitude = gpsplus.location.lng();
            Serial.println(F("Update location"));
        }

        if(gpsplus.time.isUpdated())
        {
            validTime = true;
            time.hour = gpsplus.time.hour();
            time.minute = gpsplus.time.minute();
            time.second = gpsplus.time.second(); 
            Serial.print(F("Update time"));
            Serial.printf("Time: %02d:%02d:%02d\n", time.hour, time.minute, time.second);
        }
    }
}

void UpdateGPSLoop(void *parameter)
{
    GPS *gps = (GPS *)parameter;
    while(true)
    {
        gps->ReadFromGPS();
        vTaskDelay(10 / portTICK_PERIOD_MS); // Delay for 1 second
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
